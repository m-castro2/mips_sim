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

    bool FPCoprocessor::is_unit_available(int unit_type) {
        for (std::shared_ptr<fp_unit> unit: fp_units[unit_type]) {
            if (unit->available)
                return true;
        }
        return false;
    }

    seg_reg_t FPCoprocessor::work() {
        if (erase_finished_forwarding_registers 
                && forwarding_registers->at(finished_forwarding_registers[0].first).ready
                && forwarding_registers->at(finished_forwarding_registers[0].first).value == finished_forwarding_registers[0].second) {
            
            forwarding_registers->erase(finished_forwarding_registers[0].first);
            forwarding_registers->erase(finished_forwarding_registers[1].first);
            finished_forwarding_registers = {{},{}};
            erase_finished_forwarding_registers = false;
        }
        std::cout << "FPCoprocessor Stage:" << std::endl;
        int max_delay = 0; //identify the first issued instruction
        std::shared_ptr<fp_unit> finished_unit = nullptr;

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
                    forwarding_registers->at(unit->seg_reg.data[SR_REGDEST]).ready = true; // set result as ready for HDU
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
            if (finished_unit->type < 3) { // comparisons dont need reg dest, ¿TODO MOV?
                auto position = std::find(dest_registers.get()->begin(), dest_registers.get()->end(), finished_unit->seg_reg.data[SR_REGDEST]);
                dest_registers.get()->erase(position);

                finished_forwarding_registers[0] = std::make_pair(finished_unit->seg_reg.data[SR_REGDEST], finished_unit->seg_reg.data[SR_ALUOUTPUT]);
                finished_forwarding_registers[1] = std::make_pair(finished_unit->seg_reg.data[SR_REGDEST + 1], finished_unit->seg_reg.data[SR_FPOUTPUTUPPER]);
                erase_finished_forwarding_registers = true;
            }
            return finished_unit->seg_reg;
        }

        return {};
    }

    void FPCoprocessor::set_seg_reg(int unit_type, seg_reg_t next_seg_reg) {
        for (std::shared_ptr<fp_unit> unit: fp_units.at(unit_type)) {
            if (unit->available) {
                for (int i=0; i < 32; ++i) {
                    unit->seg_reg.data[i] = next_seg_reg.data[i];
                }

                if (unit_type == 3) {
                    if (next_seg_reg.data[SR_FUNCT] == SUBOP_FPMOV) {
                        unit->active_delay = 1;
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
                    dest_registers.get()->push_back(next_seg_reg.data[SR_REGDEST]);
                }
                return;
            }      
        }

        if (unit_type == GENERAL_UNIT) { // if all are busy add a new one
            std::shared_ptr<fp_unit> gen = std::shared_ptr<fp_unit>(new fp_unit({{}, 0, 0, 0, true, 0}));
            fp_units.at(GENERAL_UNIT).push_back(gen);
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
            rt_value = fu->forward_register(rt, rt_value, true, std::cout);
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
                Utils::float_to_word(Utils::word_to_float(rs_words) +
                                    Utils::word_to_float(rt_words), outputs);
            }
            break;
        case SUBOP_FPSUB:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::double_to_word(Utils::word_to_double(rs_words) -
                                    Utils::word_to_double(rt_words), outputs);
            }
            else {
                Utils::float_to_word(Utils::word_to_float(rs_words) -
                                    Utils::word_to_float(rt_words), outputs);
            }
            break;
        case SUBOP_FPMUL:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::float_to_word(Utils::word_to_float(rs_words) *
                                    Utils::word_to_float(rt_words), outputs);
            }
            else {
                Utils::float_to_word(Utils::word_to_float(rs_words) *
                                    Utils::word_to_float(rt_words), outputs);
            }
            break;
        case SUBOP_FPDIV:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                Utils::float_to_word(Utils::word_to_float(rs_words) /
                                    Utils::word_to_float(rt_words), outputs);
            }
            else {
                Utils::float_to_word(Utils::word_to_float(rs_words) /
                                    Utils::word_to_float(rt_words), outputs);
            }
            break;
        case SUBOP_FPCEQ:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                ctrl_status_reg.c = (Utils::word_to_double(rs_words) == Utils::word_to_double(rt_words));
            }
            else {
                ctrl_status_reg.c = (Utils::word_to_float(rs_words) == Utils::word_to_float(rt_words));
            }
            return;
            break;
        case SUBOP_FPCLE:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                ctrl_status_reg.c = (Utils::word_to_double(rs_words) <= Utils::word_to_double(rt_words));
            }
            else {
                ctrl_status_reg.c = (Utils::word_to_float(rs_words) <= Utils::word_to_float(rt_words));
            }
            return;
            break;
        case SUBOP_FPCLT:
            if (unit->seg_reg.data[SR_FPPRECISION]) {
                ctrl_status_reg.c = (Utils::word_to_double(rs_words) < Utils::word_to_double(rt_words));
            }
            else {
                ctrl_status_reg.c = (Utils::word_to_float(rs_words) < Utils::word_to_float(rt_words));
            }
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
        forwarding_registers->emplace(reg_dest, fpu_forwarding_value_t({outputs[0], false}));
        forwarding_registers->emplace(reg_dest + 1, fpu_forwarding_value_t({outputs[1], false}));
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
    }

    uint32_t FPCoprocessor::get_conditional_bit() {
        return static_cast<uint32_t>(ctrl_status_reg.c);
    }

    void FPCoprocessor::status_update() {
        /* bind functions */
        hardware_manager->set_signal(SIGNAL_FPCOND, std::bind(&FPCoprocessor::get_conditional_bit, this));
    }

    std::shared_ptr<std::vector<uint32_t>> FPCoprocessor::get_dest_registers(){
        return dest_registers;
    }

    std::shared_ptr<std::map<uint32_t, fpu_forwarding_value_t>> FPCoprocessor::get_forwarding_registers() {
        return forwarding_registers;
    }

} /* namespace */
