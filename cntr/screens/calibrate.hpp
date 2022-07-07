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

#ifndef calibrate_hpp
#define calibrate_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QSlider;
class QTextEdit;
class QTimer;
class help_screen;

class calibrate : public screen
{
  Q_OBJECT
  
  public:
  calibrate(centre*set_centre_p);
  ~calibrate();
  
  private slots:
  void gateset_clicked();
  void back_clicked();
  void zero_clicked();
  void endgate_clicked();
  void startstop_clicked();
  void change_speed(int value);
  void help_button_clicked();
//  void skip_clicked();
  void done_clicked();
  void run();
  
  private:
  QLabel* count_message_p;
  button* gateset_button_p;
  button* back_button_p;
  button* zero_button_p;
  button* endgate_button_p;
  QLabel* speed_label_p;
  button* startstop_button_p;
  QSlider* speed_set_p;
  QTextEdit* message_box_p;
  button* help_button_p;
//  button* skip_button_p;
  button* done_button_p;
  
  QGroupBox* top_box_p;
  QGroupBox* control_box_p;
  QGroupBox* speed_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* control_layout_p;
  QGridLayout* speed_layout_p;
  QGridLayout* bottom_layout_p;
  
  QGridLayout* main_layout_p;
  
  help_screen* help_screen_p;
  
  bool feeder_is_running;
  ENDGATE_STATE screen_endgate;//state of endgate according to screen control.
    //If this is open, endgate will be open regardless of envelope sensor.
    //If this is closed, it will be over-ridden by presence of an envelope
  bool calibration_message_posted;
  bool original_calibrated;//to store original state
//  bool newly_calibrated;//set true if callibration occurs
  
  QTimer* run_timer_p;
};
#endif

