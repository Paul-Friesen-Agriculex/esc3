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

#ifndef ss_column_display_options_hpp
#define ss_column_display_options_hpp

#include <QString>
#include "centre.hpp"
#include "batch_mode_driver.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QDir;
class QStringList;

class ss_column_display_options : public screen
{
  Q_OBJECT
  
  public:
  ss_column_display_options(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~ss_column_display_options();
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void next_headings_clicked();
  void previous_headings_clicked();
  void done_clicked();
  void back_clicked();
  void add_remove_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* next_headings_p;
  button* previous_headings_p;
  button* done_p;
  button* back_p;
  button* add_remove_p;
  
  void display_headings();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_heading_displayed;
  
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  
  QDir* dir_p;
  
  void enter_choice(int column);
  
  
  QString ss_setup_path;
  char mode; //'a'->add display columns.  'r'->remove display columns
};  


#endif
