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

#ifndef select_field_data_source_hpp
#define select_field_data_source_hpp

#include "centre.hpp"

class button;
class QGridLayout;
//class QVBoxLayout;
class QLabel;

class select_field_data_source : public screen
{
  Q_OBJECT
  
  public:
  select_field_data_source(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  
  private slots:
    void back_clicked();
//    void edit_layout_only_clicked();
	  void ss_data_button_clicked();
	  void ss_heading_button_clicked();
	  void text_entry_button_clicked();
  
  private:
    button* back_button_p;
//    button* edit_layout_only_button_p;
	  QLabel* message_p;
	  button* ss_data_button_p;
	  button* ss_heading_button_p;
	  button* text_entry_button_p;
	  QGridLayout* layout_p;
    batch_mode_driver* batch_mode_driver_p;
};
#endif
