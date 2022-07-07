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
#include "button.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include "batch_mode_driver.hpp"
#include "help_screen.hpp"

#include "batch_options.hpp"

using namespace std;

batch_options::batch_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  
  title_p = new QLabel;
  require_seed_lot_barcode_p = new QRadioButton("Require seed lot barcode");
  require_pack_barcode_p = new QRadioButton("Require pack barcode");
  pack_match_lot_p = new QRadioButton("Pack barcode must match seed lot barcode");
  pack_contain_lot_p = new QRadioButton("Pack barcode must contain seed lot barcode");
  lot_contain_pack_p = new QRadioButton("Seed lot barcode must contain pack barcode");
  record_only_p = new QRadioButton("Record barcodes only - no matching");
  use_cutgate_p = new QRadioButton("Use cutgate");
  seed_feeder_stop_allowance_p = new QSpinBox();
  seed_feeder_stop_allowance_label_p = new QLabel("Seed feeder stop allowance");
  back_button_p = new button("Back");
  help_button_p = new button("Help");
  done_button_p = new button("Done");
  macro_menu_button_p = new button("Communications Macro Menu");
  
  barcode_matching_group_p = new QGroupBox;
  barcode_matching_group_layout_p = new QGridLayout;
  cutgate_option_group_p = new QGroupBox;
  cutgate_option_group_layout_p = new QGridLayout;
  main_layout_p = new QGridLayout;
  
  barcode_matching_group_layout_p -> addWidget(pack_match_lot_p,0,0);
  barcode_matching_group_layout_p -> addWidget(pack_contain_lot_p,0,1);
  barcode_matching_group_layout_p -> addWidget(lot_contain_pack_p,1,0);
  barcode_matching_group_layout_p -> addWidget(record_only_p,1,1);
  barcode_matching_group_p -> setLayout(barcode_matching_group_layout_p);
  
  cutgate_option_group_layout_p -> addWidget(use_cutgate_p, 0, 0);
  cutgate_option_group_layout_p -> addWidget(seed_feeder_stop_allowance_p, 0, 1);
  cutgate_option_group_layout_p -> addWidget(seed_feeder_stop_allowance_label_p, 1, 1);
  cutgate_option_group_p -> setLayout(cutgate_option_group_layout_p);
  
  main_layout_p -> addWidget(require_seed_lot_barcode_p,0,0);
  main_layout_p -> addWidget(require_pack_barcode_p,0,1);
  main_layout_p -> addWidget(back_button_p,0,2);
  main_layout_p -> addWidget(barcode_matching_group_p,1,0,1,3);
//  main_layout_p -> addWidget(cutgate_option_group_p,2,0,1,3);
  main_layout_p -> addWidget(help_button_p, 3, 0);
  main_layout_p -> addWidget(macro_menu_button_p, 3, 1);
  main_layout_p -> addWidget(done_button_p,3,2);
  setLayout(main_layout_p);
  
  require_seed_lot_barcode_p -> setAutoExclusive(false);
  require_pack_barcode_p -> setAutoExclusive(false);
  
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(require_seed_lot_barcode_p, SIGNAL(toggled(bool)), this, SLOT(barcode_switch_toggled(bool)));
  connect(require_pack_barcode_p, SIGNAL(toggled(bool)), this, SLOT(barcode_switch_toggled(bool)));
  connect(macro_menu_button_p, SIGNAL(clicked()), this, SLOT(macro_menu_button_clicked()));

  require_seed_lot_barcode_p -> setChecked(batch_mode_driver_p->require_seed_lot_barcode);
  require_pack_barcode_p -> setChecked(batch_mode_driver_p->require_pack_barcode);
  pack_match_lot_p -> setChecked(batch_mode_driver_p->pack_match_lot);
  pack_contain_lot_p -> setChecked(batch_mode_driver_p->pack_contain_lot);
  lot_contain_pack_p -> setChecked(batch_mode_driver_p->lot_contain_pack);
  record_only_p -> setChecked(batch_mode_driver_p->record_only);
  
  original_require_seed_lot_barcode = batch_mode_driver_p->require_seed_lot_barcode;
}

void batch_options::done_button_clicked()
{
  batch_mode_driver_p->require_seed_lot_barcode = require_seed_lot_barcode_p->isChecked();
  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
  batch_mode_driver_p->record_only = record_only_p->isChecked();
  
  if(  (batch_mode_driver_p->require_seed_lot_barcode==true)  &&  (original_require_seed_lot_barcode==false)  )
  {
//    batch_mode_driver_p->mode = wait_for_seed_lot_barcode;
//    batch_mode_driver_p->seed_lot_barcode_ok = false;
//    batch_mode_driver_p->seed_lot_barcode_old = true;
  }
  
//  centre_p->add_waiting_screen(15);//batch
  centre_p->screen_done = true;
}

void batch_options::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void batch_options::help_button_clicked()
{
  help_screen_p = new help_screen;
  help_screen_p -> setGeometry(geometry());
  help_screen_p -> set_text(
    "This screen allows you to set up barcode checking, to guard against errors in packet filling.  "
    "If both \"Require seed lot barcode\" and \"Require pack barcode\" are checked, barcode checking is possible.  "
    "If, for example, you then choose \"Pack barcode must match seed lot barcode\", the user will be prompted to scan a "
    "barcode for each seed lot.  "
    "The feeder will not start until a barcode is scanned.  "
    "For each pack, the user will again be prompted for a barcode.  "
    "If no barcode is scanned, or if the barcodes do not match, the end gate will not open to discharge the seed until the "
    "problem is fixed.  ");
  help_screen_p->show();
}    
    

void batch_options::barcode_switch_toggled(bool checked)
{
  if(checked == false)
  {
    record_only_p->setChecked(true);
  }
  bool barcode_matching_possible = (  require_seed_lot_barcode_p->isChecked()  &&  require_pack_barcode_p->isChecked()  );
  pack_match_lot_p->setEnabled(barcode_matching_possible);
  pack_contain_lot_p->setEnabled(barcode_matching_possible);
  lot_contain_pack_p->setEnabled(barcode_matching_possible);
}
  
void batch_options::macro_menu_button_clicked()
{
  cout<<"macro_menu_button_click"<<endl;	//OMIT~~~
//  centre_p->add_waiting_screen(28);//macro_screen

  batch_mode_driver_p->require_seed_lot_barcode = require_seed_lot_barcode_p->isChecked();
  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
  batch_mode_driver_p->record_only = record_only_p->isChecked();

  centre_p->add_waiting_screen(50);//batch_macro_type_choice
  centre_p->screen_done=true;
}
