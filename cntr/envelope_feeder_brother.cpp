#include <QTimer>
#include "envelope_feeder_brother.hpp"
#include <stdio.h>

envelope_feeder_brother::envelope_feeder_brother()
{
  feed_pulse_timer_p = new QTimer;
  feed_pulse_timer_p -> setSingleShot(true);
  connect(feed_pulse_timer_p, SIGNAL(timeout()), this, SLOT(end_feed()));
}

envelope_feeder_brother::~envelope_feeder_brother()
{
  feed_pulse_timer_p -> stop();
  delete feed_pulse_timer_p;
}

void envelope_feeder_brother::feed()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "1");
  fclose(fp);
  feed_pulse_timer_p -> start(600);
}

void envelope_feeder_brother::end_feed()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}

