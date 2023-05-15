#ifndef MIPS_SIM_SETTINGS_H
#define MIPS_SIM_SETTINGS_H

#include "mips_sim_gui.h"

#include <map>
#include <string>
#include <QtWidgets/QDialog>

namespace Ui {
class MipsSimSettings;
}

class MipsSimSettings : public QDialog
{
    Q_OBJECT

public:
    explicit MipsSimSettings(QWidget *parent = nullptr);
    ~MipsSimSettings();

private slots:
    void on_buttonBox_accepted();
    
private:
    Ui::MipsSimSettings *ui;
    MipsSimGui * pwindow;
    
    void load_status ( void );
    std::map<std::string, int> save_status ( void ) const;
    
    std::map<std::string, int> status;
};

#endif // MIPS_SIM_SETTINGS_H
