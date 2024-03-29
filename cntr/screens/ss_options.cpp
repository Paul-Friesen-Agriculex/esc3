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
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include "batch_mode_driver.hpp"
#include "button.hpp"

#include "ss_options.hpp"

using namespace std;

ss_options::ss_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  
  title_p = new QLabel;
  
  back_button_p = new button("Back");
  envelope_layout_p = new button("Envelope layout");
  column_display_p = new button("Display columns");
  macro_menu_button_p = new button("Communications\nMacro Menu");
  done_button_p = new button("Done");

  scan_barcodes_p = new QRadioButton("Scan barcodes\non pre-printed envelopes");
  print_envelopes_p = new QRadioButton("Print envelopes\njust-in-time");

  start_on_pack_collect_p = new QRadioButton("Start printing\nwhen pack collected");
  start_on_previous_pack_collect_p = new QRadioButton("Start printing\nwhen previous pack\ncollected");
  preprint_batch_p = new QRadioButton("Preprint all envelopes\nwhen seed lot scanned");

  scan_or_print_group_p = new QGroupBox;
  scan_or_print_group_layout_p = new QGridLayout;
  scan_or_print_group_layout_p -> addWidget(scan_barcodes_p, 0, 0);
  scan_or_print_group_layout_p -> addWidget(print_envelopes_p, 0, 1);
  scan_or_print_group_p -> setLayout(scan_or_print_group_layout_p);

  print_control_group_p = new QGroupBox;
  print_control_group_layout_p = new QGridLayout;  
  print_control_group_layout_p -> addWidget(start_on_pack_collect_p, 0, 0);
  print_control_group_layout_p -> addWidget(start_on_previous_pack_collect_p, 0, 1);
  print_control_group_layout_p -> addWidget(preprint_batch_p, 0, 2);
  print_control_group_p -> setLayout(print_control_group_layout_p);
  print_control_group_p -> setTitle("Print control");
  
  main_layout_p = new QGridLayout;
  main_layout_p -> addWidget(back_button_p, 0, 2);
  main_layout_p -> addWidget(column_display_p, 1, 0);
  main_layout_p -> addWidget(macro_menu_button_p, 1, 1);
  main_layout_p -> addWidget(envelope_layout_p, 1, 2);
  main_layout_p -> addWidget(scan_or_print_group_p, 2, 0, 1, 3);
  main_layout_p -> addWidget(print_control_group_p, 3, 0, 1, 3);
  main_layout_p -> addWidget(done_button_p, 4, 2);
  setLayout(main_layout_p);
  
//  require_pack_barcode_p -> setAutoExclusive(false);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(start_on_pack_collect_p, SIGNAL(toggled(bool)), this, SLOT(start_on_pack_collect_clicked(bool)));
  connect(start_on_previous_pack_collect_p, SIGNAL(toggled(bool)), this, SLOT(start_on_previous_pack_collect_clicked(bool)));
  connect(preprint_batch_p, SIGNAL(toggled(bool)), this, SLOT(preprint_batch_clicked(bool)));
  connect(envelope_layout_p, SIGNAL(clicked()), this, SLOT(envelope_layout_button_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(column_display_p, SIGNAL(clicked()), this, SLOT(column_display_button_clicked()));
  connect(macro_menu_button_p, SIGNAL(clicked()), this, SLOT(macro_menu_button_clicked()));	
  connect(scan_barcodes_p, SIGNAL(clicked()), this, SLOT(scan_barcodes_clicked()));	
  connect(print_envelopes_p, SIGNAL(clicked()), this, SLOT(print_envelopes_clicked()));	

  scan_barcodes_p -> setChecked(!batch_mode_driver_p->print_envelope);
  print_envelopes_p -> setChecked(batch_mode_driver_p->print_envelope);
  
  if(batch_mode_driver_p->print_control_mode == start_on_pack_collect)
  {
    start_on_pack_collect_p->setChecked(true);
  }
  if(batch_mode_driver_p->print_control_mode == start_on_previous_pack_collect)
  {
    start_on_previous_pack_collect_p->setChecked(true);
  }
  if(batch_mode_driver_p->print_control_mode == preprint_batch)
  {
    preprint_batch_p->setChecked(true);
  }
  
  if(batch_mode_driver_p->print_envelope == false)
  {
    print_control_group_p->hide();
    envelope_layout_p->hide();
    /*
    start_on_pack_collect_p->hide();
    start_on_previous_pack_collect_p->hide();
    preprint_batch_p->hide();
    envelope_layout_p->hide();
    */
  }
  
}


void ss_options::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void ss_options::done_button_clicked()
{
  batch_mode_driver_p->print_envelope = print_envelopes_p->isChecked();
  centre_p->add_waiting_screen(33);//ss_batch
  centre_p->screen_done = true;
}

void ss_options::envelope_layout_button_clicked()
{
  centre_p->add_waiting_screen(25);//set_envelope_size
  centre_p->screen_done = true;
}

void ss_options::column_display_button_clicked()
{
//  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
//  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
//  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
//  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
//  batch_mode_driver_p->pack_match_spreadsheet = pack_match_spreadsheet_p->isChecked();
//  batch_mode_driver_p->record_only = record_only_p->isChecked();
  centre_p -> add_waiting_screen(32);//ss_column_display_options
  centre_p -> screen_done = true;
}  
  
void ss_options::macro_menu_button_clicked()
{
  centre_p->add_waiting_screen(33);//back to ss_batch
  centre_p->add_waiting_screen(50);//batch_macro_type_choice
  centre_p->screen_done=true;
}

void ss_options::start_on_pack_collect_clicked(bool val)
{
  if(val) batch_mode_driver_p -> print_control_mode = start_on_pack_collect;
}

void ss_options::start_on_previous_pack_collect_clicked(bool val)
{
  if(val) batch_mode_driver_p -> print_control_mode = start_on_previous_pack_collect;
}

void ss_options::preprint_batch_clicked(bool val)
{
  if(val) batch_mode_driver_p -> print_control_mode = preprint_batch;
}

void ss_options::scan_barcodes_clicked()
{
//  cout<<"ss_options::scan_barcodes_clicked\n";
  print_control_group_p->hide();
  envelope_layout_p->hide();
  batch_mode_driver_p->print_envelope = false;
}

void ss_options::print_envelopes_clicked()
{
//  cout<<"ss_options::print_envelopes_clicked\n";
  print_control_group_p->show();
  envelope_layout_p->show();
  batch_mode_driver_p->print_envelope = true;
}

