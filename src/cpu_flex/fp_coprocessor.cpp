#include "fp_coprocessor.h"
#include "../global_defs.h"
#include "../utils.h"

#include <iostream>

namespace mips_sim
{

  
    FPCoprocessor::FPCoprocessor(std::vector<int> p_delays, std::vector<int> p_counts, std::shared_ptr<FPRegistersBank> p_fpr_bank)
    {
        fpr_bank = p_fpr_bank;

        delays = p_delays;
        counts = p_counts;

        for (int i=0; i<3; ++i) { //initialize vec
            fp_units.push_back({nullptr});
        }

        for (int i=0; i<counts[ADD_UNIT]; ++i) {
            std::shared_ptr<fp_unit> add = std::shared_ptr<fp_unit>(new fp_unit({{}, delays[ADD_UNIT], 0, true}));
            fp_units[ADD_UNIT][i] = add;
        }
        for (int i=0; i<counts[MULT_UNIT]; ++i) {
            std::shared_ptr<fp_unit> mult = std::shared_ptr<fp_unit>(new fp_unit({{}, delays[MULT_UNIT], 0, true}));
            fp_units[MULT_UNIT][i] = mult;
        }
        for (int i=0; i<counts[DIV_UNIT]; ++i) {
            std::shared_ptr<fp_unit> div = std::shared_ptr<fp_unit>(new fp_unit({{}, delays[DIV_UNIT], 0, true}));
            fp_units[DIV_UNIT][i] = div;
        }
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

    void FPCoprocessor::work() {
        int max_delay = 0; //identify the first issued instruction
        std::shared_ptr<fp_unit> finished_unit = nullptr;

        for (std::vector<std::shared_ptr<fp_unit>> unit_type: fp_units) {
            for (std::shared_ptr<fp_unit> unit: unit_type) {
                if (unit->available) { //empty unit
                    continue;
                }
                
                //if instruction was just received, compute
                if (unit->cycles_elapsed == 0 ) {
                    fp_unit_compute(unit);
                    std::cout << unit->seg_reg.data[SR_ALUOUTPUT] << "\n";
                    std::cout << unit->seg_reg.data[SR_FPOUTPUTUPPER] << "\n";
                }

                if (unit->cycles_elapsed < unit->delay) {
                    unit->cycles_elapsed++;
                }
                //if instruction is done
                if (unit->cycles_elapsed == unit->delay) {
                    if (unit->delay > max_delay){ // first issued instruction has priority
                        max_delay = unit->delay;
                        finished_unit = unit;
                    }
                }
            }
        }

        if (finished_unit != nullptr) {
            //send data to MEM
            finished_unit->cycles_elapsed = 0;
            finished_unit->available = true;
        }
    }

    void FPCoprocessor::set_seg_reg(int unit_type, seg_reg_t next_seg_reg) {
        for (std::shared_ptr<fp_unit> unit: fp_units[unit_type]) {
            if (unit->available) {
                for (int i=0; i < 32; ++i) {
                    unit->seg_reg.data[i] = next_seg_reg.data[i];
                }

                unit->available = false;
            }
        }
    }

    void FPCoprocessor::fp_unit_compute(std::shared_ptr<fp_unit> unit) {
        uint32_t rs_words[] = {unit->seg_reg.data[SR_RSVALUE], unit->seg_reg.data[SR_FPRSVALUEUPPER]};
        uint32_t rt_words[] = {unit->seg_reg.data[SR_RTVALUE], unit->seg_reg.data[SR_FPRTVALUEUPPER]};

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
                Utils::float_to_word(Utils::word_to_float(rs_words) +
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
        default:
            break;
        }

        unit->seg_reg.data[SR_ALUOUTPUT] = outputs[0];
        unit->seg_reg.data[SR_FPOUTPUTUPPER] = outputs[1];
    }

} /* namespace */
