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

#ifndef ss_other_options_hpp
#define ss_other_options_hpp

#include "centre.hpp"
#include <QPushButton>  //TEST~~~ replace all buttons with QPushButtons widget buttons//

class button;
class QGridLayout;
class QGroupBox;
class help_screen;


class ss_other_options : public screen
{
  Q_OBJECT
  
  public:
  ss_other_options(centre*set_centre_p);
  
  public slots:
  void back_button_clicked();
  void print_button_clicked();
  void display_columns_button_clicked();
  void done_button_clicked();
  void help_button_clicked();
  
  private:
  button* back_button_p;
  button* print_button_p;
  button* display_columns_button_p;
  button* done_button_p;
  button* help_button_p;
  QGridLayout* main_layout_p;
  
  help_screen* help_screen_p;
};
#endif
