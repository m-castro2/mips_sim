#include "cpu_flex.h"

#include "stages/stage_if.h"
#include "stages/stage_id.h"
#include "stages/stage_ex.h"
#include "stages/stage_mem.h"
#include "stages/stage_wb.h"


namespace mips_sim {

    CpuFlex::CpuFlex(std::shared_ptr<Memory> _memory,
                 std::shared_ptr<ControlUnit> _control_unit,
                 int branch_type,
                 int branch_stage,
                 bool has_forwarding_unit,
                 bool has_hazard_detection_unit)
    : CpuPipelined(_memory)
    {   
        StageIF* if_stage = new StageIF(memory);
        StageID* id_stage = new StageID();
        StageEX* ex_stage = new StageEX(status["mult-delay"], status["div-delay"]);
        StageMEM* mem_stage = new StageMEM(memory);
        StageWB* wb_stage = new StageWB();

        add_cpu_stage(if_stage);
        add_cpu_stage(id_stage);
        add_cpu_stage(ex_stage);
        add_cpu_stage(mem_stage);
        add_cpu_stage(wb_stage);

        next_cycle(std::cout);
    }

  
    CpuFlex::~CpuFlex() {}

    std::vector<CpuStage*> CpuFlex::get_cpu_stages() {
        return cpu_stages;
    }

    void CpuFlex::add_cpu_stage(CpuStage* stage) {
        cpu_stages.push_back(stage);
    }

    bool CpuFlex::next_cycle( std::ostream &out )
    {
        Cpu::next_cycle( out );

        StageIF* stage_if = dynamic_cast<StageIF*>(cpu_stages.at(0));
        StageID* stage_id = dynamic_cast<StageID*>(cpu_stages.at(1));
        StageEX* stage_ex = dynamic_cast<StageEX*>(cpu_stages.at(2));
        StageMEM* stage_mem = dynamic_cast<StageMEM*>(cpu_stages.at(3));
        StageWB* stage_wb = dynamic_cast<StageWB*>(cpu_stages.at(4));

        stage_if->work_h();
        stage_id->work_h();
        stage_ex->work_h();
        stage_mem->work_h();
        stage_wb->work_h();

        stage_if->work_l();
        stage_id->work_l();
        stage_ex->work_l();
        stage_mem->work_l();
        stage_wb->work_l();

        return true;
    }


} //namespace