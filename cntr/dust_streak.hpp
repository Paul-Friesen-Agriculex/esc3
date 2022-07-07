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

#ifndef DUST_STREAK_HPP
#define DUST_STREAK_HPP


class processor;
class centre;

class dust_streak//each dust_streak is for a single pixel.  detects if obscured by dust on camera window
{
  long long length;
  static const int detect_length = 100;//if a pixel is obscured more lines than this, assume a dust streak
  static const int detect_value = 150;//pixels with less than this intensity considered obscured
  
  public:
  bool started;//true if pixel is below detect_value.  could be due to a seed.
  bool confirmed;//true if pixel has been below detect_value for detect_length lines
  bool in_streak_zone;//this is set by the processor.  it is true for pixels where a streak is confirmed, and also for several pixels on either side.  
  //inflections in streak zone are ignored.
  
  dust_streak();
  void add_line(int pixel_value);
};

#endif
