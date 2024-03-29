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
//#include <QLabel>
#include <QDir>
#include "centre.hpp"
#include "program_choice.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"
#include "message_box.hpp"

using namespace std;

program_choice::program_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"program_choice::program_choice\n";
//  batch_mode_driver_p->list_program();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_programs_p=new button("More programs");
  add_program_p=new button("Add a program");
  delete_program_p=new button("Delete a program");
  back_p=new button("Back");
//  message_p=new QLabel("Select program");
  message_p=new message_box;
  message_p->set_text("Select program");
  message_p->setMinimumSize(250, 70);

  top_box_p=new QGroupBox;
  middle_box_p=new QGroupBox;
  bottom_box_p=new QGroupBox;
  
  top_layout_p=new QGridLayout;
  middle_layout_p=new QGridLayout;
  bottom_layout_p=new QGridLayout;
  
  main_layout_p=new QVBoxLayout;
  
  top_layout_p->addWidget(message_p,0,0);
  top_layout_p->addWidget(back_p, 0, 1);
  top_box_p->setLayout(top_layout_p);
  
  middle_layout_p->addWidget(choice1_p,0,0);
  middle_layout_p->addWidget(choice2_p,0,1);
  middle_layout_p->addWidget(choice3_p,1,0);
  middle_layout_p->addWidget(choice4_p,1,1);
  middle_layout_p->addWidget(choice5_p,2,0);
  middle_layout_p->addWidget(choice6_p,2,1);
  middle_box_p->setLayout(middle_layout_p);
  
  bottom_layout_p->addWidget(more_programs_p,0,0);
  bottom_layout_p->addWidget(add_program_p,0,1);
  bottom_layout_p->addWidget(delete_program_p,0,2);
  bottom_box_p->setLayout(bottom_layout_p);
  
  main_layout_p->addWidget(top_box_p);
  main_layout_p->addWidget(middle_box_p);
  main_layout_p->addWidget(bottom_box_p);
  
  setLayout(main_layout_p);
  
  connect(choice1_p, SIGNAL(clicked()), this, SLOT(choice1_clicked()));
  connect(choice2_p, SIGNAL(clicked()), this, SLOT(choice2_clicked()));
  connect(choice3_p, SIGNAL(clicked()), this, SLOT(choice3_clicked()));
  connect(choice4_p, SIGNAL(clicked()), this, SLOT(choice4_clicked()));
  connect(choice5_p, SIGNAL(clicked()), this, SLOT(choice5_clicked()));
  connect(choice6_p, SIGNAL(clicked()), this, SLOT(choice6_clicked()));
  connect(more_programs_p, SIGNAL(clicked()), this, SLOT(more_programs_clicked()));
  connect(add_program_p, SIGNAL(clicked()), this, SLOT(add_program_clicked()));
  connect(delete_program_p, SIGNAL(clicked()), this, SLOT(delete_program_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
  QString crop_name = centre_p->crops[0].name;
  program_path = "programs/";
  program_path.append(crop_name);
  program_path.append("/");
  dir_p = new QDir(program_path);
  program_list_p = new QStringList;
  QStringList filter;
  filter<<"*.ESC3";
  *program_list_p = dir_p->entryList(filter);
  for(int i=0; i<program_list_p->size(); ++i)
  {
    QString temp = program_list_p->at(i);
    temp.remove(".ESC3");
    program_list_p->replace(i, temp);
  }
  first_program_displayed = 0;
  display_programs();
  
  if(centre_p->crops[0].calibrated == false)
  {
    QString msg = "Crop ";
    msg.append(centre_p->crops[0].name);
    msg.append(" has not been calibrated.\nYou must calibrate it before using.");
    message_p->set_text(msg);
    message_p->set_background(255, 0, 0);
    message_p->set_text_size(20);
  }
}

program_choice::~program_choice()
{
  delete dir_p;
  dir_p = 0;
  delete program_list_p;
  program_list_p = 0;
}

void program_choice::choice1_clicked()
{
  if(first_program_displayed<program_list_p->size())
  {
    program_path.append(program_list_p->at(first_program_displayed));
    program_path.append(".ESC3");
    batch_mode_driver_p->program_path = program_path;
    batch_mode_driver_p->program_name = program_list_p->at(first_program_displayed);
    batch_mode_driver_p->load_program();
    centre_p->add_waiting_screen(15);
    centre_p->screen_done=true;
  }
}

void program_choice::choice2_clicked()
{
  if(first_program_displayed+1<program_list_p->size())
  {
    program_path.append(program_list_p->at(first_program_displayed+1));
    program_path.append(".ESC3");
    batch_mode_driver_p->program_path = program_path;
    batch_mode_driver_p->program_name = program_list_p->at(first_program_displayed+1);
    batch_mode_driver_p->load_program();
    centre_p->add_waiting_screen(15);
    centre_p->screen_done=true;
  }
}

void program_choice::choice3_clicked()
{
  if(first_program_displayed+2<program_list_p->size())
  {
    program_path.append(program_list_p->at(first_program_displayed+2));
    program_path.append(".ESC3");
    batch_mode_driver_p->program_path = program_path;
    batch_mode_driver_p->program_name = program_list_p->at(first_program_displayed+2);
    batch_mode_driver_p->load_program();
    centre_p->add_waiting_screen(15);
    centre_p->screen_done=true;
  }
}

void program_choice::choice4_clicked()
{
  if(first_program_displayed+3<program_list_p->size())
  {
    program_path.append(program_list_p->at(first_program_displayed+3));
    program_path.append(".ESC3");
    batch_mode_driver_p->program_path = program_path;
    batch_mode_driver_p->program_name = program_list_p->at(first_program_displayed+3);
    batch_mode_driver_p->load_program();
    centre_p->add_waiting_screen(15);
    centre_p->screen_done=true;
  }
}

void program_choice::choice5_clicked()
{
  if(first_program_displayed+4<program_list_p->size())
  {
    program_path.append(program_list_p->at(first_program_displayed+4));
    program_path.append(".ESC3");
    batch_mode_driver_p->program_path = program_path;
    batch_mode_driver_p->program_name = program_list_p->at(first_program_displayed+4);
    batch_mode_driver_p->load_program();
    centre_p->add_waiting_screen(15);
    centre_p->screen_done=true;
  }
}

void program_choice::choice6_clicked()
{
  if(first_program_displayed+5<program_list_p->size())
  {
    program_path.append(program_list_p->at(first_program_displayed+5));
    program_path.append(".ESC3");
    batch_mode_driver_p->program_path = program_path;
    batch_mode_driver_p->program_name = program_list_p->at(first_program_displayed+5);
    batch_mode_driver_p->load_program();
    centre_p->add_waiting_screen(15);
    centre_p->screen_done=true;
  }
}

void program_choice::more_programs_clicked()
{
  if(first_program_displayed<(program_list_p->size()-6)) first_program_displayed += 6;
  display_programs();
}

void program_choice::add_program_clicked()
{
  if(centre_p->crops[0].calibrated == false) return;
  centre_p->add_waiting_screen(14);//enter_program
  batch_mode_driver_p->clear_program();
  batch_mode_driver_p->program_path = "";
  centre_p->screen_done=true;
}

void program_choice::delete_program_clicked()
{
  centre_p->add_waiting_screen(18);//delete_program
  centre_p->screen_done=true;
}

void program_choice::back_clicked()
{
  if(first_program_displayed>5) 
  {
    first_program_displayed -= 6;
    display_programs();
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void program_choice::display_programs()
{
  if(first_program_displayed<program_list_p->size())    choice1_p->setText(program_list_p->at(first_program_displayed));
  else choice1_p->setText("");
  if(first_program_displayed+1<program_list_p->size())  choice2_p->setText(program_list_p->at(first_program_displayed+1));
  else choice2_p->setText("");
  if(first_program_displayed+2<program_list_p->size())  choice3_p->setText(program_list_p->at(first_program_displayed+2));
  else choice3_p->setText("");
  if(first_program_displayed+3<program_list_p->size())  choice4_p->setText(program_list_p->at(first_program_displayed+3));
  else choice4_p->setText("");
  if(first_program_displayed+4<program_list_p->size())  choice5_p->setText(program_list_p->at(first_program_displayed+4));
  else choice5_p->setText("");
  if(first_program_displayed+5<program_list_p->size())  choice6_p->setText(program_list_p->at(first_program_displayed+5));
  else choice6_p->setText("");
}
