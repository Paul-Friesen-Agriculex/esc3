#include <QTimer>
#include "endgate.hpp"
#include <stdio.h>
#include "centre.hpp"
#include "signal_port.hpp"
#include <iostream>

using namespace std;

endgate::endgate(centre* centre_p_s)
{
  centre_p = centre_p_s;
  signal_port_p = new signal_port;
  open_pulse_timer_p = new QTimer;
  close_pulse_timer_p = new QTimer;
  open_pulse_timer_p -> setSingleShot(true);
  close_pulse_timer_p -> setSingleShot(true);
  connect(open_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_open()));
  connect(close_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_close()));
  open();
  state = ENDGATE_OPEN;
}

endgate::~endgate()
{
  open_pulse_timer_p -> stop();
  close_pulse_timer_p -> stop();
  delete open_pulse_timer_p;
  delete close_pulse_timer_p;
}

void endgate::open()
{
  cout<<"endgate::open\n";
  
  if(state == ENDGATE_OPEN) return;
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio189/value", "w");
  fprintf(fp, "1");
  fclose(fp);
//  open_pulse_timer_p -> start(200);
  open_pulse_timer_p -> start(600);
  state = ENDGATE_OPEN;
}

void endgate::close()
{
  cout<<"endgate::close\n";
  
  if(state == ENDGATE_CLOSED) return;
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio210/value", "w");
  fprintf(fp, "1");
  fclose(fp);
//  close_pulse_timer_p -> start(200);
  close_pulse_timer_p -> start(600);
  state = ENDGATE_CLOSED;
}

void endgate::end_open()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio189/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}

void endgate::end_close()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio210/value", "w");
  fprintf(fp, "0");
  fclose(fp);
  if(centre_p->signal_port_pulse_when_endgate_closes)
  {
    signal_port_p->pulse();
  }
}

