#include <QTimer>
#include "cutgate.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;

cutgate::cutgate()
{
  open_pulse_timer_p = new QTimer;
  close_pulse_timer_p = new QTimer;
  open_pulse_timer_p -> setSingleShot(true);
  close_pulse_timer_p -> setSingleShot(true);
  connect(open_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_open()));
  connect(close_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_close()));
  opening = false;
  closing = false;

  open();
  state = CUTGATE_OPEN;
}

cutgate::~cutgate()
{
  open_pulse_timer_p -> stop();
  close_pulse_timer_p -> stop();
  delete open_pulse_timer_p;
  delete close_pulse_timer_p;
}

void cutgate::open()
{
  if( (state==CUTGATE_OPEN) || (state==CUTGATE_OPENING) ) return;
  if(closing==true)
  {
    cout<<"cutgate opened while closing\n";
    emit opened_while_closing();
    return;
  }
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio171/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  open_pulse_timer_p -> start(700);
  state = CUTGATE_OPENING;
//  state = CUTGATE_OPEN;
  opening = true;
}

void cutgate::close()
{
  if( (state==CUTGATE_CLOSED) || (state==CUTGATE_CLOSING) ) return;
  if(opening==true)
  {
    cout<<"cutgate closed while opening\n";
    emit closed_while_opening();
    return;
  }
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio172/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  close_pulse_timer_p -> start(300);
  state = CUTGATE_CLOSING;
//  state = CUTGATE_CLOSED;
  closing = true;
}

void cutgate::end_open()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio171/value", "w");
  fprintf(fp, "0");
  fclose(fp);
  opening = false;
  state = CUTGATE_OPEN;
}

void cutgate::end_close()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio172/value", "w");
  fprintf(fp, "0");
  fclose(fp);
  closing = false;
  state = CUTGATE_CLOSED;
}

