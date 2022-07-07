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

