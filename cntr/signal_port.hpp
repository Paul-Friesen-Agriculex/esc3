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

#ifndef signal_port_HPP
#define signal_port_HPP

#include <QObject>

class QTimer;

class signal_port:public QObject
{
  Q_OBJECT
  
  public:
  signal_port();
  ~signal_port();
  
  public slots:
  void pulse(void);

  private slots:
  void end_pulse(void);
  
  private:
  QTimer* pulse_timer_p;
};

#endif

/*
For this code to work, some setup is needed:

Rootless GPIO for Odroid, Lubuntu 15.10

append to  /etc/udev/rules.d/10-odroid.rules


SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:odroid /sys/class/gpio/export /sys/class/gpio/unexport ; chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:odroid /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value ; chmod 660 /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value'"


insert to /etc/rc.local before "exit 0"  note: modified 2017-06-26 after prototype assembly

echo 173 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio173/direction

echo 171 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio171/direction

echo 172 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio172/direction

echo 189 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio189/direction

echo 210 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio210/direction

echo 209 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio209/direction

echo 19  >> /sys/class/gpio/export
echo out  >> /sys/class/gpio/gpio19/direction

echo 28  >> /sys/class/gpio/export
echo out  >> /sys/class/gpio/gpio28/direction

echo 174  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio174/direction

echo 192  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio192/direction

echo 191  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio191/direction

echo 190  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio190/direction

*/
