#ifndef MIPS_SIM_FPCOPROCESSOR_H
#define MIPS_SIM_FPCOPROCESSOR_H

#include "../cpu/component/registers_bank.h"
#include "../../global_defs.h"
#include "../cpu/component/forwarding_unit.h"
#include "hardware_manager.h"

#include <memory>



namespace mips_sim
{
#define ADD_UNIT 0
#define MULT_UNIT 1
#define DIV_UNIT 2
#define GENERAL_UNIT 3

struct fp_unit {
    seg_reg_t seg_reg = {};
    int delay_s {};
    int delay_d {};
    int cycles_elapsed {};
    bool available {};
    int active_delay {}; //depends on instruction precision
    int type {};
};

//FCR31, Floating Point Control Register
typedef struct {
    uint8_t opcode; //7 bits, reserved, 0
    bool fs; // FP instruction
    bool c; //condition bit
    uint8_t cop; // 5 bits, reserved, 0
    uint8_t cause; // 6 bits E V Z O U I
    uint8_t enables; // 5 bits V Z O U I
    uint8_t flags; // 5 bits V Z O U I
    uint8_t rm; // 2 bits
} fp_coprocessor_control_status_register_format_t; 

class FPCoprocessor
{
    private:

        bool enabled {};

        fp_coprocessor_control_status_register_format_t ctrl_status_reg;

        std::vector<int> delays_s = {};
        std::vector<int> delays_d = {}; 

        std::vector<int> counts = {};

        std::vector<std::vector<std::shared_ptr<fp_unit>>> fp_units {}; //vector[type][unit]

        std::shared_ptr<FPRegistersBank> fpr_bank {};

        std::shared_ptr<ForwardingUnit> fu {};

        std::shared_ptr<HardwareManager> hardware_manager {};

        std::shared_ptr<std::vector<uint32_t>> dest_registers {}; //vector[type][regdst]

        std::shared_ptr<std::map<uint32_t, fpu_forwarding_value_t>> forwarding_registers {};

        std::vector<std::pair<uint32_t, uint32_t>> finished_forwarding_registers = {{}, {}};
        bool erase_finished_forwarding_registers = false;

    public:
  
        FPCoprocessor(std::vector<int> delays_s, std::vector<int> delays_d, std::vector<int> counts, 
                    std::shared_ptr<FPRegistersBank> fpr_bank, std::shared_ptr<ForwardingUnit> fu,
                    std::shared_ptr<HardwareManager> hardware_manager);
    
        ~FPCoprocessor();

        bool is_enabled();

        void set_enabled(bool value);

        seg_reg_t work();

        bool is_unit_available(int unit_type);

        void set_seg_reg(int unit_type, seg_reg_t seg_reg);

        void fp_unit_compute(std::shared_ptr<fp_unit> unit);

        void reset();

        uint32_t get_conditional_bit();

        void status_update();

        std::shared_ptr<std::vector<uint32_t>> get_dest_registers();

        std::shared_ptr<std::map<uint32_t, fpu_forwarding_value_t>> get_forwarding_registers();
};

} /* namespace */

#endif
