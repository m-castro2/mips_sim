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
#include <QMainWindow>
#include <QTextStream>
#include <QFont>
#include <QLabel>
#include <QLineEdit>

#define DIA_CYCLES 5

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

private slots:
    void on_btnRun_clicked();
    void on_btnNext_clicked();
    void on_btnPrev_clicked();
    void on_btnReset_clicked();
    void on_actionLoad_file_triggered();
    void on_actionExit_triggered();

private:
    Ui::MipsSimGui *ui;

    QLabel *iregtext[32];
    QLabel *fregtext[32];
    QLabel *dregtext[32];
    QFont font, fontCurrent;

    std::vector<instruction_info_t> instructions;
    std::vector<QLabel *> dmem_labels;

    std::vector<diagram_info_t> diagram_lines;

    std::string asm_filename;
    bool file_loaded;
    std::shared_ptr<mips_sim::Memory> mem;
    std::unique_ptr<mips_sim::Cpu> cpu;

    bool load_file( const std::string &, std::ostream & );
    void set_cpu_labels();
    void set_dmem_labels();
    void set_tmem_labels();
    void set_diagram_labels();
};
#endif // MIPSSIMGUI_H
