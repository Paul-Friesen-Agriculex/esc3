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

#ifndef tool_choice_hpp
#define tool_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QRadioButton;
class help_screen;

class tool_choice : public screen
{
  Q_OBJECT
  
  public:
  tool_choice(centre*set_centre_p);
  
  private slots:
  void about_button_clicked();
  void crop_edit_clicked();
  void communications_setup_button_clicked();
  void diagnostics_clicked();
  void stop_program_clicked();
  void back_button_clicked();
  void signal_port_pulse_when_endgate_closes(bool state);

  private:
  button* about_button_p;
  button* crop_edit_button_p;
  button* communications_setup_button_p;
  button* diagnostics_button_p;
  button* stop_program_button_p;
  button* back_button_p;
  QRadioButton* signal_port_pulse_when_endgate_closes_button_p;

  QGridLayout* main_layout_p;
  help_screen* help_screen_p;
//  centre* centre_p;
};
#endif
