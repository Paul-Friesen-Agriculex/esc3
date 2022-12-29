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

#include <stdio.h>
#include <iostream>
#include <QTimer>

#include "envelope_sensor.hpp"

using namespace std;

envelope_sensor::envelope_sensor()
{
  timer_p = new QTimer;
  raw_present = false;//raw output of sensor
  old_raw_present = false;
  filtered_present = false;//short glitches in raw_present removed
  duration_count = 0;
  connect (timer_p, SIGNAL(timeout()), this, SLOT(run()));
  timer_p->start(100);
  
  physical_sensor_enabled = true;
  software_triggered = false;
  software_trigger_count = 100;
  
}

void envelope_sensor::run()
{
  //for software trigger
  if(software_trigger_count<20)
  {
    software_triggered = true;
    ++software_trigger_count;
  }
  else
  {
    software_triggered = false;
  }

  //for physical sensor
  FILE * fp;
  int i;
  fp = fopen("/sys/class/gpio/gpio174/value", "r");
  if(!fscanf(fp,"%d",&i)) cout<<"envelope_sensor::run.  fscanf failure\n";
  fclose(fp);
  if (i==1)
  {
    raw_present = false;
  }
  else
  {
    raw_present = true;
  }
  
  if(raw_present != old_raw_present)
  {
    duration_count = 0;
  }
  old_raw_present = raw_present;
  ++ duration_count;
  if(duration_count>=256) duration_count = 256;//limit to prevent possible overflow
  if(duration_count<3) return;//reading is unstable
  
  //only executes if reading is stable
  filtered_present = raw_present;
}

bool envelope_sensor :: read() //returns true if envelope is present
{
  if(physical_sensor_enabled)
  {
    return(filtered_present || software_triggered);
  }
  else
  {
    return software_triggered;
  }
}

void envelope_sensor::software_trigger()//generates a pulse as though the sensor was triggered
{
  software_trigger_count = 0;
}

void envelope_sensor::enable_physical_sensor(bool enabled)//physical sensor is enabled by default.  Send false to disable and trigger only by software.
{
  physical_sensor_enabled = enabled;
}
