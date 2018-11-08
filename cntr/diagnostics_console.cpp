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

  message1_p = new QLabel("");
  message2_p = new QLabel("");
  message3_p = new QLabel("");
  message4_p = new QLabel("");
  message5_p = new QLabel("");
  message6_p = new QLabel("");
  message7_p = new QLabel("");
  status_line_p = new QLabel("");
  
  layout_p = new QVBoxLayout;
  layout_p->addWidget(start_button_p);
  layout_p->addWidget(stop_button_p);
  layout_p->addWidget(reset_time_tests_button_p);
  
  layout_p->addWidget(message1_p);
  layout_p->addWidget(message2_p);
  layout_p->addWidget(message3_p);
  layout_p->addWidget(message4_p);
  layout_p->addWidget(message5_p);
  layout_p->addWidget(message6_p);
  layout_p->addWidget(message7_p);
  layout_p->addWidget(status_line_p);
  setLayout(layout_p);
  
  timer_p = new QTimer(this);
  timer_p -> start(1000);
  
  connect(start_button_p, SIGNAL(clicked()), this, SLOT(start_recording()));
  connect(stop_button_p, SIGNAL(clicked()), this, SLOT(stop_recording()));
  connect(reset_time_tests_button_p, SIGNAL(clicked()), this, SIGNAL(reset_time_tests_signal()));
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  
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

void diagnostics_console::receive_message1(QString message)
{
  message1_p -> setText(message);
}

void diagnostics_console::receive_message2(QString message)
{
  message2_p -> setText(message);
}

void diagnostics_console::receive_message3(QString message)
{
  message3_p -> setText(message);
}

void diagnostics_console::receive_message4(QString message)
{
  message4_p -> setText(message);
}

void diagnostics_console::receive_message5(QString message)
{
  message5_p -> setText(message);
}

void diagnostics_console::receive_message6(QString message)
{
  message6_p -> setText(message);
}

void diagnostics_console::receive_message7(QString message)
{
  message7_p -> setText(message);
}

void diagnostics_console::run()
{
  QString status_string;
  if(centre_p->block_endgate_opening)
  {
    status_string.append("block_endgate_opening true\n");
  }
  else
  {
    status_string.append("block_endgate_opening false\n");
  }
  status_string.append("envelope_present ");
  if(centre_p->envelope_present) status_string.append("true\n");
  else status_string.append("false\n");
  status_string.append(QString("current_screen %1 \n").arg(centre_p->current_screen));
  status_string.append(QString("endgate_state %1 \n").arg(centre_p->get_endgate_state()));
  
  status_line_p -> setText(status_string);
}
