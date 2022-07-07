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

#ifndef CODE_39_WRITER_HPP
#define CODE_39_WRITER_HPP


class QString;
class QPainter;

class code_39_writer
{
  private:
  
  QPainter* target_p;
  int height;
  int narrow_width;//of narrow bar
  int wide_width;
  float width_factor;
  int start_x;
  int start_y;
  int x;
  int y;
  void draw_bars(char w1, char w2, char w3, char w4, char w5, char w6, char w7, char w8, char w9);
  
  public:
  code_39_writer(QPainter* target_p_s);
  void set_size(int height_s, int narrow_width_s);
  void write(int x_s, int y_s, QString* text_p);
};
  
  


#endif
