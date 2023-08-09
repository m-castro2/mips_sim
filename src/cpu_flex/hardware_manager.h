#ifndef MIPS_SIM_HARDWARE_MANAGER_H
#define MIPS_SIM_HARDWARE_MANAGER_H

#include <map>
#include <string_view>

namespace mips_sim {
    
    class HardwareManager
    {   
        private:

            std::map<std::string_view, int> status {};

        public:

            HardwareManager();
            
            ~HardwareManager();

            int get_status(std::string_view key);

            bool set_status(std::string_view key, int value);

            int get_signal(std::string_view key);

            bool set_signal(std::string_view key, int value);

    };
} //namespace
#endif