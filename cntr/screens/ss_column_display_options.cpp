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
#include "ss_column_display_options.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

ss_column_display_options::ss_column_display_options(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;

  mode = 'a';//add more display columns

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  next_headings_p=new button("More headings");
  previous_headings_p=new button("Previous headings");
  done_p=new button("Done");
  back_p=new button("Back");
  add_remove_p=new button("Remove columns");
  message_p=new QLabel("");

  top_box_p=new QGroupBox;
  middle_box_p=new QGroupBox;
  bottom_box_p=new QGroupBox;
  
  top_layout_p=new QGridLayout;
  middle_layout_p=new QGridLayout;
  bottom_layout_p=new QGridLayout;
  
  main_layout_p=new QVBoxLayout;
  
  top_layout_p->addWidget(message_p,0,0,2,1);
  top_layout_p->addWidget(back_p, 0, 1);
  top_layout_p->addWidget(add_remove_p, 1, 1);
  top_box_p->setLayout(top_layout_p);
  
  middle_layout_p->addWidget(choice1_p,0,0);
  middle_layout_p->addWidget(choice2_p,0,1);
  middle_layout_p->addWidget(choice3_p,1,0);
  middle_layout_p->addWidget(choice4_p,1,1);
  middle_layout_p->addWidget(choice5_p,2,0);
  middle_layout_p->addWidget(choice6_p,2,1);
  middle_box_p->setLayout(middle_layout_p);
  
  bottom_layout_p->addWidget(previous_headings_p,0,0);
  bottom_layout_p->addWidget(next_headings_p,0,1);
  bottom_layout_p->addWidget(done_p,0,2);
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
  connect(next_headings_p, SIGNAL(clicked()), this, SLOT(next_headings_clicked()));
  connect(previous_headings_p, SIGNAL(clicked()), this, SLOT(previous_headings_clicked()));
  connect(done_p, SIGNAL(clicked()), this, SLOT(done_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(add_remove_p, SIGNAL(clicked()), this, SLOT(add_remove_clicked()));
  
  QString crop_name = centre_p->crops[0].name;
  ss_setup_path = "ss_setups/";
  ss_setup_path.append(crop_name);
  ss_setup_path.append("/");
  dir_p = new QDir(ss_setup_path);
  first_heading_displayed = 0;
  display_headings();
}

ss_column_display_options::~ss_column_display_options()
{
  delete dir_p;
  dir_p = 0;
}

void ss_column_display_options::choice1_clicked()
{
  enter_choice(first_heading_displayed);
}

void ss_column_display_options::choice2_clicked()
{
  enter_choice(first_heading_displayed+1);
}

void ss_column_display_options::choice3_clicked()
{
  enter_choice(first_heading_displayed+2);
}

void ss_column_display_options::choice4_clicked()
{
  enter_choice(first_heading_displayed+3);
}

void ss_column_display_options::choice5_clicked()
{
  enter_choice(first_heading_displayed+4);
}

void ss_column_display_options::choice6_clicked()
{
  enter_choice(first_heading_displayed+5);
}

void ss_column_display_options::enter_choice(int column)
{
  if(mode == 'a')//add choice
  {
    batch_mode_driver_p->display_column_numbers.append(column);
  }
  else //remove choice
  {
    batch_mode_driver_p->display_column_numbers.removeAll(column);
  }
  display_headings();
}

void ss_column_display_options::next_headings_clicked()
{
  first_heading_displayed += 6;
  display_headings();
}

void ss_column_display_options::previous_headings_clicked()
{
  first_heading_displayed -= 6;
  if(first_heading_displayed<0) first_heading_displayed = 0;
  display_headings();
}

void ss_column_display_options::done_clicked()
{
  centre_p->add_waiting_screen(31);//ss_options
  centre_p->screen_done = true;
}

void ss_column_display_options::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void ss_column_display_options::add_remove_clicked()
{
  if(mode == 'a')
  {
    mode = 'r';
    add_remove_p -> setText("Add columns");
  }
  else//mode was 'r'
  {
    mode = 'a';
    add_remove_p -> setText("Remove columns");
  }
  display_headings();
}

void ss_column_display_options::display_headings()
{
//  cout<<"ss_column_display_options::display_headings."<<endl;
  
  spreadsheet_column* column_p;

  QString message_text;
  if(mode == 'a') message_text = "Pick column to display.  Current display columns:";
  else message_text = "Pick a column to remove.  Current display columns:";
//  message_text.append(QString::number(batch_mode_driver_p->display_column_numbers.size()));
  for(int i=0; i<batch_mode_driver_p->display_column_numbers.size(); ++i)
  {
    message_text.append("\n   ");
    column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(batch_mode_driver_p->display_column_numbers[i]);
    message_text.append(column_p->heading);
  }
  message_p->setText(message_text);
  
  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed);
  if(column_p==0) choice1_p->setText("");
  else choice1_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+1);
  if(column_p==0) choice2_p->setText("");
  else choice2_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+2);
  if(column_p==0) choice3_p->setText("");
  else choice3_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+3);
  if(column_p==0) choice4_p->setText("");
  else choice4_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+4);
  if(column_p==0) choice5_p->setText("");
  else choice5_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+5);
  if(column_p==0) choice6_p->setText("");
  else choice6_p->setText(column_p->heading);

//  cout<<"end ss_column_display_options::display_headings.  mode="<<mode<<endl;
}
