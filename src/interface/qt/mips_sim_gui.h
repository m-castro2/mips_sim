#ifndef MIPSSIMGUI_H
#define MIPSSIMGUI_H

#include "../../assembler/mips_assembler.h"
#include "../../assembler/mips_parser.hpp"
#include "../../cpu/cpu.h"
#include "../../mem.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <QDialog>
#include <QMainWindow>
#include <QTextStream>
#include <QFont>
#include <QLabel>
#include <QLineEdit>

#define MAX_DIA_CYCLES 10
#define MAX_COLORS      5

// static std::string colors[MAX_COLORS] = {"ForestGreen", "DarkRed", "DarkBlue",
//                                          "Chocolate", "SaddleBrown", "Indigo",
//                                          "BlueViolet"};
static std::string colors[MAX_COLORS] = {"#FFBEBC", "#85E3FF", "#AFF8D8",
                                         "#FFFFD1", "#FFCCF9"};

QT_BEGIN_NAMESPACE
namespace Ui { class MipsSimGui; }
QT_END_NAMESPACE

typedef struct
{
  uint32_t pc_address;
  QLabel * lblName;
} instruction_info_t;

typedef struct
{
  uint32_t pc_address;
  QLabel * lblName;
  std::vector<QLabel *> lblStages;
} diagram_info_t;

class MipsSimGui : public QMainWindow
{
    Q_OBJECT

public:
    MipsSimGui(QWidget *parent = nullptr);
    ~MipsSimGui();

    const std::map<std::string, int> get_cpu_status( void ) const;
    void set_cpu_status( std::map<std::string, int> new_status );
    
private slots:
    void on_btnRun_clicked();
    void on_btnNext_clicked();
    void on_btnPrev_clicked();
    void on_btnReset_clicked();
    void on_actionLoad_file_triggered();
    void on_actionExit_triggered();
    void on_actionSettings_triggered();
    
private:
    Ui::MipsSimGui *ui;

    QLabel *iregtext[32];
    QLabel *fregtext[32];
    QLabel *dregtext[32];
    QFont font, fontCurrent;

    std::vector<instruction_info_t> instructions;
    std::vector<QLabel *> dmem_labels;

    std::vector<diagram_info_t> diagram_lines;
    std::vector<QLabel *> dcycle_labels;

    std::string asm_filename;
    bool file_loaded;
    std::shared_ptr<mips_sim::Memory> mem;
    std::unique_ptr<mips_sim::Cpu> cpu;

    bool load_file( const std::string &, std::ostream & );
    void set_cpu_labels();
    void set_dmem_labels();
    void set_tmem_labels();
    void set_diagram_labels();
    
    void handle_exception(int e);
    
    QDialog * settings_window;
};
#endif // MIPSSIMGUI_H
