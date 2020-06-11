#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include "centre.hpp"
#include "tcp_client_server_addr_entry.hpp"
#include "button.hpp"

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

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,3);
  main_layout_p->addWidget(addr1_p, 1, 0);
  main_layout_p->addWidget(addr2_p, 1, 1);
  main_layout_p->addWidget(addr3_p, 1, 2);
  main_layout_p->addWidget(addr4_p, 1, 3);
  main_layout_p->addWidget(connect_button_p, 2, 0);
  main_layout_p->addWidget(message_p, 3, 0, 1, 4);
  main_layout_p->addWidget(help_button_p, 4, 0);
  main_layout_p->addWidget(ok_button_p, 4, 3);
  
  addr1_p->setRange(0,255);
  addr2_p->setRange(0,255);
  addr3_p->setRange(0,255);
  addr4_p->setRange(0,255);
  
  addr2_p->setPrefix(".");
  addr3_p->setPrefix(".");
  addr4_p->setPrefix(".");
  
  addr1_p->setValue(192);
  addr1_p->setValue(168);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(connect_button_p, SIGNAL(clicked()), this, SLOT(connect_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(centre_p->tcp_socket_p, SIGNAL(connected()), this, SLOT(connection_detected()));
}

void tcp_client_server_addr_entry::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}
















void tcp_client_server_addr_entry::net_100_clicked()
{
  centre_p->network = 1;
  if(centre_p->tcp_server_p->listen(QHostAddress("192.168.100.1"), 50000) == false)
  {
    message_p->setText(QString("listen failure.   ") . append(tcp_server_p->errorString());
    return;
  }
  message_p->setText("Listening for connection to address 192.168.100.1");
}    

void tcp_client_server_addr_entry::net_200_clicked()
{
  centre_p->network = 1;
  if(centre_p->tcp_server_p->listen(QHostAddress("192.168.200.1"), 50000) == false)
  {
    message_p->setText(QString("listen failure.   ") . append(tcp_server_p->errorString());
    return;
  }
  message_p->setText("Listening for connection to address 192.168.200.1");
}    

void tcp_client_server_addr_entry::connection_detected()
{
  centre_p->tcp_socket_p = centre_p->tcp_server_p->nextPendingConnection();
  if(centre_p->network == 1)
  {
    message_p->setText("Connected using address 192.168.100.1");
  }
  if(centre_p->network == 2)
  {
    message_p->setText("Connected using address 192.168.200.1");
  }
}
  
void tcp_client_server_addr_entry::ok_button_clicked()  
  centre_p->add_waiting_screen(5);//totalize
  centre_p->screen_done = true;
}

void tcp_client_server_addr_entry::help_button_clicked()
{
}

