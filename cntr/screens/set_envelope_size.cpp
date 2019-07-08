#include <iostream>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include "centre.hpp"
#include "set_envelope_size.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"
#include <QTimer>

using namespace std;

set_envelope_size::set_envelope_size(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  batch_mode_driver_p = batch_mode_driver_p_s;
  message_p = new QLabel("Set envelope size.  (Millimeters)");
  message_p -> setMaximumHeight(30);
  back_button_p = new button("Back");
  no_envelope_button_p = new button("No envelope\nprinting");
  width_label_p = new QLabel("Width");
  width_slider_p = new QSlider(Qt::Horizontal);
  width_slider_p -> setMinimum(0);
  width_slider_p -> setMaximum(150);
  width_slider_p -> setValue(80);
  height_label_p = new QLabel("Height");
  height_slider_p = new QSlider(Qt::Horizontal);
  height_slider_p -> setMinimum(0);
  height_slider_p -> setMaximum(250);
  height_slider_p -> setValue(150);
  ok_button_p = new button("OK");
  layout_p = new QGridLayout;
  layout_p->addWidget(no_envelope_button_p, 0, 0);
  layout_p->addWidget(back_button_p, 0, 2);
  layout_p->addWidget(message_p, 1, 0, 1, 2);
  layout_p->addWidget(width_label_p, 2, 0);
  layout_p->addWidget(width_slider_p, 2, 1, 1, 2);
  layout_p->addWidget(height_label_p, 3, 0);
  layout_p->addWidget(height_slider_p, 3, 1, 1, 2);
  layout_p->addWidget(ok_button_p, 4, 2);
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(no_envelope_button_p, SIGNAL(clicked()), this, SLOT(no_envelope_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  setLayout(layout_p);
  
  timer_p = new QTimer;
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  timer_p -> start(10);
}

set_envelope_size::~set_envelope_size()
{
  timer_p -> stop();
  delete timer_p;
  timer_p = 0;
}

void set_envelope_size::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void set_envelope_size::no_envelope_button_clicked()
{
  centre_p->add_waiting_screen(33);//ss_batch
  centre_p->screen_done = true;
}

void set_envelope_size::ok_button_clicked()
{
  batch_mode_driver_p -> envelope_p -> set_size(width_slider_p->value(), height_slider_p->value());
  centre_p -> add_waiting_screen(36);//select_field_data_source
  centre_p -> screen_done = true;
}

void set_envelope_size::run()
{
  QString width_slider_string = QString("Width = %1").arg(width_slider_p->value());
  width_label_p -> setText(width_slider_string);
  QString height_slider_string = QString("Height = %1").arg(height_slider_p->value());
  height_label_p -> setText(height_slider_string);
}
  

