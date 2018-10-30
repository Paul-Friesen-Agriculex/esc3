#include <QTimer>
#include "cutgate.hpp"
#include <stdio.h>

cutgate::cutgate()
{
  open_pulse_timer_p = new QTimer;
  close_pulse_timer_p = new QTimer;
  open_pulse_timer_p -> setSingleShot(true);
  close_pulse_timer_p -> setSingleShot(true);
  connect(open_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_open()));
  connect(close_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_close()));
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
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio171/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  open_pulse_timer_p -> start(500);//changed from 400 2018-03-28.  think it was 300 originally
  state = CUTGATE_OPEN;
}

void cutgate::close()
{
  
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio172/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  close_pulse_timer_p -> start(120);//changed from 80 2018-06-24
  state = CUTGATE_CLOSED;
}

void cutgate::end_open()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio171/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}

void cutgate::end_close()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio172/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}

