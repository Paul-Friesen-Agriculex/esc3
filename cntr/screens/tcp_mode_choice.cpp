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
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include "centre.hpp"
#include "tcp_mode_choice.hpp"
#include "button.hpp"

using namespace std;

tcp_mode_choice::tcp_mode_choice(centre*set_centre_p)
:screen(set_centre_p)
{
//  cout<<"tcp_mode_choice::tcp_mode_choice 1\n";
  
  centre_p=set_centre_p;
  
  back_button_p = new button("Back");
  function_as_server_button_p = new button("ESC-3 to function as server");
  function_as_client_button_p = new button("ESC-3 to function as client");
  help_button_p = new button("Help");

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(function_as_server_button_p, 1, 0);
  main_layout_p->addWidget(function_as_client_button_p, 2, 0);
  main_layout_p->addWidget(help_button_p, 4, 0);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(function_as_server_button_p, SIGNAL(clicked()), this, SLOT(function_as_server()));
  connect(function_as_client_button_p, SIGNAL(clicked()), this, SLOT(function_as_client()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
}

void tcp_mode_choice::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void tcp_mode_choice::function_as_server()
{
  centre_p->add_waiting_screen(41);//tcp_server_addr_choice
  centre_p->screen_done = true;
}

void tcp_mode_choice::function_as_client()
{
  centre_p->add_waiting_screen(42);//tcp_client_server_addr_entry
  centre_p->screen_done = true;
}

void tcp_mode_choice::help_button_clicked()
{
}
