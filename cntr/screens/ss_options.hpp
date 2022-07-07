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

#ifndef ss_options_hpp
#define ss_options_hpp

#include "centre.hpp"

class button;
//class QGridLayout;
class QGroupBox;
//class QVBoxLayout;
class QGridLayout;
class QLabel;
class QRadioButton;
//class QSlider;
//class QTimer;
//class QFileDialog;
class batch_mode_driver;
//class message_box;

class ss_options : public screen
{
  Q_OBJECT
  
  public:
  ss_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
//  ~batch();
  
  private slots:
  void back_button_clicked();
  void envelope_layout_button_clicked();
  void column_display_button_clicked();
  void macro_menu_button_clicked();
  
  void start_on_pack_collect_clicked(bool val);
  void start_on_previous_pack_collect_clicked(bool val);
  void preprint_batch_clicked(bool val);
  

  
  private:
  batch_mode_driver* batch_mode_driver_p;
  QLabel* title_p;
  QRadioButton* scan_barcodes_p;
  QRadioButton* print_envelopes_p;
  
  QRadioButton* start_on_pack_collect_p;
  QRadioButton* start_on_previous_pack_collect_p;
  QRadioButton* preprint_batch_p;
  
  button* back_button_p;
  button* envelope_layout_p;
  button* column_display_p;
  button* macro_menu_button_p;
  button* done_button_p;
  
  QGroupBox* scan_or_print_group_p;
  QGridLayout* scan_or_print_group_layout_p;
  QGroupBox* print_control_group_p;
  QGridLayout* print_control_group_layout_p;
  QGridLayout* main_layout_p;
  
  
  public slots:
  void done_button_clicked();
  void scan_barcodes_clicked();
  void print_envelopes_clicked();
};


#endif
