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

#ifndef totalize_options_hpp
#define totalize_options_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class totalize_options : public screen
{
  Q_OBJECT
  
  public:
  totalize_options(centre*set_centre_p);
  
  private slots:
  void back_button_clicked();
  void zero_when_seed_discharged_changed(bool value);
  void number_of_barcode_columns_changed(int value);
  void autosave_limit_changed(int value);
  void ok_button_clicked();
  void macro_menu_button_clicked();

  private:
  button* back_button_p;
  button* macro_menu_button_p;
  QRadioButton* zero_when_seed_discharged_p;
  
  QSpinBox* number_of_barcode_columns_p;
  QLabel* number_of_barcode_columns_label_p;
  QGroupBox* number_of_barcode_columns_group_p;
  QHBoxLayout* number_of_barcode_columns_layout_p;
  
  QSpinBox* autosave_limit_p;
  QLabel* autosave_limit_label_p;
  QGroupBox* autosave_limit_group_p;
  QHBoxLayout* autosave_limit_layout_p;
  
  button* ok_button_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
