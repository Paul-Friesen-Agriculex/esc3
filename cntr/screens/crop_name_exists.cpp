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
  start_screen_p = new button("Return to start screen");
  layout_p = new QVBoxLayout;
  layout_p->addWidget(message_p);
  layout_p->addWidget(ok_button_p);
  layout_p->addWidget(start_screen_p);
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(start_screen_p, SIGNAL(clicked()), this, SLOT(start_screen_clicked()));
  setLayout(layout_p);
}

void crop_name_exists::ok_button_clicked()
{
  centre_p -> add_waiting_screen(1);//crop choice
  centre_p -> screen_done = true;
}

void crop_name_exists::start_screen_clicked()
{
  centre_p -> add_waiting_screen(0);//start screen
  centre_p -> screen_done = true;
}



