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

#ifndef gateset_hpp
#define gateset_hpp

#include "centre.hpp"

class button;
class QSpinBox;
class QGridLayout;
class QLabel;

class gateset : public screen
{
  Q_OBJECT
  
  public:
  gateset(centre*set_centre_p);
  
  private slots:
	  void ok_button_clicked();
//    void enter_value(int value);
  
  private:
	  QLabel* message_p;
    QSpinBox* spinbox_p;
	  button* ok_button_p;
	  QGridLayout* layout_p;
};
#endif
