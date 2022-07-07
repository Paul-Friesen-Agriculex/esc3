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

#ifndef batch_macro_type_choice_hpp
#define batch_macro_type_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
//class QVBoxLayout;
class QLabel;
class QRadioButton;
//class QSlider;
//class QTimer;
//class QFileDialog;
class batch_mode_driver;
//class message_box;

class batch_macro_type_choice : public screen
{
  Q_OBJECT
  
  public:
  batch_macro_type_choice(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  
  private slots:

  
  private:
  batch_mode_driver* batch_mode_driver_p;
  
  QLabel* instruction_label_p;
  button* back_button_p;
  button* pack_macro_button_p;
  button* dump_macro_button_p;
  button* substitution_macro_button_p;
  button* setup_button_p;
  button* done_button_p;
  
  QLabel* pack_label_p;
  QLabel* dump_label_p;
  QLabel* substitution_label_p;
  
  QGridLayout* main_layout_p;
  
  public slots:
  void back_button_clicked();
  void pack_macro_button_clicked();
  void dump_macro_button_clicked();
  void substitution_macro_button_clicked();
  void setup_button_clicked();
  void done_button_clicked();
};


#endif
