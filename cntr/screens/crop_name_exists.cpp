#include <iostream>
#include <QVBoxLayout>
#include <QLabel>
#include "centre.hpp"
#include "crop_name_exists.hpp"
#include "button.hpp"

using namespace std;

crop_name_exists::crop_name_exists(centre* set_centre_p)
:screen(set_centre_p)
{
  message_p = new QLabel("That crop already exists.");
  ok_button_p = new button("OK");
  layout_p = new QVBoxLayout;
  layout_p->addWidget(message_p);
  layout_p->addWidget(ok_button_p);
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  setLayout(layout_p);
}

void crop_name_exists::ok_button_clicked()
{
  centre_p -> add_waiting_screen(4);
  centre_p -> screen_done = true;
}



