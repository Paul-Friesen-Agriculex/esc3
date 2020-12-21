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
