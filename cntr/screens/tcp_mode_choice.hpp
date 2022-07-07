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

#ifndef tcp_mode_choice_hpp
#define tcp_mode_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class tcp_mode_choice : public screen
{
  Q_OBJECT
  
  public:
  tcp_mode_choice(centre*set_centre_p);
  
  private slots:
  void back_button_clicked();
  void function_as_server();
  void function_as_client();
  void help_button_clicked();

  private:
  button* back_button_p;
  button* function_as_server_button_p;
  button* function_as_client_button_p;
  button* help_button_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
