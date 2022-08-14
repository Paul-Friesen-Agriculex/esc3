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

#include "message_box.hpp"
#include <QPainter>
#include <iostream>

using namespace std;


message_box::message_box(QWidget* parent)
:QWidget(parent)
{
  foreground = QColor(0,0,0);
  background = QColor(255, 255, 255);
  text = QString("");
  text_size = 16;
  update();
}

void message_box::set_text(QString text_s)
{
  if(text==text_s) return;
  text = text_s;
  update();
}

void message_box::set_background(int r, int g, int b)
{
  background = QColor(r, g, b);
//  update();
}

void message_box::set_background(QColor background_s)
{
  background = background_s;
//  update();
}

void message_box::set_foreground(int r, int g, int b)
{
  foreground = QColor(r, g, b);
//  update();
}

void message_box::set_foreground(QColor foreground_s)
{
  foreground = foreground_s;
//  update();
}

void message_box::set_text_size(int size_s)
{
  text_size = size_s;
//  cout<<"message_box::set_text_size.  text_size = "<<text_size<<endl;
//  cout<<"  text = "<<text.toStdString()<<endl;
//  update();
}

void message_box::paintEvent(QPaintEvent*) Q_DECL_OVERRIDE
{
  QPainter painter(this);
  QRect window(0,0,width(),height());
  painter.setBackground(QBrush(background));
  painter.setBackgroundMode(Qt::OpaqueMode);
  painter.setPen(foreground);
  painter.fillRect(window, QBrush(background));
  QFont font;
  font.setPixelSize(text_size);
  painter.setFont(font);
  painter.drawText(window, Qt::AlignTop | Qt::AlignLeft, text);
}
  
