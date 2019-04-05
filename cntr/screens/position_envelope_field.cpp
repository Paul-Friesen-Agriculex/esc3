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

void envelope_picture::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.drawImage(rect(), *envelope_qimage_p, envelope_qimage_p->rect() );
}
  
position_envelope_field::position_envelope_field(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  
  cout<<"start position_envelope_field::position_envelope_field \n";
  
  batch_mode_driver_p = batch_mode_driver_p_s;
  message_p = new QLabel("Position the field");
  back_button_p = new button("Back");
  type_box_p = new QGroupBox("Field Type");
  text_button_p = new QRadioButton("Text");
  code39_button_p = new QRadioButton("Code 39");
  type_box_layout_p = new QVBoxLayout;
  type_box_layout_p -> addWidget(text_button_p);
  type_box_layout_p -> addWidget(code39_button_p);
  type_box_p -> setLayout(type_box_layout_p);
  x_label_p = new QLabel("X");
  x_slider_p = new QSlider(Qt::Horizontal);
  x_slider_p -> setMinimum(0);
  x_slider_p -> setMaximum(batch_mode_driver_p->envelope_p->get_width());
  y_label_p = new QLabel("Y");
  y_slider_p = new QSlider(Qt::Horizontal);
  y_slider_p -> setMinimum(0);
  cout<<"y_slider_p setMaximum "<<batch_mode_driver_p->envelope_p->get_height()<<endl;
  y_slider_p -> setMaximum(batch_mode_driver_p->envelope_p->get_height());
  h_label_p = new QLabel("Height");
  h_slider_p = new QSlider(Qt::Horizontal);
  h_slider_p -> setMinimum(0);
  cout<<"h_slider_p setMaximum "<<batch_mode_driver_p->envelope_p->get_height()/5<<endl;
  h_slider_p -> setMaximum(batch_mode_driver_p->envelope_p->get_height()/5);
  envelope_p = batch_mode_driver_p -> envelope_p;
  envelope_picture_p = new envelope_picture(envelope_p->image_p);
  envelope_picture_p -> setMinimumSize(envelope_p->get_width()*2, envelope_p->get_height()*2);
  envelope_picture_p -> setMaximumSize(envelope_p->get_width()*2, envelope_p->get_height()*2);
  next_field_button_p = new button("Next field");
  done_button_p = new button("Done");
  layout_p = new QGridLayout;
  layout_p->addWidget(message_p, 0, 0, 1, 2);
  layout_p->addWidget(back_button_p, 0, 2);
  layout_p->addWidget(type_box_p, 1, 0, 1, 2);
  layout_p->addWidget(x_label_p, 2, 0);
  layout_p->addWidget(x_slider_p, 2, 1);
  layout_p->addWidget(y_label_p, 3, 0);
  layout_p->addWidget(y_slider_p, 3, 1);
  layout_p->addWidget(h_label_p, 4, 0);
  layout_p->addWidget(h_slider_p, 4, 1);
  layout_p->addWidget(envelope_picture_p, 1, 2, 2, 2);
  layout_p->addWidget(next_field_button_p, 3, 2);
  layout_p->addWidget(done_button_p, 3, 3);
  setLayout(layout_p);

  cout<<"p1\n";

  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(next_field_button_p, SIGNAL(clicked()), this, SLOT(next_field_button_clicked()));
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

void position_envelope_field::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void position_envelope_field::next_field_button_clicked()
{
  centre_p->add_waiting_screen(34);//select_envelope_field
  centre_p->screen_done = true;
}

void position_envelope_field::done_button_clicked()
{
  batch_mode_driver_p -> print_envelope = true;
  centre_p -> add_waiting_screen(33);//ss_batch
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
}
