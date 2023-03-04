#include "mips_sim_settings.h"
#include "ui_mips_sim_settings.h"

using namespace std;

MipsSimSettings::MipsSimSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MipsSimSettings)
{
    ui->setupUi(this);
    
    pwindow = static_cast<MipsSimGui *>(parent);
    status = pwindow->get_cpu_status();
    
    load_status();
}

MipsSimSettings::~MipsSimSettings()
{
    delete ui;
}

void MipsSimSettings::load_status( void )
{
    ui->cbForwarding->setChecked(status["has-forwarding-unit"]);
    ui->cbHazardDetection->setChecked(status["has-hazard-detection-unit"]);
    
    switch(status["branch-type"])
    {
      case 0: /* flush */
        ui->radBTypeFlush->setChecked(true); break;
      case 1: /* fixed non-taken */
        ui->radBTypeNonTaken->setChecked(true); break;
      case 2: /* default */
        ui->radBTypeDelayed->setChecked(true); break;
      default:
        assert(0);
    }
    
    switch(status["branch-stage"])
    {
      case 1: /* branch at ID stage */
        ui->radBStageID->setChecked(true); break;
      case 3: /* branch at MEM stage */
        ui->radBStageMEM->setChecked(true); break;
      default:
        assert(0);
    }
}

map<string, int> MipsSimSettings::save_status ( void ) const
{
  map<string, int> saved_status;
  
  saved_status["has-forwarding-unit"] = ui->cbForwarding->isChecked();
  saved_status["has-hazard-detection-unit"] = ui->cbHazardDetection->isChecked();

  if (ui->radBTypeFlush->isChecked())
    saved_status["branch-type"] = 0;
  else if (ui->radBTypeNonTaken->isChecked())
    saved_status["branch-type"] = 1;
  else if (ui->radBTypeDelayed->isChecked())
    saved_status["branch-type"] = 2;

  if (ui->radBStageID->isChecked())
    saved_status["branch-stage"] = 1;
  else if (ui->radBStageMEM->isChecked())
    saved_status["branch-stage"] = 3;
        
  return saved_status;
}

void MipsSimSettings::on_buttonBox_accepted()
{
  map<string, int> new_status = save_status();
  pwindow->set_cpu_status(new_status);
}
