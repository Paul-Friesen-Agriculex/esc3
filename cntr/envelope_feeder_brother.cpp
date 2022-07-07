/***********

Copyright 2021 Agriculex Inc.

This file is part of the Agriculex ESC-3 program.

The Agriculex ESC-3 program is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, GPL-3.0-or-later.

The Agriculex ESC-3 program is distributed in the hope that it will be 
useful, but, except as noted below, WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Agriculex ESC-3 program.  If not, see 
<https://www.gnu.org/licenses/>.

Agriculex Inc. warrants that executable files of the ESC-3 program 
compiled and installed by Agriculex Inc. will function as advertized in 
ESC-3 machines produced by Agriculex Inc.

**********/

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
  feed_pulse_timer_p -> start(2000);
}

void envelope_feeder_brother::end_feed()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}

