#include "recording_control.hpp"
#include "centre.hpp"

#include <iostream>
using namespace std;

recording_control::recording_control(centre* centre_p_s, QWidget* parent)
:QWidget(parent)
{
  centre_p = centre_p_s;
  
  start_button_p = new QPushButton("Start");
  stop_button_p = new QPushButton("Stop");
  
  layout_p = new QVBoxLayout;
  layout_p->addWidget(start_button_p);
  layout_p->addWidget(stop_button_p);
  setLayout(layout_p);
  
  connect(start_button_p, SIGNAL(clicked()), this, SLOT(start_recording()));
  connect(stop_button_p, SIGNAL(clicked()), this, SLOT(stop_recording()));
  
  setGeometry(1000,0,100,100);
  
  cout<<"end of recording_control::recording_control\n";
}

void recording_control::start_recording()
{
  centre_p->set_to_record_f(true);
}

void recording_control::stop_recording()
{
  centre_p->set_to_record_f(false);
}
