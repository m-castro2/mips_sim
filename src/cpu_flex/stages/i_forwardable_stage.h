#ifndef MIPS_SIM_FORWARDABLE_STAGE_H
#define MIPS_SIM_FORWARDABLE_STAGE_H


namespace mips_sim {
    
    class IForwardableStage {
        
        public:
            
            virtual ~IForwardableStage() = default;

            virtual bool forward_register(int regId, int regValue) = 0;

    };

} //namespace
#endif