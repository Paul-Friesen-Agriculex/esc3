#include <QDebug>
#include <iostream>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>

#include "centre.hpp"
#include "print_settings.hpp"

using namespace std;

print_settings::print_settings(centre*set_centre_p)
:screen(set_centre_p)
{
    brother_usb_button_p = new QRadioButton;
	  rnmark_ethernet_button_p = new QRadioButton;
	  rnmark_serial_button_p = new QRadioButton;
	  print_settings_back_button_p = new QPushButton("Back", this);
    
    brother_usb_button_p->setText("Brother USB");
    rnmark_ethernet_button_p->setText("RNMark Ethernet");
    rnmark_serial_button_p->setText("RNMark Serial");
    
    top_box_p = new QGroupBox;
    bottom_navigation_box_p = new QGroupBox;
    top_layout_p = new QGridLayout;
    bottom_navigation_layout_p = new QGridLayout;
    main_layout_p = new QVBoxLayout;
    
    top_layout_p->addWidget(brother_usb_button_p,0,0);
    top_layout_p->addWidget(rnmark_ethernet_button_p,1,0);
    top_layout_p->addWidget(rnmark_serial_button_p,2,0);
    top_box_p->setLayout(top_layout_p);
    
    bottom_navigation_layout_p->addWidget(print_settings_back_button_p,0,1);
    bottom_navigation_box_p->setLayout(bottom_navigation_layout_p);
    
    main_layout_p->addWidget(top_box_p);
	  main_layout_p->addWidget(bottom_navigation_box_p);
	  setLayout(main_layout_p);
    
    brother_usb_button_p->setAutoExclusive(false);
    rnmark_ethernet_button_p->setAutoExclusive(false);
    rnmark_serial_button_p->setAutoExclusive(false);

    connect(brother_usb_button_p, SIGNAL(clicked()), this, SLOT(toggle_brother_usb_printing()));
    connect(rnmark_ethernet_button_p, SIGNAL(clicked()), this, SLOT(toggle_rnmark_ethernet_printing()));
    connect(rnmark_serial_button_p, SIGNAL(clicked()), this, SLOT(toggle_rnmark_serial_printing()));
    connect(print_settings_back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
    
    check_printing_selections();
}
  
void print_settings::check_printing_selections()
{
  cout<<endl<<"check_print_selection: "
      <<centre_p->print_brother_usb<<", "
      <<centre_p->print_rnmark_ethernet<<", "
      <<centre_p->print_rnmark_usbserial<<endl;
  
  brother_usb_button_p->setChecked(centre_p->print_brother_usb);
  rnmark_ethernet_button_p->setChecked(centre_p->print_rnmark_ethernet);
  rnmark_serial_button_p->setChecked(centre_p->print_rnmark_usbserial);
}
  
void print_settings::toggle_brother_usb_printing()
{
  if(brother_usb_button_p->isChecked()) centre_p->print_brother_usb = true;
  else centre_p->print_brother_usb = false;
  cout<<endl<<"brother_usb: "<<brother_usb_button_p->isChecked()<<endl;
}

void print_settings::toggle_rnmark_ethernet_printing()
{
  if(rnmark_ethernet_button_p->isChecked()) centre_p->print_rnmark_ethernet = true;
  else centre_p->print_rnmark_ethernet = false;
  cout<<endl<<"rnmark_ethernet: "<<rnmark_ethernet_button_p->isChecked()<<endl;
}

void print_settings::toggle_rnmark_serial_printing()
{
  if(rnmark_serial_button_p->isChecked()) centre_p->print_rnmark_usbserial = true;
  else centre_p->print_rnmark_usbserial = false;
  cout<<endl<<"rnmark_serial: "<<rnmark_serial_button_p->isChecked()<<endl;
}

void print_settings::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
	centre_p->screen_done=true;
}


