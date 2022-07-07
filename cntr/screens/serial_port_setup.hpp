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

#ifndef serial_port_setup_hpp
#define serial_port_setup_hpp

#include "centre.hpp"
#include <QString>
class button;
class QRadioButton;
class QGroupBox;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class help_screen;

class serial_port_setup : public screen
{
  Q_OBJECT
  
  public:
  serial_port_setup(centre*set_centre_p);
  ~serial_port_setup();
  
  private slots:
  void back_button_clicked();
  void done_button_clicked();
  void serial_port_name_button_clicked();

  private:
  button* back_button_p;
  QRadioButton* b9600_p;
  QRadioButton* b19200_p;
  QRadioButton* b38400_p;
  QRadioButton* b57600_p;
  QRadioButton* b115200_p;
  QLabel* serial_port_name_label_p;
  button* serial_port_name_button_p;
  button* done_button_p;

  QLabel* message_p;
  QGroupBox* baud_box_p;
  QVBoxLayout* baud_layout_p;
  QGridLayout* main_layout_p;
  
};
#endif
