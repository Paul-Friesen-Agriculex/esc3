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

#ifndef help_screen_hpp
#define help_screen_hpp

#include "centre.hpp"

class button;
class QTextEdit;
class QVBoxLayout;
class QString;

class help_screen : public screen
{
  Q_OBJECT
  
  public:
  help_screen();
  void set_text(QString text);
  
  private:
  QTextEdit* text_edit_p;
  button* exit_button_p;
  QVBoxLayout* layout_p;
};

#endif
  
