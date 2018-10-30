#include "message_box.hpp"
#include <QPainter>


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
  text = text_s;
  update();
}

void message_box::set_background(int r, int g, int b)
{
  background = QColor(r, g, b);
  update();
}

void message_box::set_foreground(int r, int g, int b)
{
  foreground = QColor(r, g, b);
  update();
}

void message_box::set_text_size(int size_s)
{
  text_size = size_s;
  update();
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
  
