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

#ifndef crop_hpp
#define crop_hpp


struct crop
{
  QString name;
  int sensitivity;
  int gate_setting;
  int area_mean;
  int area_stdev;
  float max_inflection_1;//measured with arms 1% of outline length
  float max_inflection_3;//measured with arms 3% of outline length
  float max_inflection_9;//measured with arms 9% of outline length
  bool calibrated;
  
  int high_feed_speed;
  int low_feed_speed;
  int dump_speed;
};

#endif
