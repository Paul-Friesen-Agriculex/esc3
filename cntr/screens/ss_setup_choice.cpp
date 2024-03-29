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
#include <QLabel>
#include <QDir>
#include "centre.hpp"
#include "ss_setup_choice.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

ss_setup_choice::ss_setup_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"ss_setup_choice::ss_setup_choice\n";
//  batch_mode_driver_p->list_ss_setup();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_ss_setups_p=new button("More spreadsheet setups");
  add_ss_setup_p=new button("Add a spreadsheet setup");
  delete_ss_setup_p=new button("Delete a spreadsheet setup");
  back_p=new button("Back");
  message_p=new QLabel("Select spreadsheet setup");

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
  
  bottom_layout_p->addWidget(more_ss_setups_p,0,0);
  bottom_layout_p->addWidget(add_ss_setup_p,0,1);
  bottom_layout_p->addWidget(delete_ss_setup_p,0,2);
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
  connect(more_ss_setups_p, SIGNAL(clicked()), this, SLOT(more_ss_setups_clicked()));
  connect(add_ss_setup_p, SIGNAL(clicked()), this, SLOT(add_ss_setup_clicked()));
  connect(delete_ss_setup_p, SIGNAL(clicked()), this, SLOT(delete_ss_setup_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
  QString crop_name = centre_p->crops[0].name;
  ss_setup_path = "ss_setups/";
  ss_setup_path.append(crop_name);
  ss_setup_path.append("/");
  dir_p = new QDir(ss_setup_path);
  ss_setup_list_p = new QStringList;
  QStringList filter;
  filter<<"*.ESC3sss";
  *ss_setup_list_p = dir_p->entryList(filter);
  for(int i=0; i<ss_setup_list_p->size(); ++i)
  {
    QString temp = ss_setup_list_p->at(i);
    temp.remove(".ESC3sss");
    ss_setup_list_p->replace(i, temp);
  }
  first_ss_setup_displayed = 0;
  display_ss_setups();
  
  (batch_mode_driver_p->display_column_numbers).clear();
}

ss_setup_choice::~ss_setup_choice()
{
  delete dir_p;
  dir_p = 0;
  delete ss_setup_list_p;
  ss_setup_list_p = 0;
}

void ss_setup_choice::choice1_clicked()
{
  if(first_ss_setup_displayed<ss_setup_list_p->size())
  {
    ss_setup_path.append(ss_setup_list_p->at(first_ss_setup_displayed));
    ss_setup_path.append(".ESC3sss");
    
//    cout<<"ss_setup_path "<<ss_setup_path.toStdString()<<endl;
    
    batch_mode_driver_p->ss_setup_path = ss_setup_path;
    batch_mode_driver_p->load_ss_setup();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::choice2_clicked()
{
  if(first_ss_setup_displayed+1<ss_setup_list_p->size())
  {
    ss_setup_path.append(ss_setup_list_p->at(first_ss_setup_displayed+1));
    ss_setup_path.append(".ESC3sss");
    batch_mode_driver_p->ss_setup_path = ss_setup_path;
    batch_mode_driver_p->load_ss_setup();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::choice3_clicked()
{
  if(first_ss_setup_displayed+2<ss_setup_list_p->size())
  {
    ss_setup_path.append(ss_setup_list_p->at(first_ss_setup_displayed+2));
    ss_setup_path.append(".ESC3sss");
    batch_mode_driver_p->ss_setup_path = ss_setup_path;
    batch_mode_driver_p->load_ss_setup();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::choice4_clicked()
{
  if(first_ss_setup_displayed+3<ss_setup_list_p->size())
  {
    ss_setup_path.append(ss_setup_list_p->at(first_ss_setup_displayed+3));
    ss_setup_path.append(".ESC3sss");
    batch_mode_driver_p->ss_setup_path = ss_setup_path;
    batch_mode_driver_p->load_ss_setup();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::choice5_clicked()
{
  if(first_ss_setup_displayed+4<ss_setup_list_p->size())
  {
    ss_setup_path.append(ss_setup_list_p->at(first_ss_setup_displayed+4));
    ss_setup_path.append(".ESC3sss");
    batch_mode_driver_p->ss_setup_path = ss_setup_path;
    batch_mode_driver_p->load_ss_setup();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::choice6_clicked()
{
  if(first_ss_setup_displayed+5<ss_setup_list_p->size())
  {
    ss_setup_path.append(ss_setup_list_p->at(first_ss_setup_displayed+5));
    ss_setup_path.append(".ESC3sss");
    batch_mode_driver_p->ss_setup_path = ss_setup_path;
    batch_mode_driver_p->load_ss_setup();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::more_ss_setups_clicked()
{
  if(first_ss_setup_displayed<(ss_setup_list_p->size()-6)) first_ss_setup_displayed += 6;
  display_ss_setups();
}

void ss_setup_choice::add_ss_setup_clicked()
{
  centre_p->add_waiting_screen(23);//enter_ss_setup
  cout<<"ss_setup_choice::add_ss_setup_clicked 1\n";
  batch_mode_driver_p->clear_ss_setup();
  cout<<"ss_setup_choice::add_ss_setup_clicked 2\n";batch_mode_driver_p->ss_setup_path = "";
  centre_p->screen_done=true;
  cout<<"ss_setup_choice::add_ss_setup_clicked 3\n";
}

void ss_setup_choice::delete_ss_setup_clicked()
{
  centre_p->add_waiting_screen(30);//ss_setup_delete
  centre_p->screen_done=true;
}

void ss_setup_choice::back_clicked()
{
  if(first_ss_setup_displayed>5) 
  {
    first_ss_setup_displayed -= 6;
    display_ss_setups();
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void ss_setup_choice::display_ss_setups()
{
  if(first_ss_setup_displayed<ss_setup_list_p->size())    choice1_p->setText(ss_setup_list_p->at(first_ss_setup_displayed));
  else choice1_p->setText("");
  if(first_ss_setup_displayed+1<ss_setup_list_p->size())  choice2_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+1));
  else choice2_p->setText("");
  if(first_ss_setup_displayed+2<ss_setup_list_p->size())  choice3_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+2));
  else choice3_p->setText("");
  if(first_ss_setup_displayed+3<ss_setup_list_p->size())  choice4_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+3));
  else choice4_p->setText("");
  if(first_ss_setup_displayed+4<ss_setup_list_p->size())  choice5_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+4));
  else choice5_p->setText("");
  if(first_ss_setup_displayed+5<ss_setup_list_p->size())  choice6_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+5));
  else choice6_p->setText("");
}
