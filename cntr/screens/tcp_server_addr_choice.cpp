#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include "centre.hpp"
#include "tcp_server_addr_choice.hpp"
#include "button.hpp"

using namespace std;

tcp_server_addr_choice::tcp_server_addr_choice(centre*set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  back_button_p = new button("Back");
  net_100_button_p = new button("Use 192.168.100.1");
  net_200_button_p = new button("Use 192.168.200.1");
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
  connect(centre_p->tcp_server_p, SIGNAL(newConnection()), this, SLOT(connection_detected()));
}

void tcp_server_addr_choice::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void tcp_server_addr_choice::net_100_clicked()
{
  centre_p->network = 1;
  if(centre_p->tcp_server_p->listen(QHostAddress("192.168.100.1"), 50000) == false)
  {
    message_p->setText(QString("listen failure.   ") . append(tcp_server_p->errorString());
    return;
  }
  message_p->setText("Listening for connection to address 192.168.100.1");
}    

void tcp_server_addr_choice::net_200_clicked()
{
  centre_p->network = 1;
  if(centre_p->tcp_server_p->listen(QHostAddress("192.168.200.1"), 50000) == false)
  {
    message_p->setText(QString("listen failure.   ") . append(tcp_server_p->errorString());
    return;
  }
  message_p->setText("Listening for connection to address 192.168.200.1");
}    

void tcp_server_addr_choice::connection_detected()
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
  
void tcp_server_addr_choice::ok_button_clicked()  
  centre_p->add_waiting_screen(5);//totalize
  centre_p->screen_done = true;
}

void tcp_server_addr_choice::help_button_clicked()
{
}

