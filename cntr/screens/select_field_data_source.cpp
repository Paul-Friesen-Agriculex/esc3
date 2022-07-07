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
#include <QLabel>
#include "centre.hpp"
#include "batch_mode_driver.hpp"
#include "select_field_data_source.hpp"
#include "button.hpp"

using namespace std;

select_field_data_source::select_field_data_source(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  back_button_p = new button("Back");
  message_p = new QLabel("Select type of data for a new field.");
  ss_data_button_p = new button("Spreadsheet data.  Different for each envelope.");
  ss_heading_button_p = new button("Spreadsheet heading.  Same for each envelope.");
  text_entry_button_p = new button("Enter text.  Same text entered on each envelope.");
  layout_p = new QGridLayout;
  layout_p->addWidget(back_button_p, 0, 1);
  layout_p->addWidget(message_p, 1, 0);
  layout_p->addWidget(ss_data_button_p, 2, 0);
  layout_p->addWidget(ss_heading_button_p, 3, 0);
  layout_p->addWidget(text_entry_button_p, 4, 0);
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(ss_data_button_p, SIGNAL(clicked()), this, SLOT(ss_data_button_clicked()));
  connect(ss_heading_button_p, SIGNAL(clicked()), this, SLOT(ss_heading_button_clicked()));
  connect(text_entry_button_p, SIGNAL(clicked()), this, SLOT(text_entry_button_clicked()));
  setLayout(layout_p);
//  cout<<"end select_field_data_source::select_field_data_source\n";
}

void select_field_data_source::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void select_field_data_source::ss_data_button_clicked()
{
  batch_mode_driver_p->field_data_source_flag = 'd';
  centre_p->add_waiting_screen(34);//select_envelope_field
  centre_p->screen_done = true;
}

void select_field_data_source::ss_heading_button_clicked()
{
  batch_mode_driver_p->field_data_source_flag = 'h';
  centre_p->add_waiting_screen(34);//select_envelope_field
  centre_p->screen_done = true;
}

void select_field_data_source::text_entry_button_clicked()
{
  batch_mode_driver_p->field_data_source_flag = 't';
  centre_p->add_waiting_screen(37);//enter_field_text
  centre_p->screen_done = true;
}



