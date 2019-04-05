#include <iostream>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include "centre.hpp"
#include "set_envelope_size.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

set_envelope_size::set_envelope_size(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  batch_mode_driver_p = batch_mode_driver_p_s;
  message_p = new QLabel("Set envelope size.  (Millimeters)");
  back_button_p = new button("Back");
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
  layout_p->addWidget(message_p, 0, 0, 2, 1);
  layout_p->addWidget(back_button_p, 0, 2);
  layout_p->addWidget(width_label_p, 1, 0);
  layout_p->addWidget(width_slider_p, 1, 1, 1, 2);
  layout_p->addWidget(height_label_p, 2, 0);
  layout_p->addWidget(height_slider_p, 2, 1, 1, 2);
  layout_p->addWidget(ok_button_p, 3, 3);
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  setLayout(layout_p);
}

void set_envelope_size::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void set_envelope_size::ok_button_clicked()
{
  batch_mode_driver_p -> envelope_p -> set_size(width_slider_p->value(), height_slider_p->value());
  centre_p -> add_waiting_screen(34);//select_envelope_field
  centre_p -> screen_done = true;
}



