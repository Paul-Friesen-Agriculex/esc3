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

#ifndef enter_program_hpp
#define enter_program_hpp

#include <QList>
#include "centre.hpp"
#include "batch_mode_driver.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QString;
class QTextEdit;
class QTimer;
class keypad;
class help_screen;

class enter_program : public screen
{
  Q_OBJECT
  
  public:
  enter_program(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~enter_program();
  
  private slots:
  void options_clicked();
  void back_clicked();
  void number_entered(int value);
  void help_clicked();
  void done_clicked();
  
  private:
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  QLabel* prompt_p;
  keypad* keypad_p;
  button* options_button_p;
  QTextEdit* edit_p;
  button* back_button_p;
  button* help_button_p;
  button* done_button_p;
  QGridLayout* layout_p;
  
  bm_set* bm_set_p;
  QString* program_string_p;
  void update_program_string();
  char enter_flag;//'p' -> number of packs.  's' -> number of seeds per pack
  int current_packs;
  int current_seeds;
  help_screen* help_screen_p;
};
#endif
