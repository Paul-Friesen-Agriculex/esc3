
#include <QImage>
#include <QPrinter>
#include <QPainter>
#include <iostream>
#include "envelope.hpp"
#include "code_39_writer.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

envelope::envelope()
{
  
//  cout<<"\n\n**************************envelope_constructor\n\n";
  
  x=10;
  y=10;
  h=5;
  type=Ubuntu_mono;
  sample_row=0;
  image_p = 0;
  set_size(85, 150);
}

envelope::~envelope()
{
  
  cout<<"\n\n**************************envelope_destructor\n\n";
  
  if(image_p) delete image_p;
}

void envelope::set_size(int width_s, int height_s)//dimensions mm
{
  if(image_p)
  {
    delete image_p;
  }
  image_p = new QImage(width_s*pixels_per_mm, height_s*pixels_per_mm, QImage::Format_Mono);
  width = width_s;
  height = height_s;
  refresh_image();
}

int envelope::get_width()
{
  return width;
}

int envelope::get_height()
{
  return height;
}

int envelope::get_selected_x()
{
  return(field_list[selected_field].x);
}

int envelope::get_selected_y()
{
  return(field_list[selected_field].y);
}

int envelope::get_selected_h()
{
  return(field_list[selected_field].h);
}

envelope_field_type envelope::get_selected_type()
{
  return(field_list[selected_field].type);
}

void envelope::move_selected_x(int val)
{
  field_list[selected_field].x = val;
  x=val;
  cout<<"envelope::move_selected_x.  x="<<x<<endl;
  refresh_image();
}

void envelope::move_selected_y(int val)
{
  field_list[selected_field].y = val;
  y=val;
  refresh_image();
}

void envelope::move_selected_h(int val)
{
  field_list[selected_field].h = val;
  h=val;
  refresh_image();
}

void envelope::change_selected_type(envelope_field_type val)
{
  field_list[selected_field].type = val;
  type = val;
  refresh_image();
}

void envelope::refresh_image()
{
  image_p -> fill(1);

  QPainter painter(image_p);
  code_39_writer writer(&painter);
  for(int i=0; i<field_list.size(); ++i)
  {
    QPen pen;
    if(i == selected_field)
    {
      pen.setBrush(Qt::red);
    }
    else
    {
      pen.setBrush(Qt::black);
    }
    painter.setPen(pen);
    switch(field_list[i].type)
    {
      case Ubuntu_mono:
        painter.setFont(QFont("Ubuntu Mono", field_list[i].h*pixels_per_mm));
        painter.drawText(field_list[i].x*pixels_per_mm, field_list[i].y*pixels_per_mm, field_list[i].column_p->data_list[sample_row]);
//        cout<<"drew text "<<field_list[i].column_p->data_list[sample_row].toStdString()<<endl;
        break;
      case code_39:
        writer.set_size(field_list[i].h*pixels_per_mm, field_list[i].h*pixels_per_mm/20);
        writer.write(field_list[i].x*pixels_per_mm, field_list[i].y*pixels_per_mm, &(field_list[i].column_p->data_list[sample_row]));
        break;
      default:
        cout<<"envelope::refresh_image - type not found\n";  
    }
  }
}

void envelope::enter_field(spreadsheet_column* column_p_s)
{
  
  cout<<"envelope::enter_field\n";
  cout<<"  x = "<<x<<endl;
  cout<<"  y = "<<y<<endl;
  cout<<"  h = "<<h<<endl;
  cout<<"  type = "<<type<<endl;
  
  envelope_field ef;
  ef.column_p = column_p_s;
  ef.x = x;
  ef.y = y;
  ef.h = h;
  ef.type = type;
  
  field_list.append(ef);
  selected_field = field_list.size()-1;
  
  cout<<"field_list headings\n";
  for(int i=0; i<field_list.size(); ++i)
  {
    cout<<field_list[i].column_p->heading.toStdString()<<endl;
  }
}

int envelope::delete_field(QString heading)//return 1 on success, 0 on failure
{
  for(int i=0; i<field_list.size(); ++i)
  {
    if(field_list[i].column_p->heading == heading)
    {
      field_list.removeAt(i);
      refresh_image();
      return 1;
    }
  }
  return 0;
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

    
