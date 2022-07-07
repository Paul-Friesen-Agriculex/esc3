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
#include <QTcpServer>
#include <QTcpSocket>
#include "centre.hpp"
#include "tcp_server_addr_choice.hpp"
#include "button.hpp"

using namespace std;

tcp_server_addr_choice::tcp_server_addr_choice(centre*set_centre_p)
:screen(set_centre_p)
{
  
  cout<<"tcp_server_address_choice::tcp_server_address_choice 1\n";
  
  centre_p=set_centre_p;
  
  back_button_p = new button("Back");
  net_100_button_p = new button("Use 192.168.100.1");
  net_200_button_p = new button("Use 192.168.200.1");
  message_p = new QLabel("Select the TCP server address.\nConnecting device should connect to this address.");
  help_button_p = new button("Help");
  ok_button_p = new button("OK");

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(net_100_button_p, 1, 0);
  main_layout_p->addWidget(net_200_button_p, 2, 0);
  main_layout_p->addWidget(message_p, 3, 0, 1, 2);
  main_layout_p->addWidget(help_button_p, 4, 0);
  main_layout_p->addWidget(ok_button_p, 4, 1);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(net_100_button_p, SIGNAL(clicked()), this, SLOT(net_100_clicked()));
  connect(net_200_button_p, SIGNAL(clicked()), this, SLOT(net_200_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
//  connect(centre_p->tcp_server_p, SIGNAL(newConnection()), this, SLOT(connection_detected()));
}

void tcp_server_addr_choice::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void tcp_server_addr_choice::net_100_clicked()
{
  QString message_string = centre_p->choose_tcp_network(1);
  if(message_string=="")//success
  {
    message_p->setText("Listening for connection to address 192.168.100.1");
    return;
  }
  message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void tcp_server_addr_choice::net_200_clicked()
{
  QString message_string = centre_p->choose_tcp_network(2);
  if(message_string=="")//success
  {
    message_p->setText("Listening for connection to address 192.168.200.1");
    return;
  }
  message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void tcp_server_addr_choice::connection_detected()
{
  cout<<"connection_detected\n";

  if(centre_p->network == 1)
  {
    message_p->setText("Connected using address 192.168.100.1");
  }
  if(centre_p->network == 2)
  {
    message_p->setText("Connected using address 192.168.200.1");
  }
  
//  centre_p->tcp_write("test\n");
  
}
  
void tcp_server_addr_choice::ok_button_clicked() 
{ 
//  centre_p->add_waiting_screen(5);//totalize
  centre_p->screen_done = true;
}

void tcp_server_addr_choice::help_button_clicked()
{
}

