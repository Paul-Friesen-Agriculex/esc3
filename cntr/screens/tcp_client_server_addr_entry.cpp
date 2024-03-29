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
#include <QTcpSocket>
#include "centre.hpp"
#include "tcp_client_server_addr_entry.hpp"
#include "button.hpp"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug> //2022_01_20

using namespace std;

tcp_client_server_addr_entry::tcp_client_server_addr_entry(centre*set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  back_button_p = new button("Back");
  connect_button_p = new button("Connect");
  addr1_p = new QSpinBox;
  addr2_p = new QSpinBox;
  addr3_p = new QSpinBox;
  addr4_p = new QSpinBox;
  message_p = new QLabel;
  help_button_p = new button("Help");
  ok_button_p = new button("OK");
  diagnostics_button_p = new button("Diag.");   //2022_01_20

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,3);
  main_layout_p->addWidget(addr1_p, 1, 0);
  main_layout_p->addWidget(addr2_p, 1, 1);
  main_layout_p->addWidget(addr3_p, 1, 2);
  main_layout_p->addWidget(addr4_p, 1, 3);
  main_layout_p->addWidget(connect_button_p, 2, 0);
  main_layout_p->addWidget(diagnostics_button_p, 2, 2); //2022_01_20
  main_layout_p->addWidget(message_p, 3, 0, 1, 4);
//  main_layout_p->addWidget(help_button_p, 4, 0);
  main_layout_p->addWidget(ok_button_p, 4, 3);
  
  addr1_p->setStyleSheet("QSpinBox { width: 50px}" );
  addr2_p->setStyleSheet("QSpinBox { width: 50px}" );
  addr3_p->setStyleSheet("QSpinBox { width: 50px}" );
  addr4_p->setStyleSheet("QSpinBox { width: 50px}" );
  
  addr1_p->setRange(0,255);
  addr2_p->setRange(0,255);
  addr3_p->setRange(0,255);
  addr4_p->setRange(0,255);
  
  addr2_p->setPrefix(".");
  addr3_p->setPrefix(".");
  addr4_p->setPrefix(".");
  
  addr1_p->setValue(192);
  addr2_p->setValue(168);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(connect_button_p, SIGNAL(clicked()), this, SLOT(connect_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(centre_p, SIGNAL(tcp_connection_detected_signal()), this, SLOT(connection_detected()));
  connect(diagnostics_button_p, SIGNAL(clicked()), this, SLOT(diagnostic_button_clicked()));   //2022_01_20
  
  message_p->setText("Enter the IP address of the host and touch \"connect\".");
  ok_button_p->setEnabled(false);
  
  default_ip_addr = "192.168.1.2";
  retrieve_connection_ip();
}

void tcp_client_server_addr_entry::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void tcp_client_server_addr_entry::connect_button_clicked()
{
  QString addr_string = QString::number(addr1_p->value()) + "." + QString::number(addr2_p->value()) + "." + QString::number(addr3_p->value()) + "." + QString::number(addr4_p->value());
//  cout<<"addr_string = "<<addr_string.toStdString()<<endl;
  centre_p->tcp_socket_p->connectToHost(addr_string, 50000);
  message_p->setText("Attempting connection");
  
  //2022_01_19---------------------------------------------TEST~~~//
  qDebug()<<centre_p->tcp_socket_p->state();  //
}
  
void tcp_client_server_addr_entry::ok_button_clicked()
{
//  centre_p->add_waiting_screen(5);//totalize
  centre_p->screen_done = true;
}
  
void tcp_client_server_addr_entry::help_button_clicked()
{
}

void tcp_client_server_addr_entry::connection_detected()
{
  message_p->setText("Connection established");
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = true;
  centre_p->tcp_link_established = true;
  ok_button_p->setEnabled(true);
}

void tcp_client_server_addr_entry::retrieve_connection_ip()
{
  QString full_ipv4;
  QStringList ipv4_segments;
  
  int addr1, addr2, addr3, addr4;
  
  QList<QHostAddress> list = QNetworkInterface::allAddresses();
  
  for(int i=0; i<list.count(); ++i)
  {
    if(!list[i].isLoopback())
    {
      if(list[i].protocol() == QAbstractSocket:: IPv4Protocol)
      {
        full_ipv4 = list[i].toString();
      }
    }
  }
  
  if(full_ipv4.isEmpty())
  {
    full_ipv4 = default_ip_addr;
  }
  
  ipv4_segments = full_ipv4.split(".");
  
  addr1 = ipv4_segments[0].toInt();
  addr2 = ipv4_segments[1].toInt();
  addr3 = ipv4_segments[2].toInt();
  addr4 = ipv4_segments[3].toInt();
  
  addr1_p->setValue(addr1);
  addr2_p->setValue(addr2);
  addr3_p->setValue(addr3);
  addr4_p->setValue(addr4);  
}

//============================================================================================//

void tcp_client_server_addr_entry::diagnostic_button_clicked()
{
  cout<<"~~diagnostic_button_clicked()~~"<<endl;  //OMIT~~~//
  
  QString addr_string = QString::number(addr1_p->value()) + "." 
                        + QString::number(addr2_p->value()) + "." 
                        + QString::number(addr3_p->value()) 
                        + "." + QString::number(addr4_p->value());
  
  qDebug() << addr_string;
  qDebug() << centre_p->tcp_socket_p->state();
  
  //centre_p->tcp_socket_p
  

}

//============================================================================================//
