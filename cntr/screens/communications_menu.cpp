#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QFileInfo>
#include "centre.hpp"
#include "communications_menu.hpp"
#include "button.hpp"

using namespace std;

communications_menu::communications_menu(centre*set_centre_p)
:screen(set_centre_p)
{
//  cout<<"communications_menu::communications_menu 1\n";
  
  centre_p=set_centre_p;
  
  back_button_p = new button("Back");
  communicate_by_keyboard_cable_button_p = new button("Communicate by Keyboard Cable");
  function_as_server_1_button_p = new button("Function as TCP Server Using 192.168.100.1");
  function_as_server_2_button_p = new button("Function as TCP Server Using 192.168.200.1");
  function_as_client_button_p = new button("Function as TCP Client");
  help_button_p = new button("Help");
  ok_button_p = new button("OK");
  message_p = new QLabel("Choose communication method");

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(communicate_by_keyboard_cable_button_p,1,0);
  main_layout_p->addWidget(function_as_server_1_button_p, 2, 0);
  main_layout_p->addWidget(function_as_server_2_button_p, 3, 0);
  main_layout_p->addWidget(function_as_client_button_p, 4, 0);
  main_layout_p->addWidget(help_button_p, 6, 0);
  main_layout_p->addWidget(message_p, 1, 1, 3, 1);
  main_layout_p->addWidget(ok_button_p, 6, 1);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(communicate_by_keyboard_cable_button_p, SIGNAL(clicked()), this, SLOT(communicate_by_keyboard_cable_button_clicked()));
  connect(function_as_server_1_button_p, SIGNAL(clicked()), this, SLOT(function_as_server_1_clicked()));
  connect(function_as_server_2_button_p, SIGNAL(clicked()), this, SLOT(function_as_server_2_clicked()));
  connect(function_as_client_button_p, SIGNAL(clicked()), this, SLOT(function_as_client_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(centre_p, SIGNAL(tcp_connection_detected_signal()), this, SLOT(connection_detected()));
  
  ok_button_p->setEnabled(false);
}

void communications_menu::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void communications_menu::communicate_by_keyboard_cable_button_clicked()
{
  if(QFileInfo::exists("/dev/usb2serial"))
  {
    message_p->setText("Ready to communicate.");
    centre_p->communicate_by_keyboard_cable = true;
    centre_p->communicate_by_tcp = false;
    ok_button_p->setEnabled(true);
  }
  else
  {
    message_p->setText("No USB communications cable detected.\nThis requires a special cable from Agriculex.\nIf it is attached, be sure you are using the right end.");
  }
  
  /*
  int filedesc = open("/dev/usb2serial", O_WRONLY);
  if(filedesc < 0)
  {
    cout<<"serial NOT available"<<endl;
    serialusb_connected = false;
  }
  else
  {
	cout<<"serial available"<<endl;
	serialusb_connected = true;
	if(write(filedesc,"", 0) != 0){
    int system_int; //TEST~~~
    system_int = system("stty -F /dev/usb2serial cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
    cout<<system_int<<endl; //TEST~~~
    
	  if(write(filedesc,"", 0) != 0)
	  {
		cout<<"check connection"<<endl;
	  }
	  else
      {
	    cout<<"initialization sent"<<endl;
      }
    }
    else
    {
	  cout<<"initialization sent"<<endl;
    }
  }
  */

}  

void communications_menu::function_as_server_1_clicked()
{
  QString message_string = centre_p->choose_tcp_network(1);
  if(message_string=="")//success
  {
    message_p->setText("Listening for connection to address 192.168.100.1");
    return;
  }
  message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void communications_menu::function_as_server_2_clicked()
{
  QString message_string = centre_p->choose_tcp_network(2);
  if(message_string=="")//success
  {
    message_p->setText("Listening for connection to address 192.168.200.1");
    return;
  }
  message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void communications_menu::function_as_client_clicked()
{
  centre_p->add_waiting_screen(42);//tcp_client_server_addr_entry
  centre_p->screen_done = true;
}

void communications_menu::help_button_clicked()
{
}

void communications_menu::connection_detected()
{
//  cout<<"connection_detected\n";

  if(centre_p->network == 1)
  {
    message_p->setText("Connected using address 192.168.100.1");
  }
  if(centre_p->network == 2)
  {
    message_p->setText("Connected using address 192.168.200.1");
  }
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = true;
  centre_p->tcp_link_established = true;
  ok_button_p->setEnabled(true);
  
//  centre_p->tcp_write("test\n");
  
}
  
void communications_menu::ok_button_clicked() 
{ 
//  centre_p->add_waiting_screen(28);//macro_screen
  centre_p->screen_done = true;//go to waiting screen
}


