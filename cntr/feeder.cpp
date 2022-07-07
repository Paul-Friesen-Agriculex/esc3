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

#include <QThread>
#include "feeder.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;



loop_timer::loop_timer(int* period_p_s, bool* running_p_s)
{
//  cout<<"loop_timer constructor\n";
  pulse_state = false;
  period_p = period_p_s;
  running_p = running_p_s;
//  cout<<"end loop_timer constructor\n";
}

void loop_timer::run()
{
  while(*running_p)
  {
    usleep(*period_p);
    FILE * fp;
    fp = fopen("/sys/class/gpio/gpio209/value", "w");
    if(pulse_state == true)
    {
      pulse_state = false;
      fprintf(fp, "0");
    }
    else
    {
      pulse_state = true;
      fprintf(fp, "1");
    }
    fclose(fp);
  }
}


feeder::feeder()
{
  running = false;
  speed = 0;
  period = 0;
  loop_timer_finished = true;
}

feeder::~feeder()
{
  running = false;//will stop loop_timer, which will set loop_timer_finished
  while(1)
  {
    if(loop_timer_finished) break;
  }
}

void feeder::loop_timer_finished_slot()
{
  loop_timer_finished = true;
}

void feeder::set_speed(int speed_set)
{
  speed = speed_set;
  if (speed <= 0)
  {
    running = false;
  }
  else
  {
    period = 1000000/speed;
    if(running == false)
    {
      running = true;
      loop_timer_finished = false;
      loop_timer_p = new loop_timer(&period, &running);
      connect(loop_timer_p, SIGNAL(finished()), loop_timer_p, SLOT(deleteLater()));
      connect(loop_timer_p, SIGNAL(finished()), this, SLOT(loop_timer_finished_slot()));
      loop_timer_p -> start();
    }
  }
}
