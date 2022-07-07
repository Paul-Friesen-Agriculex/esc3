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

#ifndef macro_builder_hpp
#define macro_builder_hpp

#include "centre.hpp"

#include <QString>

class QSignalMapper;
class QLabel;
//class QLineEdit;
class QTextEdit;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QGroupBox;
class button;

class macro_builder:public screen
{
  Q_OBJECT
  
  QSignalMapper* signalMapper;
  QLabel* instruction_label_p;
//  QLineEdit* lineEdit;
  QTextEdit* lineEdit;
  QHBoxLayout *top_line_p;
  QHBoxLayout *input_line_modify;
  QVBoxLayout* form_p;
  QHBoxLayout *bottom_line_p;
    
  QPushButton *totalize_count_button;
  QPushButton *text_entry_button;
  QPushButton *batch_count_button;
  QPushButton *seed_button;
  QPushButton *barcode_button1;
  QPushButton *barcode_button2;
  QPushButton *barcode_button3;
  QPushButton *barcode_button4;
  QPushButton *weight_button;
  QPushButton *lotcode_button;
  QPushButton *packcode_button;
  QPushButton *substitution_button;
  QPushButton *dump_count_button;
  QPushButton *newline;
  QPushButton *horizontal_tab;
  QPushButton *spacebar;
  QPushButton *arrow_up;
  QPushButton *arrow_down;
  QPushButton *arrow_left;
  QPushButton *arrow_right;
  QPushButton *clear_entry;
  QPushButton *remove_last;
  
  QGroupBox *totalize_box;
  QVBoxLayout *barcodes;
  QHBoxLayout *barcodes_columns;
  QVBoxLayout *barcodes_left;
  QVBoxLayout *barcodes_right;
  QLabel *totalize_box_label;
  
  QGroupBox *accessibility_button_box;
  QVBoxLayout *accessibility_keys_container;
  QHBoxLayout *accessibility_keys;
  QVBoxLayout *accessibility_keys_left;
  QVBoxLayout *accessibility_keys_right;
  QLabel *accessibility_box_label;
  
  button* save_button_p;
  button* back_button_p;
  
  static const int macro_button_width = 160;
  static const int macro_button_height = 35;  
  
  public:
  macro_builder(centre* centre_p);
  
  private slots:
  void dialogbox_buttons(int n);
  void back_button_clicked();
  void save_button_clicked();
};
  


#endif
