
#include <QImage>
#include <QPrinter>
#include <QPainter>
#include <iostream>
#include "envelope.hpp"
#include "code_39_writer.hpp"

using namespace std;

envelope::envelope(QWidget *parent)
 : QWidget(parent)
{
  setMinimumSize(300,600);//size of screen widget in pixels
  setMaximumSize(300,600);

  first_field = 0;
  column = 0;
  type = Ubuntu_mono;
  text = "";
  x = 0;//mm
  y = 0;//mm
  height = 5;//mm
  image_p = 0;
  set_size(85, 150);//size of envelope in mm (w X h)
}

envelope::~envelope()
{
  envelope_field* ef_p1;
  envelope_field* ef_p2;
  ef_p1 = first_field;
  while(ef_p1 != 0)
  {
    ef_p2 = ef_p1;
    ef_p1 = ef_p1 -> next;
    delete ef_p2;
  }
}

void envelope::set_size(int width_s, int height_s)
{
  if(image_p)
  {
    delete image_p;
  }
  image_p = new QImage(width_s*pixels_per_mm, height*pixels_per_mm, QImage::Format_Mono);
  image_p -> fill(1);
}

void envelope::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.drawImage(QRect(0,0,300,600), *image_p);
}

void envelope::enter_field()
{
  QPainter painter(image_p);
  code_39_writer writer(&painter);
  switch(type)
  {
    case Ubuntu_mono:
      painter.setFont(QFont("Ubuntu Mono", height*pixels_per_mm));
      painter.drawText(x*pixels_per_mm, y*pixels_per_mm, text);
      break;
    case code_39:
      writer.set_size(height*pixels_per_mm, height*pixels_per_mm/20);
      writer.write(x, y, &text);
      break;
    default:
      cout<<"envelope::enter_field - type not found\n";  
  }
}

void envelope::print()
{
  QPrinter printer(QPrinter::HighResolution);
  printer.setPaperSize(QPrinter::B7);
  QPainter painter(&printer);
  image_p -> invertPixels();
  painter.drawImage(QRect(0,0,3000,6000), *image_p);
  image_p -> invertPixels();
}

    
