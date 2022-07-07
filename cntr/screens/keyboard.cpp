/***********

Copyright 2021 Agriculex Inc.

This file is part of the Agriculex ESC-3 program.

The Agriculex ESC-3 program is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, GPL-3.0-or-later.

The Agriculex ESC-3 program is distributed in the hope that it will be 
useful, but, except as noted below, WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Agriculex ESC-3 program.  If not, see 
<https://www.gnu.org/licenses/>.

Agriculex Inc. warrants that executable files of the ESC-3 program 
compiled and installed by Agriculex Inc. will function as advertized in 
ESC-3 machines produced by Agriculex Inc.

**********/

#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QStringList>
#include "centre.hpp"
#include "crop_choice.hpp"
#include "button.hpp"
#include "keyboard.hpp"

using namespace std;

kb_key::kb_key(const QString & text, QWidget* parent)
:QPushButton(text, parent)
{
  setMinimumSize(65,50);  //TEST~~~
//  setMaximumSize(65,50);  //
}
  
keyboard::keyboard(centre* set_centre_p)
:screen(set_centre_p)
{
  message_p=new QTextEdit;
  message_p->setMaximumSize(500,40);
  message_p->setFocusPolicy(Qt::NoFocus);
  entry_line_display_p=new QLineEdit;
  entry_line_display_p->setFocusPolicy(Qt::NoFocus);
  back_button_p=new button("BACK");
  entry_line_p=new QString;
  top_box_p=new QGroupBox;
  top_box_layout_p= new QGridLayout;
  top_box_layout_p->addWidget(message_p,0,0);
  top_box_layout_p->addWidget(entry_line_display_p,1,0);
  top_box_layout_p->addWidget(back_button_p,0,1);
  top_box_p->setLayout(top_box_layout_p);
  
  n1_key_p=new kb_key(",\n1");
  n2_key_p=new kb_key(".\n2");
  n3_key_p=new kb_key("?\n3");
  n4_key_p=new kb_key("!\n4");
  n5_key_p=new kb_key(":\n5");
  n6_key_p=new kb_key(";\n6");
  n7_key_p=new kb_key("@\n7");
  n8_key_p=new kb_key("=\n8");
  n9_key_p=new kb_key("(\n9");
  n0_key_p=new kb_key(")\n0");
  Q_key_p=new kb_key("Q");
  W_key_p=new kb_key("W");
  E_key_p=new kb_key("E");
  R_key_p=new kb_key("R");
  T_key_p=new kb_key("T");
  Y_key_p=new kb_key("Y");
  U_key_p=new kb_key("U");
  I_key_p=new kb_key("I");
  O_key_p=new kb_key("O");
  P_key_p=new kb_key("P");
  A_key_p=new kb_key("A");
  S_key_p=new kb_key("S");
  D_key_p=new kb_key("D");
  F_key_p=new kb_key("F");
  G_key_p=new kb_key("G");
  H_key_p=new kb_key("H");
  J_key_p=new kb_key("J");
  K_key_p=new kb_key("K");
  L_key_p=new kb_key("L");
  Z_key_p=new kb_key("Z");
  X_key_p=new kb_key("X");
  C_key_p=new kb_key("C");
  V_key_p=new kb_key("V");
  B_key_p=new kb_key("B");
  N_key_p=new kb_key("N");
  M_key_p=new kb_key("M");
  space_key_p=new QPushButton("Space");
  space_key_p-> setMinimumSize(130,65);
  enter_key_p=new kb_key("OK");
  backspace_key_p=new kb_key("<-X");
  shift_key_p=new QPushButton("S\nH\nI\nF\nT");
  shift_key_p-> setMinimumSize(40,200);
  
  key_box_p=new QGroupBox;
  key_box_layout_p=new QGridLayout;
  key_box_layout_p->addWidget(n1_key_p, 0, 1);
  key_box_layout_p->addWidget(n2_key_p, 0, 2);
  key_box_layout_p->addWidget(n3_key_p, 0, 3);
  key_box_layout_p->addWidget(n4_key_p, 0, 4);
  key_box_layout_p->addWidget(n5_key_p, 0, 5);
  key_box_layout_p->addWidget(n6_key_p, 0, 6);
  key_box_layout_p->addWidget(n7_key_p, 0, 7);
  key_box_layout_p->addWidget(n8_key_p, 0, 8);
  key_box_layout_p->addWidget(n9_key_p, 0, 9);
  key_box_layout_p->addWidget(n0_key_p, 0, 10);
  key_box_layout_p->addWidget(Q_key_p, 1, 1);
  key_box_layout_p->addWidget(W_key_p, 1, 2);
  key_box_layout_p->addWidget(E_key_p, 1, 3);
  key_box_layout_p->addWidget(R_key_p, 1, 4);
  key_box_layout_p->addWidget(T_key_p, 1, 5);
  key_box_layout_p->addWidget(Y_key_p, 1, 6);
  key_box_layout_p->addWidget(U_key_p, 1, 7);
  key_box_layout_p->addWidget(I_key_p, 1, 8);
  key_box_layout_p->addWidget(O_key_p, 1, 9);
  key_box_layout_p->addWidget(P_key_p, 1, 10);
  key_box_layout_p->addWidget(A_key_p, 2, 1);
  key_box_layout_p->addWidget(S_key_p, 2, 2);
  key_box_layout_p->addWidget(D_key_p, 2, 3);
  key_box_layout_p->addWidget(F_key_p, 2, 4);
  key_box_layout_p->addWidget(G_key_p, 2, 5);
  key_box_layout_p->addWidget(H_key_p, 2, 6);
  key_box_layout_p->addWidget(J_key_p, 2, 7);
  key_box_layout_p->addWidget(K_key_p, 2, 8);
  key_box_layout_p->addWidget(L_key_p, 2, 9);
  key_box_layout_p->addWidget(backspace_key_p, 2, 10);
  key_box_layout_p->addWidget(Z_key_p, 3, 1);
  key_box_layout_p->addWidget(X_key_p, 3, 2);
  key_box_layout_p->addWidget(C_key_p, 3, 3);
  key_box_layout_p->addWidget(V_key_p, 3, 4);
  key_box_layout_p->addWidget(space_key_p, 3, 5, 1, 2);
  key_box_layout_p->addWidget(B_key_p, 3, 7);
  key_box_layout_p->addWidget(N_key_p, 3, 8);
  key_box_layout_p->addWidget(M_key_p, 3, 9);
  key_box_layout_p->addWidget(enter_key_p, 3, 10);
  key_box_layout_p->addWidget(shift_key_p, 0, 0, 4, 1);

  key_box_p->setLayout(key_box_layout_p);
  
  main_layout_p=new QVBoxLayout;
  main_layout_p->addWidget(top_box_p);
  main_layout_p->addWidget(key_box_p);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
  connect(n1_key_p, SIGNAL(clicked()), this, SLOT(n1_clicked()));
  connect(n2_key_p, SIGNAL(clicked()), this, SLOT(n2_clicked()));
  connect(n3_key_p, SIGNAL(clicked()), this, SLOT(n3_clicked()));
  connect(n4_key_p, SIGNAL(clicked()), this, SLOT(n4_clicked()));
  connect(n5_key_p, SIGNAL(clicked()), this, SLOT(n5_clicked()));
  connect(n6_key_p, SIGNAL(clicked()), this, SLOT(n6_clicked()));
  connect(n7_key_p, SIGNAL(clicked()), this, SLOT(n7_clicked()));
  connect(n8_key_p, SIGNAL(clicked()), this, SLOT(n8_clicked()));
  connect(n9_key_p, SIGNAL(clicked()), this, SLOT(n9_clicked()));
  connect(n0_key_p, SIGNAL(clicked()), this, SLOT(n0_clicked()));
  connect(Q_key_p, SIGNAL(clicked()), this, SLOT(Q_clicked()));
  connect(W_key_p, SIGNAL(clicked()), this, SLOT(W_clicked()));
  connect(E_key_p, SIGNAL(clicked()), this, SLOT(E_clicked()));
  connect(R_key_p, SIGNAL(clicked()), this, SLOT(R_clicked()));
  connect(T_key_p, SIGNAL(clicked()), this, SLOT(T_clicked()));
  connect(Y_key_p, SIGNAL(clicked()), this, SLOT(Y_clicked()));
  connect(U_key_p, SIGNAL(clicked()), this, SLOT(U_clicked()));
  connect(I_key_p, SIGNAL(clicked()), this, SLOT(I_clicked()));
  connect(O_key_p, SIGNAL(clicked()), this, SLOT(O_clicked()));
  connect(P_key_p, SIGNAL(clicked()), this, SLOT(P_clicked()));
  connect(A_key_p, SIGNAL(clicked()), this, SLOT(A_clicked()));
  connect(S_key_p, SIGNAL(clicked()), this, SLOT(S_clicked()));
  connect(D_key_p, SIGNAL(clicked()), this, SLOT(D_clicked()));
  connect(F_key_p, SIGNAL(clicked()), this, SLOT(F_clicked()));
  connect(G_key_p, SIGNAL(clicked()), this, SLOT(G_clicked()));
  connect(H_key_p, SIGNAL(clicked()), this, SLOT(H_clicked()));
  connect(J_key_p, SIGNAL(clicked()), this, SLOT(J_clicked()));
  connect(K_key_p, SIGNAL(clicked()), this, SLOT(K_clicked()));
  connect(L_key_p, SIGNAL(clicked()), this, SLOT(L_clicked()));
  connect(backspace_key_p, SIGNAL(clicked()), this, SLOT(backspace_clicked()));
  connect(Z_key_p, SIGNAL(clicked()), this, SLOT(Z_clicked()));
  connect(X_key_p, SIGNAL(clicked()), this, SLOT(X_clicked()));
  connect(C_key_p, SIGNAL(clicked()), this, SLOT(C_clicked()));
  connect(V_key_p, SIGNAL(clicked()), this, SLOT(V_clicked()));
  connect(space_key_p, SIGNAL(clicked()), this, SLOT(space_clicked()));
  connect(B_key_p, SIGNAL(clicked()), this, SLOT(B_clicked()));
  connect(N_key_p, SIGNAL(clicked()), this, SLOT(N_clicked()));
  connect(M_key_p, SIGNAL(clicked()), this, SLOT(M_clicked()));
  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
  connect(shift_key_p, SIGNAL(clicked()), this, SLOT(shift_clicked()));
  
  connect(this, SIGNAL(external_keyboard_enter()), enter_key_p, SIGNAL(clicked()));

  if(centre_p->new_keyboard_entry)//this is a general keyboard entry screen
  {
    message_p->setText(centre_p->keyboard_message_string);
  }
  entry_line_display_p -> setStyleSheet( "font-size: 15pt;");
  
  shift_active = false;
  setFocus();
}

void keyboard::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void keyboard::n1_clicked()
{
  if(shift_active) entry_line_p->append(",");
  else entry_line_p->append("1");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n2_clicked()
{
  if(shift_active) entry_line_p->append(".");
  else entry_line_p->append("2");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n3_clicked()
{
  if(shift_active) entry_line_p->append("?");
  else entry_line_p->append("3");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n4_clicked()
{
  if(shift_active) entry_line_p->append("!");
  else entry_line_p->append("4");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n5_clicked()
{
  if(shift_active) entry_line_p->append(":");
  else entry_line_p->append("5");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n6_clicked()
{
  if(shift_active) entry_line_p->append(";");
  else entry_line_p->append("6");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n7_clicked()
{
  if(shift_active) entry_line_p->append("@");
  else entry_line_p->append("7");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n8_clicked()
{
  if(shift_active) entry_line_p->append("=");
  else entry_line_p->append("8");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n9_clicked()
{
  if(shift_active) entry_line_p->append("(");
  else entry_line_p->append("9");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::n0_clicked()
{
  if(shift_active) entry_line_p->append(")");
  else entry_line_p->append("0");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::Q_clicked()
{
  if(shift_active) entry_line_p->append("Q");
  else entry_line_p->append("q");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::W_clicked()
{
  if(shift_active) entry_line_p->append("W");
  else entry_line_p->append("w");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::E_clicked()
{
  if(shift_active) entry_line_p->append("E");
  else entry_line_p->append("e");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::R_clicked()
{
  if(shift_active) entry_line_p->append("R");
  else entry_line_p->append("r");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::T_clicked()
{
  if(shift_active) entry_line_p->append("T");
  else entry_line_p->append("t");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::Y_clicked()
{
  if(shift_active) entry_line_p->append("Y");
  else entry_line_p->append("y");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::U_clicked()
{
  if(shift_active) entry_line_p->append("U");
  else entry_line_p->append("u");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::I_clicked()
{
  if(shift_active) entry_line_p->append("I");
  else entry_line_p->append("i");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::O_clicked()
{
  if(shift_active) entry_line_p->append("O");
  else entry_line_p->append("o");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::P_clicked()
{
  if(shift_active) entry_line_p->append("P");
  else entry_line_p->append("p");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::A_clicked()
{
  if(shift_active) entry_line_p->append("A");
  else entry_line_p->append("a");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::S_clicked()
{
  if(shift_active) entry_line_p->append("S");
  else entry_line_p->append("s");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::D_clicked()
{
  if(shift_active) entry_line_p->append("D");
  else entry_line_p->append("d");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::F_clicked()
{
  if(shift_active) entry_line_p->append("F");
  else entry_line_p->append("f");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::G_clicked()
{
  if(shift_active) entry_line_p->append("G");
  else entry_line_p->append("g");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::H_clicked()
{
  if(shift_active) entry_line_p->append("H");
  else entry_line_p->append("h");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::J_clicked()
{
  if(shift_active) entry_line_p->append("J");
  else entry_line_p->append("j");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::K_clicked()
{
  if(shift_active) entry_line_p->append("K");
  else entry_line_p->append("k");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::L_clicked()
{
  if(shift_active) entry_line_p->append("L");
  else entry_line_p->append("l");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::backspace_clicked()
{
  entry_line_p->chop(1);
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::Z_clicked()
{
  if(shift_active) entry_line_p->append("Z");
  else entry_line_p->append("z");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::X_clicked()
{
  if(shift_active) entry_line_p->append("X");
  else entry_line_p->append("x");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::C_clicked()
{
  if(shift_active) entry_line_p->append("C");
  else entry_line_p->append("c");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::V_clicked()
{
  if(shift_active) entry_line_p->append("V");
  else entry_line_p->append("v");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::space_clicked()
{
  entry_line_p->append(" ");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::B_clicked()
{
  if(shift_active) entry_line_p->append("B");
  else entry_line_p->append("b");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::N_clicked()
{
  if(shift_active) entry_line_p->append("N");
  else entry_line_p->append("n");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}
void keyboard::M_clicked()
{
  if(shift_active) entry_line_p->append("M");
  else entry_line_p->append("m");
  entry_line_display_p->setText(*entry_line_p);
  shift_active = false;
}

void keyboard::enter_clicked()
{
      
//  cout<<"start keyboard::enter_clicked.  new_keyboard_entry = "<<centre_p->new_keyboard_entry<<endl;
      
//  centre_p->new_keyboard_entry = true;
  centre_p->keyboard_return_string.clear();
  centre_p->keyboard_return_string.append(*entry_line_p);
  centre_p->screen_done = true;//go to waiting screen
  shift_active = false;
  
  cout<<"end keyboard::enter_clicked.  keyboard_return_string = "<<centre_p->keyboard_return_string.toStdString()<<endl;
  
}

void keyboard::shift_clicked()
{
  shift_active = true;
}

void keyboard::keyPressEvent(QKeyEvent* event)
{
  QWidget::keyPressEvent(event);
  int key = event->key();
  cout<<"key "<<key<<endl;
  if((key >= 0x30 && key <= 0x39) || (key >= 0x41 && key <= 0x5a)) //alphanumeric
  {
    entry_line_p->append(event->text());
    entry_line_display_p->setText(*entry_line_p);
    cout<<"entry line "<<entry_line_p->toLatin1().data()<<endl;
  }
  if(key == Qt::Key_Return)
  {
    emit external_keyboard_enter();
  }
}

crop_name_entry::crop_name_entry(centre* centre_p)
:keyboard(centre_p)
{
  QString message("Enter crop name.");
  message_p->setText(message);
  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
}

void crop_name_entry::enter_clicked()
{
  if((*entry_line_p).trimmed() == "")
  {
    centre_p->add_waiting_screen(0);//start
    centre_p->screen_done = true;
    return;
  }
  if(centre_p->changing_existing_crop == true)//overwrite name.  Do not create new crop.
  {
    centre_p->crops[0].name=*entry_line_p;
    centre_p->screen_done = true;
    centre_p->changing_existing_crop = false;
    return;
  }
  int i;
  
  //only executes if not changing existing crop
  //check if crop name exists
  for(i=0; i<99; ++i)
  {
    if (centre_p->crops[i].name == *entry_line_p)
    {
      centre_p->add_waiting_screen(19);//crop_name_exists
      centre_p->screen_done = true;
      return;
    }
  }
  
  //only executes for new unique name
  for(i=99;i>0;--i)
  {
    centre_p->crops[i]=centre_p->crops[i-1];
  }
  centre_p->crops[0].name=*entry_line_p;
  centre_p->crops[0].sensitivity=0;
  centre_p->crops[0].gate_setting=0;
  centre_p->crops[0].area_mean=0;
  centre_p->crops[0].area_stdev=0;
  centre_p->crops[0].max_inflection_1=0;
  centre_p->crops[0].max_inflection_3=0;
  centre_p->crops[0].max_inflection_9=0;
  centre_p->crops[0].calibrated=0;
  centre_p->add_waiting_screen(10);
  centre_p->screen_done=true;
}

totalize_save_file::totalize_save_file(centre* centre_p)
:keyboard(centre_p)
{
  QString message("Enter file name.");
  message_p->setText(message);
  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
  QString default_file_name = centre_p->tm_last_filename;
  *entry_line_p = default_file_name;
  entry_line_display_p -> setText(*entry_line_p);
}

void totalize_save_file::enter_clicked()
{
  QDir qdir("user");
  if(!qdir.exists())
  {
    
    cout<<"making directory user\n";
    
    qdir.cd(qdir.currentPath());
    qdir.mkdir("user");
  }
  qdir.cd("user");
  QString crop_name = centre_p->crops[0].name;
  if(!qdir.exists(crop_name))
  {
    qdir.mkdir(crop_name);
  }
  
  QString outfile_name("user/");
  outfile_name.append(crop_name);
  outfile_name.append("/");
  outfile_name.append(*entry_line_p);
  outfile_name.append(".csv");
  
  cout<<"outfile_name "<<outfile_name.toLatin1().data()<<endl;
  
  centre_p->tm_save_filename = outfile_name;
  centre_p->tm_last_filename = *entry_line_p;
  centre_p->add_waiting_screen(5);//totalize
  centre_p->screen_done=true;
  //returning to totalize with tm_save_filename not blank will trigger save.  see totalize constructor.
}

batch_save_program::batch_save_program(centre* centre_p, batch_mode_driver* batch_mode_driver_p_s)
:keyboard(centre_p)
{
  message_p->setMaximumSize(500,100);  
  
  batch_mode_driver_p = batch_mode_driver_p_s;
  QString message("Enter file name. Programs already saved for ");
  message.append(centre_p->crops[0].name);
  message.append(":\n");
  QDir program_dir("./programs");
  program_dir.cd(centre_p->crops[0].name);
  program_dir.setFilter(QDir::Files);
  QStringList program_list = program_dir.entryList();
  for(int i=0; i<program_list.size(); ++i)
  {
    if(i>0) message.append(", ");
    QString name = program_list[i];
    name.remove(".ESC3");
    message.append(name);
  }
  message_p->setText(message);
  
  QString path(batch_mode_driver_p->program_path);
  int slash_index = path.lastIndexOf("/");
  int chars_to_take = path.length() - slash_index - 1;
  QString file_name = path.right(chars_to_take);
  file_name.remove(".ESC3");
  *entry_line_p = file_name;
  entry_line_display_p->setText(*entry_line_p);

  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
}

void batch_save_program::enter_clicked()
{
  QDir qdir("programs");
  if(!qdir.exists())
  {
    
    cout<<"making directory programs\n";
    
    qdir.cd(qdir.currentPath());
    qdir.mkdir("programs");
  }
  qdir.cd("programs");
  QString crop_name = centre_p->crops[0].name;
  if(!qdir.exists(crop_name))
  {
    qdir.mkdir(crop_name);
  }
  
  QString outfile_name("programs/");
  outfile_name.append(crop_name);
  outfile_name.append("/");
  outfile_name.append(*entry_line_p);
  outfile_name.append(".ESC3");
  
  cout<<"outfile_name "<<outfile_name.toLatin1().data()<<endl;
  
  batch_mode_driver_p->bm_save_program_filename = outfile_name;
  batch_mode_driver_p->program_name = *entry_line_p;
  
  batch_mode_driver_p->program_path = outfile_name;//added 2021-03-14 to cause default name to appear if editing a newly entered program
  
  centre_p->add_waiting_screen(15);//batch
  centre_p->screen_done=true;
  //returning to batch with bm_save_program filename not blank will trigger save.  see batch constructor.
}
  







batch_save_table::batch_save_table(centre* centre_p, batch_mode_driver* batch_mode_driver_p_s)
:keyboard(centre_p)
{
  batch_mode_driver_p = batch_mode_driver_p_s;
  QString message("Enter file name.");
  message_p->setText(message);
  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));

  *entry_line_p = batch_mode_driver_p->bm_last_table_filename;
  entry_line_display_p -> setText(*entry_line_p);
}

void batch_save_table::enter_clicked()
{
  QDir qdir("user");
  if(!qdir.exists())
  {
    
    cout<<"making directory user\n";
    
    qdir.cd(qdir.currentPath());
    qdir.mkdir("user");
  }
  qdir.cd("user");
  QString crop_name = centre_p->crops[0].name;
  if(!qdir.exists(crop_name))
  {
    qdir.mkdir(crop_name);
  }
  
  QString outfile_name("user/");
  outfile_name.append(crop_name);
  outfile_name.append("/");
  outfile_name.append(*entry_line_p);
  outfile_name.append(".csv");
  
  cout<<"outfile_name "<<outfile_name.toLatin1().data()<<endl;
  
  batch_mode_driver_p->bm_save_table_filename = outfile_name;
  batch_mode_driver_p->bm_last_table_filename = *entry_line_p;
  centre_p->add_waiting_screen(15);//batch
  centre_p->screen_done=true;
  //returning to batch with bm_save_table_filename not blank will trigger save.  see batch constructor.
}
  
batch_save_ss_setup::batch_save_ss_setup(centre* centre_p, batch_mode_driver* batch_mode_driver_p_s)
:keyboard(centre_p)
{
  message_p->setMaximumSize(500,100);  
  
  batch_mode_driver_p = batch_mode_driver_p_s;
  QString message("Enter file name. Spreadsheet setups already saved for ");
  message.append(centre_p->crops[0].name);
  message.append(":\n");
  QDir program_dir("./ss_setups");
  program_dir.cd(centre_p->crops[0].name);
  program_dir.setFilter(QDir::Files);
  QStringList program_list = program_dir.entryList();
  for(int i=0; i<program_list.size(); ++i)
  {
    if(i>0) message.append(", ");

    QString name = program_list[i];
    name.remove(".ESC3sss");
    message.append(name);
  }
  message_p->setText(message);
  
  QString path(batch_mode_driver_p->ss_setup_path);
  int slash_index = path.lastIndexOf("/");
  int chars_to_take = path.length() - slash_index - 1;
  QString file_name = path.right(chars_to_take);
  file_name.remove(".ESC3sss");
  *entry_line_p = file_name;
  entry_line_display_p->setText(*entry_line_p);

  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
}


void batch_save_ss_setup::enter_clicked()
{
  QDir qdir("ss_setups");
  if(!qdir.exists())
  {
    
    cout<<"making directory ss_setups\n";
    
    qdir.cd(qdir.currentPath());
    qdir.mkdir("ss_setups");
  }
  qdir.cd("ss_setups");
  QString crop_name = centre_p->crops[0].name;
  if(!qdir.exists(crop_name))
  {
    qdir.mkdir(crop_name);
  }
  
  QString outfile_name("ss_setups/");
  outfile_name.append(crop_name);
  outfile_name.append("/");
  outfile_name.append(*entry_line_p);
  outfile_name.append(".ESC3sss");
  
  cout<<"outfile_name "<<outfile_name.toLatin1().data()<<endl;
  
  batch_mode_driver_p->bm_save_ss_setup_filename = outfile_name;
  centre_p->add_waiting_screen(33);//ss_batch
  centre_p->screen_done=true;
  //returning to ss_batch with bm_save_ss_setup filename not blank will trigger save.  see ss_batch constructor.
}

enter_field_text::enter_field_text(centre* centre_p, batch_mode_driver* batch_mode_driver_p_s)
:keyboard(centre_p)
{
//  message_p->setMaximumSize(500,100);  
  
  batch_mode_driver_p = batch_mode_driver_p_s;
  QString message("Enter text.");
  message_p->setText(message);

  connect(enter_key_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
}

void enter_field_text::enter_clicked()
{
//  batch_mode_driver_p->envelope_p->enter_text_field(*entry_line_p);
  centre_p->add_waiting_screen(35);//position_envelope_field
  centre_p->screen_done=true;
}
