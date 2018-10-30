#include <QTimer>
#include "endgate.hpp"
#include <stdio.h>

endgate::endgate()
{
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
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio189/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  open_pulse_timer_p -> start(200);
  state = ENDGATE_OPEN;
}

void endgate::close()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio210/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  close_pulse_timer_p -> start(200);
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
}

