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

#include <QPainter>
#include <iostream>
#include "display.hpp"

using namespace std;

extern int valid_start; //defined in processor.cpp
extern int valid_end;
extern const int line_length; //defined in centre.cpp

display::display(int lines_to_display_set, QWidget* parent)
:QLabel(parent)
{
  lines_to_display = lines_to_display_set;
}

void display::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, false);

  QTransform transform(float(width())/float(line_length), 0, 0, float(height())/float(lines_to_display), 0, 0);
  painter.setTransform(transform);
  painter.drawImage(0,0,image);
  QPen pen;
  pen.setColor(QColor(255,0,0));
  painter.setPen(pen);
  
  painter.drawLine(valid_start, 0, valid_start, lines_to_display);
  painter.drawLine(valid_end, 0, valid_end, lines_to_display);
}


