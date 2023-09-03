#include "hardware_manager.h"

#include <string_view>

namespace mips_sim {

    HardwareManager::HardwareManager(int p_branch_type, int p_branch_stage)
    : branch_type { p_branch_type}, branch_stage { p_branch_stage } 
    {
        
    }
  
    HardwareManager::~HardwareManager() {}

    int HardwareManager::get_branch_type() {
        return branch_type;
    }

    int HardwareManager::get_branch_stage() {
        return branch_stage;
    }

    int HardwareManager::get_status(int key) {
        return status[key];
    }

    bool HardwareManager::set_status(int key, int value) {
        status[key] = value;
        return true;
    }

    std::function<uint32_t( void )> HardwareManager::get_signal(std::string_view key)
    {
        return cpu_signals[key];
    }

    void HardwareManager::set_signal(std::string_view key, std::function<uint32_t( void )> signal_function)
    {
        cpu_signals[key] = signal_function;
    }

    void HardwareManager::reset() {
        for (int i = 0; i < status.size(); ++i) {
            status[i] = -1;
        }
        fp_stall = false;
    }

    void HardwareManager::set_stage_instruction(int stage, uint32_t instruction) {
        stage_instructions[stage] = instruction;
    }

    uint32_t HardwareManager::get_stage_instruction(int stage) {
        return stage_instructions[stage];
    }

    bool HardwareManager::get_fp_stall(){
        return fp_stall;
    }

    void HardwareManager::set_fp_stall(bool value) {
        fp_stall = value;
    }


} //namespace