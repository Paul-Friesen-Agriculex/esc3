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

#ifndef position_envelope_field_hpp
#define position_envelope_field_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QRadioButton;
class QLabel;
class QSlider;
class QTimer;
class QImage;
class envelope;

class envelope_picture : public QWidget
{
  Q_OBJECT
  
  public:
  envelope_picture(QImage* qimage_s);
  void set_current_position(int x, int y);
  void set_start_displacement(int pixels_down);//envelope printing of image starts 10mm down from top.  
    //This sets the number of pixels down to start the image display to make this image match envelope.
  
  private:
  QImage* envelope_qimage_p;
  int current_x, current_y;
  int start_displacement;
  
  protected: 
  void paintEvent(QPaintEvent* event);
};

class position_envelope_field : public screen
{
  Q_OBJECT
  
  public:
  position_envelope_field(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~position_envelope_field();
  
  private slots:
	  void back_button_clicked();
    void select_previous_button_clicked();
    void select_next_button_clicked();
    void delete_selected_clicked();
    void clear_envelope_clicked();
    void test_print_button_clicked();
    void new_field_button_clicked();
    void done_button_clicked();
    void text_button_toggled(bool val);
    void code39_button_toggled(bool val);
    void set_x(int x_s);
    void set_y(int y_s);
    void set_h(int h_s);
    void run();
  
  private:
    batch_mode_driver* batch_mode_driver_p;
	  QLabel* message_p;
    button* back_button_p;
    button* select_previous_button_p;
    button* select_next_button_p;
    button* delete_selected_button_p;
    button* clear_envelope_button_p;
    button* test_print_button_p;
    QGroupBox* type_box_p;
    QVBoxLayout* type_box_layout_p;
    QRadioButton* text_button_p;
    QRadioButton* code39_button_p;
    QLabel* x_label_p;
    QSlider* x_slider_p;
    QLabel* y_label_p;
    QSlider* y_slider_p;
    QLabel* h_label_p;
    QSlider* h_slider_p;
	  button* new_field_button_p;
	  button* done_button_p;
    envelope* envelope_p;
    envelope_picture* envelope_picture_p;
	  QGridLayout* layout_p;
    QTimer* timer_p;
    QImage* envelope_image_p;
};
#endif
