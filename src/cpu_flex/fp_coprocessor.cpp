#include "fp_coprocessor.h"
#include "../global_defs.h"
#include "../utils.h"

#include <iostream>

namespace mips_sim
{

  
    FPCoprocessor::FPCoprocessor(std::vector<int> p_delays_s, std::vector<int> p_delays_d, std::vector<int> p_counts,
                                std::shared_ptr<FPRegistersBank> p_fpr_bank, std::shared_ptr<ForwardingUnit> p_fu,
                                std::shared_ptr<HardwareManager> p_hardware_manager)
    {
        fpr_bank = p_fpr_bank;
        fu = p_fu;
        hardware_manager = p_hardware_manager;

        delays_s = p_delays_s;
        delays_d = p_delays_d;
        counts = p_counts;

        ctrl_status_reg = {};

        std::vector<std::shared_ptr<fp_unit>> v {};
        for (int i=0; i<counts[ADD_UNIT]; ++i) {
            std::shared_ptr<fp_unit> add = std::shared_ptr<fp_unit>(new fp_unit({{}, delays_s[ADD_UNIT], delays_d[ADD_UNIT], 0, true, 0, ADD_UNIT}));
            v.push_back(add);
        }
        fp_units.push_back(v);

        v.clear();
        for (int i=0; i<counts[MULT_UNIT]; ++i) {
            std::shared_ptr<fp_unit> mult = std::shared_ptr<fp_unit>(new fp_unit({{}, delays_s[MULT_UNIT], delays_d[MULT_UNIT], 0, true, 0, MULT_UNIT}));
            v.push_back(mult);
        }
        fp_units.push_back(v);

        v.clear();
        for (int i=0; i<counts[DIV_UNIT]; ++i) {
            std::shared_ptr<fp_unit> div = std::shared_ptr<fp_unit>(new fp_unit({{}, delays_s[DIV_UNIT], delays_d[DIV_UNIT], 0, true, 0, DIV_UNIT}));
            v.push_back(div);
        }
        fp_units.push_back(v);
        
        v.clear();
        //3 should be enough since max delay is 2
        for (int i=0; i<3; ++i) {
            std::shared_ptr<fp_unit> gen = std::shared_ptr<fp_unit>(new fp_unit({{}, 0, 0, 0, true, 0, GENERAL_UNIT}));
            v.push_back(gen);
        }
        fp_units.push_back(v);

        dest_registers = std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>({}));

        forwarding_registers = std::shared_ptr<std::map<uint32_t, fpu_forwarding_value_t>>(new std::map<uint32_t, fpu_forwarding_value_t>({}));

        status_update();
    }
    
    FPCoprocessor::~FPCoprocessor() {

    }
    
    
    bool FPCoprocessor::is_enabled() {
        return enabled;
    }

    void FPCoprocessor::set_enabled(bool value) {
        enabled = value;
    }

    uint32_t FPCoprocessor::get_available_units() {
        uint32_t bit_field = 0;
        for (int unit_type = ADD_UNIT; unit_type < 4; ++unit_type) {
            for (std::shared_ptr<fp_unit> unit: fp_units[unit_type]) {
                if (unit->available)
                    bit_field |= 1 << unit_type;
            }
        }
        return bit_field;
    }

    seg_reg_t FPCoprocessor::work() {
        if (condition_delay)
            --condition_delay;
        ++cycle;
        std::cout << "FPCoprocessor Stage:" << std::endl;
        int max_delay = 0; //identify the first issued instruction
        std::shared_ptr<fp_unit> finished_unit = nullptr;

        seg_reg_t output_seg_reg = {};

        for (std::vector<std::shared_ptr<fp_unit>> unit_type: fp_units) {
            for (std::shared_ptr<fp_unit> unit: unit_type) {
                if (unit->available) { //empty unit
                    continue;
                }

                std::cout << "  FPCoprocessor"; 
                switch (std::find(fp_units.begin(), fp_units.end(), unit_type) - fp_units.begin())
                {
                case ADD_UNIT:
                    std::cout << " Add Unit"; break;
                case MULT_UNIT:
                    std::cout << " Mult Unit"; break;
                case DIV_UNIT:
                    std::cout << " Div Unit"; break;
                default: break;
                }
                std::cout << ": " << Utils::decode_instruction(unit->seg_reg.data[SR_INSTRUCTION]) << std::endl;
                
                //if instruction was just received, compute
                if (unit->cycles_elapsed == 0 ) {
                    fp_unit_compute(unit);
                }

                //if instruction is done
                if (unit->cycles_elapsed == unit->active_delay) {
                    if (unit->active_delay > max_delay){ // first issued instruction has priority
                        max_delay = unit->active_delay;
                        finished_unit = unit;
                    }
                    if (unit->type != 3 || unit->seg_reg.data[SR_FUNCT] == SUBOP_FPMOV){ //compare have no regdest
                        forwarding_registers->at(unit->seg_reg.data[SR_REGDEST]).ready = true; // set result as ready for HDU
                        if (unit->seg_reg.data[SR_FPPRECISION]) { // if instruction is double precision, set paired register as ready as well
                            forwarding_registers->at(unit->seg_reg.data[SR_REGDEST]+1).ready = true;
                        }
                    }
                }

                if (unit->cycles_elapsed < unit->active_delay) {
                    unit->cycles_elapsed++;
                }
            }
        }

        if (finished_unit != nullptr) {
            //send data to MEM
            finished_unit->cycles_elapsed = 0;
            finished_unit->available = true;
            if (finished_unit->type < 3 || finished_unit->seg_reg.data[SR_FUNCT] == SUBOP_FPMOV) { // comparisons dont need reg dest, ¿TODO MOV?
                auto position = std::find(dest_registers.get()->begin(), dest_registers.get()->end(), finished_unit->seg_reg.data[SR_REGDEST]);
                dest_registers.get()->erase(position);

                finished_forwarding_registers[0] = std::make_tuple(finished_unit->seg_reg.data[SR_REGDEST], finished_unit->seg_reg.data[SR_ALUOUTPUT], cycle);
                finished_forwarding_registers[1] = std::make_tuple(finished_unit->seg_reg.data[SR_REGDEST]+1, finished_unit->seg_reg.data[SR_FPOUTPUTUPPER], cycle);
                erase_finished_forwarding_registers = true;
            }
            output_seg_reg = finished_unit->seg_reg;
        }

        if (erase_finished_forwarding_registers 
                && forwarding_registers->at(std::get<0>(finished_forwarding_registers[0])).ready
                && forwarding_registers->at(std::get<0>(finished_forwarding_registers[0])).cycle == std::get<2>(finished_forwarding_registers[0])) {
            if (forwarding_registers->at(std::get<0>(finished_forwarding_registers[0])).just_finished){
                forwarding_registers->at(std::get<0>(finished_forwarding_registers[0])).just_finished = false; //dont erase until next cycle so it can be forwarded to other FP units
            }
            else {
                forwarding_registers->erase(std::get<0>(finished_forwarding_registers[0]));
                forwarding_registers->erase(std::get<0>(finished_forwarding_registers[1]));
                finished_forwarding_registers = {{},{},{}};
                erase_finished_forwarding_registers = false;
            }
        }



        return output_seg_reg;
    }

    void FPCoprocessor::set_seg_reg(int unit_type, seg_reg_t next_seg_reg) {
        for (std::shared_ptr<fp_unit> unit: fp_units.at(unit_type)) {
            if (unit->available) {
                hardware_manager->add_to_fp_coprocessor_active_instructions_count(1);
                for (int i=0; i < 32; ++i) {
                    unit->seg_reg.data[i] = next_seg_reg.data[i];
                }

                if (unit_type == 3) {
                    if (next_seg_reg.data[SR_FUNCT] == SUBOP_FPMOV) {
                        unit->active_delay = 1;
                        dest_registers->push_back(unit->seg_reg.data[SR_REGDEST]);
                    }
                    else { //CEQ, CLE, CLT
                        unit->active_delay = 2;
                    }
                    unit->cycles_elapsed = 0;
                    unit->available = false;
                    return;
                }

                unit->cycles_elapsed = 0;
                unit->available = false;
                unit->active_delay = next_seg_reg.data[SR_FPPRECISION] ? unit->delay_d : unit->delay_s;

                if (unit_type < 3){ // comparisons dont need reg dest
                    dest_registers->push_back(next_seg_reg.data[SR_REGDEST]);
                }
                return;
            }      
        }

        if (unit_type == GENERAL_UNIT) { // if all are busy add a new one
            std::shared_ptr<fp_unit> gen = std::shared_ptr<fp_unit>(new fp_unit({{}, 0, 0, 0, true, 0, GENERAL_UNIT}));
            fp_units.at(GENERAL_UNIT).push_back(gen);
            hardware_manager->add_to_fp_coprocessor_active_instructions_count(1);
        }

    }

    void FPCoprocessor::fp_unit_compute(std::shared_ptr<fp_unit> unit) {
        uint32_t rs_value = unit->seg_reg.data[SR_RSVALUE];
        uint32_t rs_value_upper = unit->seg_reg.data[SR_FPRSVALUEUPPER];
        uint32_t rt_value = unit->seg_reg.data[SR_RTVALUE];
        uint32_t rt_value_upper = unit->seg_reg.data[SR_FPRTVALUEUPPER];

        if (fu->is_enabled()) {
            uint32_t rs = unit->seg_reg.data[SR_RS];
            uint32_t rt = unit->seg_reg.data[SR_RT];
            
            rs_value = fu->forward_register(rs, rs_value, true, std::cout, true);
            rs_value_upper = fu->forward_register(rs+1, rs_value_upper, true, std::cout, true);
            rt_value = fu->forward_register(rt, rt_value, true, std::cout, true);
            rt_value_upper = fu->forward_register(rt+1, rt_value_upper, true, std::cout, true);
        }

        uint32_t rs_words[] = {rs_value, rs_value_upper};
        uint32_t rt_words[] = {rt_value, rt_value_upper};

        uint32_t outputs[2];

        switch (unit->seg_reg.data[SR_FUNCT])
        {
        case SUBOP_FPADD:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::double_to_word(Utils::word_to_double(rs_words) +
                                    Utils::word_to_double(rt_words), outputs);
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                Utils::float_to_word(float_rs + float_rt, outputs);
            }
            break;
        case SUBOP_FPSUB:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::double_to_word(Utils::word_to_double(rs_words) -
                                    Utils::word_to_double(rt_words), outputs);
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                Utils::float_to_word(float_rs - float_rt, outputs);
            }
            break;
        case SUBOP_FPMUL:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::double_to_word(Utils::word_to_double(rs_words) *
                                    Utils::word_to_double(rt_words), outputs);
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                Utils::float_to_word(float_rs * float_rt, outputs);
            }
            break;
        case SUBOP_FPDIV:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::double_to_word(Utils::word_to_double(rs_words) /
                                    Utils::word_to_double(rt_words), outputs);
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                Utils::float_to_word(float_rs / float_rt, outputs);
            }
            break; 
        case SUBOP_FPMOV:
            // MOV are encoded so they use RT as source
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::double_to_word(Utils::word_to_double(rt_words), outputs); 
            }
            else {
                float value = Utils::word_to_float(rt_words); // read as double
                Utils::float_to_word(value, outputs); // from double
            }
            break;
        case SUBOP_FPCEQ:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                ctrl_status_reg.c = (Utils::word_to_double(rs_words) == Utils::word_to_double(rt_words));
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                ctrl_status_reg.c = (float_rs == float_rt);
            }
            condition_delay = 2;
            return;
            break;
        case SUBOP_FPCLE:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                ctrl_status_reg.c = (Utils::word_to_double(rs_words) <= Utils::word_to_double(rt_words));
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                ctrl_status_reg.c = (float_rs <= float_rt);
            }
            condition_delay = 2;
            return;
            break;
        case SUBOP_FPCLT:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                ctrl_status_reg.c = (Utils::word_to_double(rs_words) < Utils::word_to_double(rt_words));
            }
            else {
                float float_rs = (float) Utils::word_to_float(rs_words);
                float float_rt = (float) Utils::word_to_float(rt_words);
                ctrl_status_reg.c = (float_rs < float_rt);
            }
            condition_delay = 2;
            return;
            break;
        default:
            return;
            break;
        }

        unit->seg_reg.data[SR_ALUOUTPUT] = outputs[0];
        unit->seg_reg.data[SR_FPOUTPUTUPPER] = outputs[1];

        // add finished instructions to possible values to forward
        uint32_t reg_dest = unit->seg_reg.data[SR_REGDEST];

        auto inserted = forwarding_registers->emplace(reg_dest, fpu_forwarding_value_t({outputs[0], false, true, cycle}));
        if (!inserted.second) {
            forwarding_registers->at(reg_dest) = fpu_forwarding_value_t({outputs[0], false, true, cycle});
        }
        inserted = forwarding_registers->emplace(reg_dest + 1, fpu_forwarding_value_t({outputs[1], false, true, cycle}));
        if (!inserted.second) {
            forwarding_registers->at(reg_dest+1) = fpu_forwarding_value_t({outputs[1], false, true, cycle});
        }
    }

    void FPCoprocessor::reset() {
        //TODO reset mov, bc
        for(std::vector<std::shared_ptr<fp_unit>> type_vector: fp_units) {
            for (std::shared_ptr<fp_unit> unit: type_vector) {
                unit->seg_reg = {};
                unit->available = true;
                unit->cycles_elapsed = 0;
            }   
        }
        dest_registers.get()->clear();
        erase_finished_forwarding_registers = false;
        forwarding_registers->clear();

        cycle = 0;
    }

    uint32_t FPCoprocessor::get_conditional_bit() {
        return static_cast<uint32_t>(ctrl_status_reg.c);
    }

    void FPCoprocessor::status_update() {
        /* bind functions */
        hardware_manager->set_signal(SIGNAL_FPCOND, std::bind(&FPCoprocessor::get_conditional_bit, this));
        hardware_manager->set_signal(SIGNAL_FP_UNIT_AVAIL, std::bind(&FPCoprocessor::get_available_units, this));
        hardware_manager->set_signal(SIGNAL_FPCONDRD, std::bind(&FPCoprocessor::get_conditional_bit_ready, this));
    }

    std::shared_ptr<std::vector<uint32_t>> FPCoprocessor::get_dest_registers(){
        return dest_registers;
    }

    std::shared_ptr<std::map<uint32_t, fpu_forwarding_value_t>> FPCoprocessor::get_forwarding_registers() {
        return forwarding_registers;
    }

    uint32_t FPCoprocessor::get_conditional_bit_ready(){
        return (condition_delay == 0);
    }

} /* namespace */
