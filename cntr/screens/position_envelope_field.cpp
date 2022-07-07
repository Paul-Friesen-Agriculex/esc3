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

#include <iostream>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QPainter>
#include "centre.hpp"
#include "position_envelope_field.hpp"
#include "button.hpp"
#include "envelope.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

envelope_picture::envelope_picture(QImage* qimage_s)
:QWidget(0)
{
  envelope_qimage_p = qimage_s;
}

void envelope_picture::set_current_position(int x, int y)
{
  current_x = x;
  current_y = y;
}

void envelope_picture::set_start_displacement(int pixels_down)
{
  start_displacement = pixels_down;
}

void envelope_picture::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setBrush(QBrush(Qt::white));
  painter.setPen(Qt::white);
  painter.drawRect(rect());
  //image printing actually starts 10mm down from top of envelope
  //QImage height has been reduced 10mm to account for this.
  //When we display the QImage here, we wish to start the display 10mm down, so that this image matches the image as it will print.
  QRect draw_rect = QRect(0, start_displacement, width(), height()-start_displacement);
  painter.drawImage(draw_rect, *envelope_qimage_p, envelope_qimage_p->rect() );
}
  
position_envelope_field::position_envelope_field(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  
  cout<<"start position_envelope_field::position_envelope_field \n";
  
  batch_mode_driver_p = batch_mode_driver_p_s;
  message_p = new QLabel("Position the field\n(or click \"new field\")");
  back_button_p = new button("Back");
  select_previous_button_p = new button("Select\nprevious field");
  select_next_button_p = new button("Select\nnext field");
  delete_selected_button_p = new button("Delete selected");
  clear_envelope_button_p = new button("Clear envelope");
  test_print_button_p = new button("Test print");
  type_box_p = new QGroupBox("Field Type");
  text_button_p = new QRadioButton("Text");
  code39_button_p = new QRadioButton("Code 39");
  type_box_layout_p = new QVBoxLayout;
  type_box_layout_p -> addWidget(text_button_p);
  type_box_layout_p -> addWidget(code39_button_p);
  type_box_p -> setLayout(type_box_layout_p);
  x_label_p = new QLabel("X ");
  x_slider_p = new QSlider(Qt::Horizontal);
  x_slider_p -> setMinimum(0);
//  x_slider_p -> setMaximum(batch_mode_driver_p->envelope_p->get_width());
  y_label_p = new QLabel("Y ");
  y_slider_p = new QSlider(Qt::Horizontal);
  y_slider_p -> setMinimum(15);
//  cout<<"y_slider_p setMaximum "<<batch_mode_driver_p->envelope_p->get_height()<<endl;
//  y_slider_p -> setMaximum(batch_mode_driver_p->envelope_p->get_height());
  h_label_p = new QLabel("Height ");
  h_slider_p = new QSlider(Qt::Horizontal);
  h_slider_p -> setMinimum(0);
//  cout<<"h_slider_p setMaximum "<<batch_mode_driver_p->envelope_p->get_height()/5<<endl;
//  h_slider_p -> setMaximum(batch_mode_driver_p->envelope_p->get_height()/5);
  envelope_p = batch_mode_driver_p -> envelope_p;
  envelope_picture_p = new envelope_picture(envelope_p->image_p);
  
  //get_width, get_height are in mm.  Following lines set screen display size in screen pixels
  envelope_picture_p -> setMinimumSize(envelope_p->get_width()*2, envelope_p->get_height()*2);
  envelope_picture_p -> setMaximumSize(envelope_p->get_width()*2, envelope_p->get_height()*2);
  envelope_picture_p -> set_start_displacement(10*2);//start drawing image 10mm down
  
  new_field_button_p = new button("New field");
  done_button_p = new button("Done");
  layout_p = new QGridLayout;
  layout_p->addWidget(message_p, 0, 0, 1, 2);
  layout_p->addWidget(back_button_p, 0, 3);
  layout_p->addWidget(select_previous_button_p, 1, 1);
  layout_p->addWidget(select_next_button_p, 1, 2);
  layout_p->addWidget(delete_selected_button_p, 2, 1);
  layout_p->addWidget(clear_envelope_button_p, 2, 2);
  layout_p->addWidget(test_print_button_p, 3, 1);
  layout_p->addWidget(type_box_p, 1, 0, 3, 1);
  type_box_p->setBackgroundRole(QPalette::AlternateBase);
  layout_p->addWidget(x_label_p, 4, 0);
  layout_p->addWidget(x_slider_p, 4, 1, 1, 2);
  layout_p->addWidget(y_label_p, 5, 0);
  layout_p->addWidget(y_slider_p, 5, 1, 1, 2);
  layout_p->addWidget(h_label_p, 6, 0);
  layout_p->addWidget(h_slider_p, 6, 1, 1, 2);
  layout_p->addWidget(envelope_picture_p, 1, 3, 4, 2);
  layout_p->addWidget(new_field_button_p, 6, 3);
  layout_p->addWidget(done_button_p, 6, 4);
  setLayout(layout_p);

  cout<<"p1\n";

  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(select_previous_button_p, SIGNAL(clicked()), this, SLOT(select_previous_button_clicked()));
  connect(select_next_button_p, SIGNAL(clicked()), this, SLOT(select_next_button_clicked()));
  connect(delete_selected_button_p, SIGNAL(clicked()), this, SLOT(delete_selected_clicked()));
  connect(clear_envelope_button_p, SIGNAL(clicked()), this, SLOT(clear_envelope_clicked()));
  connect(test_print_button_p, SIGNAL(clicked()), this, SLOT(test_print_button_clicked()));
  connect(new_field_button_p, SIGNAL(clicked()), this, SLOT(new_field_button_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(x_slider_p, SIGNAL(valueChanged(int)), this, SLOT(set_x(int)));
  connect(y_slider_p, SIGNAL(valueChanged(int)), this, SLOT(set_y(int)));
  connect(h_slider_p, SIGNAL(valueChanged(int)), this, SLOT(set_h(int)));
  connect(text_button_p, SIGNAL(toggled(bool)), this, SLOT(text_button_toggled(bool)));
  connect(code39_button_p, SIGNAL(toggled(bool)), this, SLOT(code39_button_toggled(bool)));

  cout<<"p2\n";

  
//  envelope_p -> enter_field();
  x_slider_p -> setValue(envelope_p->get_selected_x());
  y_slider_p -> setValue(envelope_p->get_selected_y());
  h_slider_p -> setValue(envelope_p->get_selected_h());
  if(envelope_p->get_selected_type() == Ubuntu_mono) text_button_p->setChecked(true);
  if(envelope_p->get_selected_type() == code_39) code39_button_p->setChecked(true);
  
  envelope_image_p = envelope_p->image_p;
  timer_p = new QTimer;
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  timer_p -> start(10);

  cout<<"end position_envelope_field::position_envelope_field \n";


}

position_envelope_field::~position_envelope_field()
{
  timer_p -> stop();
  delete timer_p;
  timer_p = 0;
}

void position_envelope_field::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void position_envelope_field::select_previous_button_clicked()
{
  
  envelope_p -> select_previous_field();

  x_slider_p -> setValue(envelope_p->get_selected_x());
  y_slider_p -> setValue(envelope_p->get_selected_y());
  h_slider_p -> setValue(envelope_p->get_selected_h());
  if(envelope_p->get_selected_type() == Ubuntu_mono) text_button_p->setChecked(true);
  if(envelope_p->get_selected_type() == code_39) code39_button_p->setChecked(true);


}

void position_envelope_field::select_next_button_clicked()
{
  
  envelope_p -> select_next_field();

  x_slider_p -> setValue(envelope_p->get_selected_x());
  y_slider_p -> setValue(envelope_p->get_selected_y());
  h_slider_p -> setValue(envelope_p->get_selected_h());
  if(envelope_p->get_selected_type() == Ubuntu_mono) text_button_p->setChecked(true);
  if(envelope_p->get_selected_type() == code_39) code39_button_p->setChecked(true);
  
}

void position_envelope_field::delete_selected_clicked()
{
  envelope_p->delete_field();
}

void position_envelope_field::clear_envelope_clicked()
{
  envelope_p->clear_fields();
}

void position_envelope_field::test_print_button_clicked()
{
  envelope_p -> print();
}

void position_envelope_field::new_field_button_clicked()
{
  centre_p->add_waiting_screen(36);//select_field_data_source
  centre_p->screen_done = true;
}

void position_envelope_field::done_button_clicked()
{
  batch_mode_driver_p -> print_envelope = true;
  centre_p -> add_waiting_screen(33);//ss_batch
  centre_p -> add_waiting_screen(31);//ss_options
  centre_p -> screen_done = true;
}

void position_envelope_field::text_button_toggled(bool val)
{
  if(val)
  {
    cout<<"change to Ubuntu_mono\n";
    envelope_p->change_selected_type(Ubuntu_mono);
  }
}

void position_envelope_field::code39_button_toggled(bool val)
{
  if(val)
  {
    cout<<"change to code_39\n";
    envelope_p->change_selected_type(code_39);
  }
}

void position_envelope_field::set_x(int x_s)
{
  envelope_p->move_selected_x(x_s);
}

void position_envelope_field::set_y(int y_s)
{
  envelope_p->move_selected_y(y_s);
}

void position_envelope_field::set_h(int h_s)
{
  envelope_p->move_selected_h(h_s);
}

void position_envelope_field::run()
{

  envelope_picture_p -> update();
  QString x_slider_string = QString("X = %1").arg(envelope_p->get_selected_x());
  x_label_p -> setText(x_slider_string);
  QString y_slider_string = QString("Y = %1").arg(envelope_p->get_selected_y());
  y_label_p -> setText(y_slider_string);
  QString h_slider_string = QString("Height = %1").arg(envelope_p->get_selected_h());
  h_label_p -> setText(h_slider_string);

}
