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


#include <QImage>
#include <QPrinter>
#include <QPainter>
#include <QTimer>
#include <iostream>
#include "envelope.hpp"
#include "code_39_writer.hpp"
#include "batch_mode_driver.hpp"
#include "envelope_feeder_brother.hpp"

using namespace std;

envelope::envelope()
{
  selected_field=0;
  x=10;
  y=10;
  h=5;
  type=Ubuntu_mono;
  sample_row=0;
  image_p = 0;
  set_size(85, 150);
  printing = false;
  
  e_f_brother_p = new envelope_feeder_brother;
}

envelope::~envelope()
{
  if(image_p) delete image_p;
  if(e_f_brother_p) delete e_f_brother_p;
  timer_p -> stop();
  delete timer_p;
}

void envelope::set_size(int width_s, int height_s)//dimensions mm
{
  if(image_p)
  {
    delete image_p;
  }
  //image printing actually starts 10mm down from top of envelope
  image_p = new QImage(width_s*pixels_per_mm, (height_s-10)*pixels_per_mm, QImage::Format_RGB16);
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
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return(0);//invalid
  return(field_list[selected_field].x);
}

int envelope::get_selected_y()
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return(0);//invalid
  return(field_list[selected_field].y);
}

int envelope::get_selected_h()
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return(0);//invalid
  return(field_list[selected_field].h);
}

envelope_field_type envelope::get_selected_type()
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return(Ubuntu_mono);//invalid
  return(field_list[selected_field].type);
}

void envelope::move_selected_x(int val)
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return;//invalid
  field_list[selected_field].x = val;
  x=val;
  refresh_image();
}

void envelope::move_selected_y(int val)
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return;//invalid
  field_list[selected_field].y = val;
  y=val;
  refresh_image();
}

void envelope::move_selected_h(int val)
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return;//invalid
  field_list[selected_field].h = val;
  h=val;
  refresh_image();
}

void envelope::change_selected_type(envelope_field_type val)
{
  if(  (selected_field>=field_list.size())  ||  (selected_field<0)  ) return;//invalid
  field_list[selected_field].type = val;
  type = val;
  refresh_image();
}

void envelope::refresh_image()
{
  image_p -> fill(QColor(255, 255, 255));

  QPainter painter(image_p);
  code_39_writer writer(&painter);
  for(int i=0; i<field_list.size(); ++i)
  {
    if( (i==selected_field) && (printing==false) )
    {
      painter.setPen(QPen(Qt::red));
      painter.setBrush(QBrush(Qt::red));
    }
    else
    {
      painter.setPen(QPen(Qt::black));
      painter.setBrush(QBrush(Qt::black));
    }
    switch(field_list[i].type)
    {
      case Ubuntu_mono:
        painter.setFont(QFont("Ubuntu Mono", field_list[i].h*pixels_per_mm));
        //image printing actually starts 10mm down from top of envelope
        if(field_list[i].data_source_flag == 'd')
        {
          painter.drawText(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, field_list[i].column_p->data_list[sample_row]);
        }
        else if(field_list[i].data_source_flag == 'h')
        {
          painter.drawText(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, field_list[i].column_p->heading);
        }
        else if(field_list[i].data_source_flag == 't')
        {
          painter.drawText(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, field_list[i].text);
        }
        break;
      case code_39:
        writer.set_size(field_list[i].h*pixels_per_mm, field_list[i].h*pixels_per_mm/20);
        //image printing actually starts 10mm down from top of envelope

        if(field_list[i].data_source_flag == 'd')
        {
          writer.write(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, &field_list[i].column_p->data_list[sample_row]);
        }
        else if(field_list[i].data_source_flag == 'h')
        {
          writer.write(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, &field_list[i].column_p->heading);
        }
        else if(field_list[i].data_source_flag == 't')
        {
          writer.write(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, &field_list[i].text);
        }

        break;
      default:
        cout<<"envelope::refresh_image - type not found\n";  
    }
  }
}

void envelope::enter_ss_data_field(spreadsheet_column* column_p_s)
{
  y += 10;
  if(y>=height) y=height;
  
  envelope_field ef;
  ef.column_p = column_p_s;
  ef.x = x;
  ef.y = y;
  ef.h = h;
  ef.type = type;
  ef.data_source_flag = 'd';
  ef.text = "";
  
  field_list.append(ef);
  selected_field = field_list.size()-1;
}

void envelope::enter_ss_heading_field(spreadsheet_column* column_p_s)
{
  y += 10;
  if(y>=height) y=height;
  
  envelope_field ef;
  ef.column_p = column_p_s;
  ef.x = x;
  ef.y = y;
  ef.h = h;
  ef.type = type;
  ef.data_source_flag = 'h';
  ef.text = "";
  
  field_list.append(ef);
  selected_field = field_list.size()-1;
}

void envelope::enter_text_field(QString text_s)
{
  y += 10;
  if(y>=height) y=height;
  
  envelope_field ef;
  ef.column_p = 0;
  ef.x = x;
  ef.y = y;
  ef.h = h;
  ef.type = type;
  ef.data_source_flag = 't';
  ef.text = text_s;
  
  field_list.append(ef);
  selected_field = field_list.size()-1;
}

void envelope::delete_field()//delete selected field
{
  field_list.removeAt(selected_field);
  if(selected_field>=field_list.size()) selected_field = field_list.size()-1;
  if(selected_field<0) selected_field=0;
  refresh_image();
}

void envelope::clear_fields()//delete all fields
{
  field_list.clear();
  selected_field = 0;
  refresh_image();
}

void envelope::select_previous_field()
{
  --selected_field;
  if(selected_field<0) selected_field = 0;
  refresh_image();
}

void envelope::select_next_field()
{
  ++selected_field;
  if(selected_field>=field_list.size()) selected_field = field_list.size()-1;
  refresh_image();
}

void envelope::print()
{
  cout<<"start envelope::print\n";
  printing = true;//black and white image for printing
  refresh_image();
  
  QPrinter printer(QPrinter::HighResolution);
  QPainter painter(&printer);
  QPoint startpoint( (92-width/2)*pixels_per_mm ,  -10 );
  painter.drawImage(startpoint, *image_p);
  printing = false;//return to colour for screen display
  refresh_image();
}

void envelope::print(int line_number)
{
  cout<<"start envelop::print("<<line_number<<")\n";
  if(line_number<0) return;//-1 is used to signal no more rows
  sample_row = line_number;
//  refresh_image();
  print();
}
    
void envelope::feed()
{
  e_f_brother_p->feed();
}
