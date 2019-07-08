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


