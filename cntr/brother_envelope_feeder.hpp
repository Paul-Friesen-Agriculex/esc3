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

#ifndef BROTHER_ENVELOPE_FEEDER_HPP
#define BROTHER_ENVELOPE_FEEDER_HPP

#include <QObject>

class QTimer;

class brother_envelope_feeder:public QObject
{
  Q_OBJECT
  
  public:
  brother_envelope_feeder();
  ~brother_envelope_feeder();
  
  private:
  bool envelope_feeder_motor_running;
  bool envelope_feeder_motor_triggered;
  bool envelope_feeder_sensor_triggered;
  QTimer* delay_envelope_taken;
  QTimer* delay_sensor_triggered;
  
  public slots:
  void run();

  private slots:  
  void envelope_feeder_sensor_state();
  void envelope_feeder_motor_state();
  void motor_on();
  void motor_off();
};

#endif
