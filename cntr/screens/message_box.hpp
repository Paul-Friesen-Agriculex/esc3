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

#include <QWidget>
#include <QString>
#include <QColor>

class message_box:public QWidget
{
  Q_OBJECT
  
  public:
  message_box(QWidget* parent = 0);
  void set_text(QString text_s);
  void set_background(int r, int g, int b);
  void set_background(QColor background_s);
  void set_foreground(int r, int g, int b);
  void set_foreground(QColor foreground_s);
  void set_text_size(int size_s);
  
  protected:
  void paintEvent(QPaintEvent* event);
  
  private:
  QColor foreground;
  QColor background;
  QString text;
  int text_size;
};
