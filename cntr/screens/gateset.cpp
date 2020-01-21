#include <iostream>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include "centre.hpp"
#include "gateset.hpp"
#include "button.hpp"

using namespace std;

gateset::gateset(centre* set_centre_p)
:screen(set_centre_p)
{
  message_p = new QLabel("Adjust the gate setting.");
  spinbox_p = new QSpinBox;
  spinbox_p -> setMinimum(0);
  spinbox_p -> setMaximum(22);
  spinbox_p -> setValue(centre_p->crops[0].gate_setting);
  ok_button_p = new button("OK");
  layout_p = new QGridLayout;
  layout_p->addWidget(message_p, 0, 0);
  layout_p->addWidget(spinbox_p, 0, 1);
//  spinbox_p->setMaximumWidth(200);
  layout_p->addWidget(ok_button_p,1,0,1,2);
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  setLayout(layout_p);
}

void gateset::ok_button_clicked()
{
//  centre_p -> add_waiting_screen(4);
  centre_p -> crops[0].gate_setting = spinbox_p->value();
  centre_p -> screen_done = true;
}



