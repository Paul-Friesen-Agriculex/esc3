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



