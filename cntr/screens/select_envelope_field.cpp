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
#include "select_envelope_field.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

select_envelope_field::select_envelope_field(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{

    cout<<"start select_envelope_field::select_envelope_field\n";

  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"select_envelope_field::select_envelope_field\n";
//  batch_mode_driver_p->list_ss_setup();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_headings_p=new button("More headings");
  previous_headings_p=new button("Previous headings");
  next_p=new button("Next");
  back_p=new button("Back");
  message_p=new QLabel("");

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
  
  bottom_layout_p->addWidget(previous_headings_p,0,0);
  bottom_layout_p->addWidget(more_headings_p,0,1);
  bottom_layout_p->addWidget(next_p,0,2);
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
  connect(more_headings_p, SIGNAL(clicked()), this, SLOT(more_headings_clicked()));
  connect(previous_headings_p, SIGNAL(clicked()), this, SLOT(previous_headings_clicked()));
  connect(next_p, SIGNAL(clicked()), this, SLOT(next_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
//  QString crop_name = centre_p->crops[0].name;
//  ss_setup_path = "ss_setups/";
//  ss_setup_path.append(crop_name);
//  ss_setup_path.append("/");
//  dir_p = new QDir(ss_setup_path);
  first_heading_displayed = 0;
  display_headings();
  
    cout<<"end select_envelope_field::select_envelope_field\n";
  
}

select_envelope_field::~select_envelope_field()
{
//  delete dir_p;
//  dir_p = 0;
}

void select_envelope_field::choice1_clicked()
{
  enter_choice(first_heading_displayed);
}

void select_envelope_field::choice2_clicked()
{
  enter_choice(first_heading_displayed+1);
}

void select_envelope_field::choice3_clicked()
{
  enter_choice(first_heading_displayed+2);
}

void select_envelope_field::choice4_clicked()
{
  enter_choice(first_heading_displayed+3);
}

void select_envelope_field::choice5_clicked()
{
  enter_choice(first_heading_displayed+4);
}

void select_envelope_field::choice6_clicked()
{
  enter_choice(first_heading_displayed+5);
}

void select_envelope_field::enter_choice(int column)
{
  if(batch_mode_driver_p->field_data_source_flag == 'd') batch_mode_driver_p->envelope_p->enter_ss_data_field(batch_mode_driver_p->get_spreadsheet_column_pointer(column));
  if(batch_mode_driver_p->field_data_source_flag == 'h') batch_mode_driver_p->envelope_p->enter_ss_heading_field(batch_mode_driver_p->get_spreadsheet_column_pointer(column));
  centre_p->add_waiting_screen(35);//position_envelope_field
  centre_p->screen_done = true;
}

void select_envelope_field::more_headings_clicked()
{
  first_heading_displayed += 6;
  display_headings();
}

void select_envelope_field::previous_headings_clicked()
{
  first_heading_displayed -= 6;
  if(first_heading_displayed<0) first_heading_displayed = 0;
  display_headings();
}

void select_envelope_field::next_clicked()
{
  centre_p->add_waiting_screen(35);//position envelope field
  centre_p->screen_done=true;
}

void select_envelope_field::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void select_envelope_field::display_headings()
{
//  cout<<"select_envelope_field::display_headings."<<endl;
  
  spreadsheet_column* column_p;

  QString message_text("Pick field name");
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

//  cout<<"end select_envelope_field::display_headings.  mode="<<mode<<endl;
}
