#include "hardware_manager.h"

#include <string_view>

namespace mips_sim {

    HardwareManager::HardwareManager()
    {

    }
  
    HardwareManager::~HardwareManager() {}


    int HardwareManager::get_status(std::string_view key) {
        return status[key];
    }

    bool HardwareManager::set_status(std::string_view key, int value) {
        status[key] = value;
        return true;
    }

    int HardwareManager::get_signal(std::string_view key) {
        return 0;
    }

    bool HardwareManager::set_signal(std::string_view key, int value) {
        return 0;
    }

} //namespace