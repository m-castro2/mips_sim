#include "mips_sim_gui.h"
#include "ui_mips_sim_gui.h"
#include "mips_sim_settings.h"
#include "../../assembler/mips_assembler.h"
#include "../../cpu/cpu_multi.h"
#include "../../cpu/cpu_pipelined.h"
#include "../../utils.h"
#include "../../exception.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <QtCore/QString>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

using namespace std;
using namespace mips_sim;

void MipsSimGui::set_dmem_labels()
{
  uint32_t start = MEM_DATA_START;
  uint32_t length = MEM_DATA_SIZE;
  QLabel * dmem_label = nullptr;
  size_t lines = 0;
  stringstream out;

  try
  {
    for (uint32_t mem_addr=start; mem_addr<start+length; mem_addr+=16)
    {
      if (dmem_labels.size() <= lines)
      {
        dmem_label = new QLabel(ui->scrollMemData);
        dmem_label->setMinimumSize(QSize(30, 15));
        dmem_label->setMaximumSize(QSize(16777215, 15));
        dmem_label->setFont(font);
        dmem_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        ui->scrollMemData->layout()->addWidget(dmem_label);
        dmem_labels.push_back(dmem_label);
      }
      else
      {
        dmem_label = dmem_labels[lines];
      }

      lines++;
      out.str(std::string());
      uint32_t word = mem->mem_read_32(mem_addr);
      out << "[" << Utils::hex32(mem_addr) << "] " << Utils::hex32(word);
      word = mem->mem_read_32(mem_addr + 4);
      out << " " << Utils::hex32(word);
      word = mem->mem_read_32(mem_addr + 8);
      out << " " << Utils::hex32(word);
      word = mem->mem_read_32(mem_addr + 12);
      out << " " << Utils::hex32(word);
      dmem_label->setText(out.str().c_str());
    }
  }
  catch (int)
  {
    assert(dmem_label);
    dmem_label->setText(out.str().c_str());
    /* ignore */
  }

  // try
  // {
  //   for (uint32_t mem_addr=MEM_STACK_START; mem_addr<MEM_STACK_START+128; mem_addr+=16)
  //   {
  //     lines++;
  //     out.str(std::string());
  //     uint32_t word = mem->mem_read_32(mem_addr);
  //     out << "[" << Utils::hex32(mem_addr) << "] " << Utils::hex32(word);
  //     word = mem->mem_read_32(mem_addr + 4);
  //     out << " " << Utils::hex32(word);
  //     word = mem->mem_read_32(mem_addr + 8);
  //     out << " " << Utils::hex32(word);
  //     word = mem->mem_read_32(mem_addr + 12);
  //     out << " " << Utils::hex32(word);
  //     ui->textSMem->append(out.str().c_str());
  //   }
  // }
  // catch (int e)
  // {
  //   cerr << "Error " << e << ": " << err_msg;
  //   if (err_v)
  //     cerr << " [0x" << Utils::hex32(err_v) << "]";
  //   cerr << endl;
  //   ui->textSMem->append(out.str().c_str());
  //   /* ignore */
  // }
}

void MipsSimGui::set_cpu_labels()
{
  float f;
  double d;
  uint32_t pc_value = cpu->read_special_register(SPECIAL_PC);

  ui->actionSettings->setEnabled(file_loaded);
  ui->btnReset->setEnabled(file_loaded);
  
  if (!file_loaded)
  {
    ui->btnPrev->setEnabled(false);
    ui->btnNext->setEnabled(false);
    ui->btnRun->setEnabled(false);
    return;
  }

  set_dmem_labels();
  set_diagram_labels();

  ui->lblPCValue->setText(Utils::hex32(pc_value).c_str());
  ui->lblHIValue->setText(Utils::hex32(cpu->read_special_register(SPECIAL_HI)).c_str());
  ui->lblLOValue->setText(Utils::hex32(cpu->read_special_register(SPECIAL_LO)).c_str());
  ui->lblCycles->setText(QString::number(cpu->get_cycle()));

  ui->btnPrev->setEnabled(cpu->get_cycle() > 0);
  ui->btnNext->setEnabled(cpu->is_ready());
  ui->btnRun->setEnabled(cpu->is_ready());

  /* set registers */
  for (int i=0; i<32; ++i)
  {
    iregtext[i]->setText(Utils::hex32(cpu->read_register(i)).c_str());
    f = cpu->read_register_f(i);

    fregtext[i]->setText(QString::number(f, 'f', 2));
    if (i%2 == 0)
    {
      d = cpu->read_register_d(i);
      dregtext[i]->setText(QString::number(d, 'f', 2));
    }
  }

  /* update instructions */
  uint32_t cpu_state[5];
  CpuPipelined & cpupipe = dynamic_cast<CpuPipelined &>(*cpu);

  cpupipe.get_current_state(cpu_state);

  for (instruction_info_t instruction : instructions)
  {
    if (instruction.pc_address == pc_value)
    {
      instruction.lblName->setFont(fontCurrent);
    }
    else
    {
      instruction.lblName->setFont(font);
    }
  }
}

void MipsSimGui::set_diagram_labels()
{
  QGridLayout * layout = static_cast<QGridLayout *>(ui->frameDiagram->layout());
  CpuPipelined & cpupipe = dynamic_cast<CpuPipelined &>(*cpu);
  const uint32_t * const * diagram = cpupipe.get_diagram();
  uint32_t current_cycle = cpu->get_cycle();
  size_t instr_count;// = instructions.size();

  /* clear diagram */
  for (diagram_info_t dline : diagram_lines)
  {
    layout->removeWidget(dline.lblName);
    delete(dline.lblName);
    for (QLabel * slabel : dline.lblStages)
    {
      layout->removeWidget(slabel);
      delete(slabel);
    }
  }
  diagram_lines.clear();

  /*  count width */
  uint32_t min_cycle = current_cycle;
  uint32_t instr_ids[20];
  instr_count = 0;

  if (current_cycle > 0)
  {
    const vector<uint32_t> & instruct_pcs = cpu->get_loaded_instructions();
    for (size_t i=1; i<=instruct_pcs.size(); i++)
    {
      if (diagram[i][current_cycle] > 0)
      {
        instr_ids[instr_count] = i;
        instr_count++;

        uint32_t lmin_cycle = current_cycle;
        /* check backwards */

        for (size_t j=current_cycle; diagram[i][j] > 0 && j > 0; j--)
        {
          lmin_cycle--;
        }
        if (lmin_cycle < min_cycle)
          min_cycle = lmin_cycle;
      }
    }

    for (int j=MAX_DIA_CYCLES-1; j>=0; j--)
    {
      if ((size_t)j > (MAX_DIA_CYCLES - (current_cycle - min_cycle) - 1))
        dcycle_labels[j]->setText(QString::number(j - MAX_DIA_CYCLES + current_cycle + 1));
      else
        dcycle_labels[j]->setText("");
    }

    for (instruction_info_t instruction : instructions)
    {
        instruction.lblName->setStyleSheet("");
    }

    for (size_t i=0; i<instr_count; i++)
    {
      diagram_info_t dinfo;
      uint32_t word;
      string color_style = "background-color: " + colors[instr_ids[i] % MAX_COLORS];

      dinfo.pc_address = instruct_pcs[instr_ids[i]];

      word = mem->mem_read_32(dinfo.pc_address);

      dinfo.lblName = new QLabel(ui->frameDiagram);
      dinfo.lblName->setMinimumSize(QSize(260, 15));
      dinfo.lblName->setMaximumSize(QSize(260, 15));
      dinfo.lblName->setFont(font);
      dinfo.lblName->setStyleSheet(QString::fromStdString(color_style));
      string iname = "[" + Utils::hex32(dinfo.pc_address) + "] " + Utils::decode_instruction(word);
      dinfo.lblName->setText(iname.c_str());
      layout->addWidget(dinfo.lblName, i+1, 0, 1, 1);

      /* update instruction */
      for (instruction_info_t instruction : instructions)
      {
        if (instruction.pc_address == instruct_pcs[instr_ids[i]])
        {
          instruction.lblName->setStyleSheet(QString::fromStdString(
            "background-color: " + colors[instr_ids[i] % MAX_COLORS]));
        }
      }

      for (size_t j=current_cycle; j>min_cycle; j--)
      {
        if (diagram[instr_ids[i]][j] > 0)
        {
          QLabel * instr_stage = new QLabel(ui->frameDiagram);
          instr_stage->setMinimumSize(QSize(30, 15));
          instr_stage->setMaximumSize(QSize(30, 15));
          instr_stage->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
          instr_stage->setFont(font);
          instr_stage->setFrameShape(QFrame::StyledPanel);

          if (diagram[instr_ids[i]][j] == diagram[instr_ids[i]][j-1])
          {
            instr_stage->setText("--"); // (if) (id) ... ?
            instr_stage->setStyleSheet("background-color: yellow");
          }
          else
          {
            instr_stage->setText(stage_names[diagram[instr_ids[i]][j] - 1].c_str());
            instr_stage->setStyleSheet(QString::fromStdString(color_style));
          }

          layout->addWidget(instr_stage,
                            i+1,
                            MAX_DIA_CYCLES-(current_cycle-min_cycle) + (j-min_cycle),
                            1,
                            1);

          dinfo.lblStages.push_back(instr_stage);
        }
      }

      diagram_lines.push_back(dinfo);
    }
  }
  else
  {
    /* clear instruction colors */
    for (instruction_info_t instruction : instructions)
      instruction.lblName->setStyleSheet("");
    for (int j=MAX_DIA_CYCLES-1; j>=0; j--)
        dcycle_labels[j]->setText("");
  }
}

MipsSimGui::MipsSimGui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MipsSimGui)
{
    mem = shared_ptr<Memory>(new Memory());
    file_loaded = false;

    cpu = unique_ptr<Cpu>(new CpuPipelined(mem));
    cpu->print_status(cout);

    settings_window = new MipsSimSettings( this );
    
    //TODO For Testing
    // CpuPipelined & cpupipe = dynamic_cast<CpuPipelined &>(*cpu);
    // cpupipe.enable_forwarding_unit(false);
    // cpupipe.set_branch_type(BRANCH_DELAYED);

    ui->setupUi(this);

    font.setFamily(QString::fromUtf8("Courier"));
    font.setPointSize(10);
    font.setBold(false);
    font.setItalic(false);
    font.setWeight(60);

    fontCurrent.setFamily(QString::fromUtf8("Courier"));
    fontCurrent.setPointSize(10);
    fontCurrent.setBold(true);
    fontCurrent.setItalic(true);
    fontCurrent.setWeight(60);

    QGridLayout * regslayout = static_cast<QGridLayout *>(ui->frameRegs->layout());
    QGridLayout * fregslayout = static_cast<QGridLayout *>(ui->frameFRegs->layout());
    QGridLayout * dregslayout = static_cast<QGridLayout *>(ui->frameDRegs->layout());

    /* set registers */
    for (int i=0; i<32; ++i)
    {
        int row = i%8;
        int col = (i/8) * 2;

        QLabel * ireglabel = new QLabel(ui->frameRegs);
        ireglabel->setMinimumSize(QSize(30, 15));
        ireglabel->setMaximumSize(QSize(16777215, 15));
        ireglabel->setFont(font);
        ireglabel->setText(Utils::get_register_name(i).c_str());
        ireglabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        regslayout->addWidget(ireglabel, row, col, 1, 1);

        iregtext[i] = new QLabel(ui->frameRegs);
        iregtext[i]->setMinimumSize(QSize(75, 15));
        iregtext[i]->setMaximumSize(QSize(16777215, 15));
        iregtext[i]->setFont(font);
        iregtext[i]->setFrameShape(QFrame::StyledPanel);
        regslayout->addWidget(iregtext[i], row, col+1, 1, 1);

        QLabel * freglabel = new QLabel(ui->frameFRegs);
        freglabel->setMinimumSize(QSize(30, 15));
        freglabel->setMaximumSize(QSize(16777215, 15));
        freglabel->setFont(font);
        freglabel->setText(Utils::get_fp_register_name(i).c_str());
        freglabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        fregslayout->addWidget(freglabel, row, col, 1, 1);

        fregtext[i] = new QLabel(ui->frameFRegs);
        fregtext[i]->setMinimumSize(QSize(80, 15));
        fregtext[i]->setMaximumSize(QSize(16777215, 15));
        fregtext[i]->setFont(font);
        fregtext[i]->setFrameShape(QFrame::StyledPanel);
        fregslayout->addWidget(fregtext[i], row, col+1, 1, 1);

        if (i%2 == 0)
        {
            QLabel * dreglabel = new QLabel(ui->frameDRegs);
            dreglabel->setMinimumSize(QSize(30, 15));
            dreglabel->setMaximumSize(QSize(16777215, 15));
            dreglabel->setFont(font);
            dreglabel->setText(freglabel->text());
            dreglabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            dregslayout->addWidget(dreglabel, row/2, col, 1, 1);

            dregtext[i] = new QLabel(ui->frameDRegs);
            dregtext[i]->setMinimumSize(QSize(80, 15));
            dregtext[i]->setMaximumSize(QSize(16777215, 15));
            dregtext[i]->setFont(font);
            dregtext[i]->setFrameShape(QFrame::StyledPanel);
            dregslayout->addWidget(dregtext[i], row/2, col+1, 1, 1);
        }
    }

    QGridLayout * layout = static_cast<QGridLayout *>(ui->frameDiagram->layout());

    for (size_t j=0; j<MAX_DIA_CYCLES; j++)
    {
        QLabel * dcycle_label = new QLabel(ui->frameDiagram);
        dcycle_label->setMinimumSize(QSize(30, 15));
        dcycle_label->setMaximumSize(QSize(30, 15));
        dcycle_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        dcycle_label->setFont(font);
        dcycle_label->setText(QString::number(j)); // (if) (id) ... ?
        layout->addWidget(dcycle_label, 0, j + 1, 1, 1);

        dcycle_labels.push_back(dcycle_label);
    }

    set_cpu_labels();
}

MipsSimGui::~MipsSimGui()
{
    delete settings_window;
    delete ui;
}

void MipsSimGui::handle_exception(int e)
{
  QMessageBox msgBox;
  msgBox.setWindowTitle("Exception caught!");
  if (err_v)
    msgBox.setText(QString::fromStdString("Exception at [0x" + Utils::hex32(err_v)) + "]");
  else
    msgBox.setText(QString::fromStdString("Exception " + std::to_string(e)));
  msgBox.setStyleSheet("QLabel{min-width: 400px;}");
  msgBox.setInformativeText(QString::fromStdString(err_msg));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.exec();
  
  /* move to previous cycle */
  cpu->run_to_cycle(cpu->get_cycle()-1, cout);
  set_cpu_labels();
  
  cerr << "EXCEPTION " << e << ": " << err_msg;
  if (err_v)
    cerr << " [0x" << Utils::hex32(err_v) << "]";
  cerr << endl;
}

void MipsSimGui::on_btnRun_clicked()
{
  try
  {
    while (cpu->is_ready())
    {
      cpu->next_cycle(cout);
    }
    set_cpu_labels();
  }
  catch(int e)
  {
    handle_exception(e);
  }
}

void MipsSimGui::on_btnNext_clicked()
{
  try
  {
    cpu->next_cycle(cout);
    set_cpu_labels();
  }
  catch(int e)
  {
    handle_exception(e);
  }
}

void MipsSimGui::on_btnPrev_clicked()
{
  try
  {
    cpu->run_to_cycle(cpu->get_cycle()-1, cout);
    set_cpu_labels();
  }
  catch(int e)
  {
    handle_exception(e);
  }
}

void MipsSimGui::on_btnReset_clicked()
{
  cpu->reset(true, true);
  set_cpu_labels();
}

void MipsSimGui::on_actionLoad_file_triggered()
{
  QString filters = tr("Assembly code(*.s *.asm);; All files(*)");
  QString file_name = QFileDialog::getOpenFileName(this,
                                                   tr("Open File"),
                                                   "",
                                                   filters);

  if(!file_name.isEmpty()&& !file_name.isNull())
  {
    const std::string filename = file_name.toStdString();
    file_loaded = load_file(filename, cout);
    if (file_loaded)
    {
      uint32_t start = MEM_TEXT_START;
      uint32_t length = MEM_TEXT_SIZE;

      asm_filename = filename;

      /* clear instructions */

      for (instruction_info_t instruction : instructions)
      {
        static_cast<QGridLayout *>(ui->frameTMem->layout())->removeWidget(instruction.lblName);
        delete(instruction.lblName);
      }

      /* clear data mem */

      for (QLabel * dmem_label : dmem_labels)
      {
        static_cast<QLayout *>(ui->scrollMemData->layout())->removeWidget(dmem_label);
        delete(dmem_label);
      }
      instructions.clear();
      dmem_labels.clear();

      for (uint32_t mem_addr=start, instr_index=1; mem_addr<start+length; mem_addr+=4, instr_index++)
      {
        instruction_info_t iinfo;
        iinfo.pc_address = mem_addr;

        try {
          uint32_t word = mem->mem_read_32(mem_addr);
          QLabel * instr_label;

          instr_label = new QLabel(ui->frameTMem);
          instr_label->setMinimumSize(QSize(400, 15));
          instr_label->setMaximumSize(QSize(400, 15));
          instr_label->setFont(font);
          string iname = "[" + Utils::hex32(mem_addr) + "] "
                             + Utils::hex32(word) + "  "
                             + Utils::decode_instruction(word);
          instr_label->setText(iname.c_str());
          static_cast<QGridLayout *>(ui->frameTMem->layout())->addWidget(instr_label, instr_index, 0, 1, 1);
          iinfo.lblName = instr_label;

          instructions.push_back(iinfo);
        }
        catch (int e)
        {
          /* ignore */
        }
        ui->frameTMem->setMinimumSize(QSize(350, instructions.size()*20));
      }

      /* set cpu */
      cpu->reset(true, true);
      set_cpu_labels();

      cout << "File loaded" << endl;
    }
    else
      cout << "Error loading file" << endl;
  }
}

void MipsSimGui::on_actionSettings_triggered()
{
  settings_window->exec();
}

void MipsSimGui::on_actionExit_triggered()
{
  QMessageBox msgBox;
  msgBox.setText("You will exit MIPS sim");
  msgBox.setInformativeText("Are you sure?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Yes)
  {
    close();
  }
}

bool MipsSimGui::load_file( const string & filename, ostream & out )
{
  if (!Utils::file_exists(filename))
  {
    QMessageBox msgBox;
    msgBox.setText("Error loading file");
    msgBox.setInformativeText(QString::fromStdString("File " + filename + " does not exist"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return false;
  }

  try {
    if (assemble_file(filename.c_str(), mem) != 0)
    {
      QMessageBox msgBox;
      msgBox.setText("Error loading file");
      msgBox.setInformativeText("Error parsing the input file");
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
      return false;
    }

    out << "File read OK" << endl;
    //show_memory( out );

    mem->snapshot(MEM_TEXT_REGION);
    mem->snapshot(MEM_DATA_REGION);
  }
  catch(int e)
  {
    QMessageBox msgBox;
    msgBox.setText("Error loading file");
    msgBox.setInformativeText(QString::fromStdString("Exception: " + err_msg));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    cerr << "EXCEPTION " << e << ": " << err_msg;
    if (err_v)
      cerr << " [0x" << Utils::hex32(err_v) << "]";
    cerr << endl;
    return false;
  }

  return true;
}

const map<string, int> MipsSimGui::get_cpu_status( void ) const
{
  assert(cpu);
  return cpu->get_status();
}

void MipsSimGui::set_cpu_status( std::map<std::string, int> new_status )
{
  assert(cpu);
  if (cpu->set_status(new_status, true))
  {
    cpu->reset(true, true);
    set_cpu_labels();
  }
}
