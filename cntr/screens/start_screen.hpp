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

#ifndef start_screen_hpp
#define start_screen_hpp

#include "centre.hpp"
#include <QPushButton>  //TEST~~~ replace all buttons with QPushButtons widget buttons//

class button;
class QGridLayout;
class QGroupBox;
class help_screen;


class start_screen : public screen
{
  Q_OBJECT
  
  public:
  start_screen(centre*set_centre_p);
  
  public slots:
  void totalize_clicked();
  void batch_clicked();
  void batch_from_spreadsheet_clicked();
  void tools_clicked();
  void slave_mode_clicked();
  void eject_memory_sticks_clicked();
  void help_button_clicked();
  void shutdown();
//  void exit_program();
  
  private:
  button* totalize_button_p;
  button* batch_button_p;
  button* batch_from_spreadsheet_button_p;
  button* tools_button_p;
  button* slave_mode_button_p;
  button* eject_memory_sticks_button_p;
  button* shutdown_button_p;
  button* help_button_p;
//  button* exit_button_p;
  QGridLayout* modebox_layout_p;
  QGridLayout* shutdownbox_layout_p;
  QGroupBox* modebox_p;
  QGroupBox* shutdownbox_p;
  QGridLayout* main_layout_p;
  
  help_screen* help_screen_p;
};
#endif
