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

#ifndef playback_hpp
#define playback_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QLabel;
class QImage;
class QTimer;
class QSlider;
class QLineEdit;
class display;
class blobdisp;

class playback : public screen
{
  Q_OBJECT
  
  public:
  playback(centre*set_centre_p);
  ~playback();
  
  private slots:
  void autoadvance_clicked();
  void back_clicked();
  void zero_clicked();
  void advance1_clicked();
  void advance10_clicked();
  void advance100_clicked();
  void advance_n();
  void set_n(int n_s);
  void quit_clicked();
  void run();
  void save_image();
  void load_image();
  
  private:
  QLabel* title_p;
  button* back_button_p;
  QLabel* count_message_p;
  button* zero_button_p;
  button* advance1_button_p;
  button* advance10_button_p;
  button* advance100_button_p;
  button* file_button_p;
  button* load_file_button_p;
  button* quit_button_p;
  button* autoadvance_button_p;
  QSlider* autoadvance_slider_p;
//  button* save_image_button_p;
  QLineEdit* file_name_p;
  QGroupBox* autoadvance_box_p;
  QGridLayout* autoadvance_layout_p;
  display* display_p;
  blobdisp* blobdisp_p;

  QGridLayout* main_layout_p;
  
  QTimer* qtimer_p;
  QTimer* autoadvance_timer_p;
  bool autoadvance_on;
  int n;//number of lines to advance on each autoadvance pulse
  
  
  signals:
  void send_line(unsigned char* line_start_p);
};
#endif


