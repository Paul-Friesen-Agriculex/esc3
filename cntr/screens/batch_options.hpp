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

#ifndef batch_options_hpp
#define batch_options_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QLabel;
class QRadioButton;
class QSpinBox;
class batch_mode_driver;
class help_screen;

class batch_options : public screen
{
  Q_OBJECT
  
  public:
  batch_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  
  private slots:
  void barcode_switch_toggled(bool checked);
  void help_button_clicked();
  void macro_menu_button_clicked();

  
  private:
  batch_mode_driver* batch_mode_driver_p;
  QLabel* title_p;
  QRadioButton* require_seed_lot_barcode_p;
  QRadioButton* require_pack_barcode_p;
  QRadioButton* pack_match_lot_p;
  QRadioButton* pack_contain_lot_p;
  QRadioButton* lot_contain_pack_p;
  QRadioButton* pack_match_spreadsheet_p;
  QRadioButton* record_only_p;
  QRadioButton* use_cutgate_p;
  QSpinBox* seed_feeder_stop_allowance_p;
  QLabel* seed_feeder_stop_allowance_label_p;
  
  button* back_button_p;
  button* done_button_p;
  button* macro_menu_button_p;
  button* help_button_p;
  
  QGroupBox* barcode_matching_group_p;
  QGroupBox* cutgate_option_group_p;
  QGridLayout* barcode_matching_group_layout_p;
  QGridLayout* cutgate_option_group_layout_p;
  QGridLayout* main_layout_p;
  
  help_screen* help_screen_p;
  
  bool original_require_seed_lot_barcode;
  
  public slots:
  void done_button_clicked();
  void back_button_clicked();
};


#endif
