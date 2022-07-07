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

#ifndef totalize_hpp
#define totalize_hpp
#include "table.hpp"
#include "centre.hpp"
#include "macro_screen.hpp"


class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QSlider;
class QTimer;
class QFileDialog;

class totalize : public screen
{
  Q_OBJECT
  
  public:
  totalize(centre*set_centre_p);
  ~totalize();
    
  private slots:
  void options_clicked();
  void back_clicked();
  void zero_clicked();
  void endgate_clicked();
  void startstop_clicked();
  void change_speed(int value);
  void save_clicked();
  void clear_table_clicked();
  void quit_clicked();
  void run();
  
  private:
  QLabel* count_message_p;
  button* options_button_p;
  button* back_button_p;
  button* zero_button_p;
  button* endgate_button_p;
  QLabel* speed_label_p;
  button* startstop_button_p;
  QSlider* speed_set_p;
  table* table_p;
  QLabel* status_box_p;
  button* save_button_p;
  button* clear_table_button_p;
  button* quit_button_p;
  
  QGroupBox* top_box_p;
  QGroupBox* control_box_p;
  QGroupBox* speed_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* control_layout_p;
  QGridLayout* speed_layout_p;
  QGridLayout* bottom_layout_p;
  
  QGridLayout* main_layout_p;
  
  bool feeder_is_running;
  ENDGATE_STATE screen_endgate;//state of endgate according to screen control.
    //If this is open, endgate will be open regardless of envelope sensor.
    //If this is closed, it will be over-ridden by presence of an envelope
  
  int count;
  int old_count;
  bool old_envelope_present;
  
  QTimer* run_timer_p;
//  macro_screen* macro_screen_p;
  QString count_string;
  
};

#endif

