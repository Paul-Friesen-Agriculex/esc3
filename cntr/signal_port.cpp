#include <QTimer>
#include "signal_port.hpp"
#include <stdio.h>

signal_port::signal_port()
{
  pulse_timer_p = new QTimer;
  pulse_timer_p -> setSingleShot(true);
  connect(pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_pulse()));
}

signal_port::~signal_port()
{
  pulse_timer_p -> stop();
  delete pulse_timer_p;
}

void signal_port::pulse()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  pulse_timer_p -> start(500);
}

void signal_port::end_pulse()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}

