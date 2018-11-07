#include "diagnostics_console.hpp"
#include "centre.hpp"

#include <iostream>
using namespace std;

diagnostics_console::diagnostics_console(centre* centre_p_s, QWidget* parent)
:QWidget(parent)
{
  centre_p = centre_p_s;
  
  start_button_p = new QPushButton("Start recording");
  stop_button_p = new QPushButton("Stop recording");
  reset_time_tests_button_p = new QPushButton("Reset time tests");
  message_p = new QLabel("");
  
  layout_p = new QVBoxLayout;
  layout_p->addWidget(start_button_p);
  layout_p->addWidget(stop_button_p);
  layout_p->addWidget(reset_time_tests_button_p);
  
  layout_p->addWidget(message_p);
  setLayout(layout_p);
  
  connect(start_button_p, SIGNAL(clicked()), this, SLOT(start_recording()));
  connect(stop_button_p, SIGNAL(clicked()), this, SLOT(stop_recording()));
  connect(reset_time_tests_button_p, SIGNAL(clicked()), this, SIGNAL(reset_time_tests_signal()));
  
  setGeometry(1000,0,100,100);
  
  cout<<"end of diagnostics_console::diagnostics_console\n";
}

void diagnostics_console::start_recording()
{
  centre_p->set_to_record_f(true);
}

void diagnostics_console::stop_recording()
{
  centre_p->set_to_record_f(false);
}

void diagnostics_console::receive_message(QString message)
{
  message_p -> setText(message);
}
