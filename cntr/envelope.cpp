
#include <QImage>
#include <QPrinter>
#include <QPainter>
#include <QTimer>
#include <iostream>
#include "envelope.hpp"
#include "code_39_writer.hpp"
#include "batch_mode_driver.hpp"
#include "envelope_feeder_brother.hpp"
#include "rnmark_printer.hpp" //

using namespace std;

envelope::envelope(batch_mode_driver* batch_mode_driver_p_s)
{
  
//  cout<<"\n\n**************************envelope_constructor\n\n";
  batch_mode_driver_p = batch_mode_driver_p_s;
  
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
  rnmark_printer_p = new rnmark_printer;

  
//  timer_p = new QTimer;
//  timer_p -> setSingleShot(true);
//  connect(timer_p, SIGNAL(timeout()), e_f_brother_p, SLOT(feed()));

  feedback_timer_p = new QTimer; 
  connect(feedback_timer_p, SIGNAL(timeout()), this, SLOT(rnmark_printer_p->read_packets_from_printer()));
  feedback_timer_p->start(1000);
}

envelope::~envelope()
{
  
  cout<<"\n\n**************************envelope_destructor\n\n";
  
  if(image_p) delete image_p;
  if(e_f_brother_p) delete e_f_brother_p;
  if(rnmark_printer_p) delete rnmark_printer_p;
  feedback_timer_p -> stop();
  delete feedback_timer_p;
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
  cout<<"envelope::move_selected_x.  x="<<x<<endl;
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
  cout<<"envelope::refresh_image.  sample_row = "<<sample_row<<endl;
  
  //serial_transmission_str->clear();
  
  //image_p -> fill(QColor(255, 255, 255));
  image_p -> fill(Qt::white);

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
//        writer.write(field_list[i].x*pixels_per_mm, (field_list[i].y-10)*pixels_per_mm, &(field_list[i].column_p->data_list[sample_row]));


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
//  cout<<"envelope::enter_ss_data_field\n";
//  cout<<"  x = "<<x<<endl;
//  cout<<"  y = "<<y<<endl;
//  cout<<"  h = "<<h<<endl;
//  cout<<"  type = "<<type<<endl;
  
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
  
//  cout<<"field_list headings\n";
//  for(int i=0; i<field_list.size(); ++i)
//  {
//    cout<<field_list[i].column_p->heading.toStdString()<<endl;
//  }
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
//  ef.text = batch_mode_driver_p -> text_for_envelope_field;
  ef.text = text_s;
  
  field_list.append(ef);
  selected_field = field_list.size()-1;
}

void envelope::delete_field()//delete selected field
{
  field_list.removeAt(selected_field);
  if(selected_field>=field_list.size()) selected_field = field_list.size()-1;
  if(selected_field<0) selected_field=0;
//  cout<<"selected_field="<<selected_field<<endl;
  refresh_image();
}

void envelope::clear_fields()//delete all fields
{
  field_list.clear();
  selected_field = 0;
  refresh_image();
}

/*
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
*/
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
  
  //Brother USB Printer//
  if(batch_mode_driver_p->brother_usb_setting())  
  {
    cout<<endl<<"BM_DRIVER::BROTHER_USB: "<<batch_mode_driver_p->brother_usb_setting()<<endl;
    
    QPrinter printer(QPrinter::HighResolution);
//  QPrinter printer(QPrinter::ScreenResolution);
//  printer.setPaperSize(QPrinter::B7);
    QPainter painter(&printer);
//  image_p -> invertPixels();
//  painter.drawImage(QRect(0,0,3000,6000), *image_p);

//  painter.drawImage(image_p->rect(), *image_p);
    QPoint startpoint( (92-width/2)*pixels_per_mm ,  -10 );
    painter.drawImage(startpoint, *image_p);
  }
  
  //USB Serial Data Transmission - 32 character limit per envelope field, padded with "&"//  
  if((rnmark_printer_p->check_usb_connection()) && (batch_mode_driver_p->rnmark_usbserial_setting()))
  {
    QString serial_transmission_str;
    serial_transmission_str.clear();
  
    cout<<endl<<"field_list.size(): "<<field_list.size()<<endl;
    for(int q=0; q<field_list.size(); ++q)
    {
      if(field_list[q].data_source_flag == 'd')
      {
        qDebug()<<field_list[q].column_p->data_list[sample_row];
        serial_transmission_str.append(field_list[q].column_p->data_list[sample_row]);
        while(serial_transmission_str.size() < (q+1)*32) serial_transmission_str.append("&");
      }
      else if(field_list[q].data_source_flag == 'h')
      {
        qDebug()<<field_list[q].column_p->heading;
        serial_transmission_str.append(field_list[q].column_p->heading);
        while(serial_transmission_str.size() < (q+1)*32) serial_transmission_str.append("&");
      }
      else if(field_list[q].data_source_flag == 't')
      {
        qDebug()<<field_list[q].text;
        serial_transmission_str.append(field_list[q].text);
        while(serial_transmission_str.size() < (q+1)*32) serial_transmission_str.append("&");
      }
    }
    rnmark_printer_p->send_barcode_serial_transmission(serial_transmission_str);
    serial_transmission_str.clear();
  }

  //Socket Ethernet Transmission//
  if((rnmark_printer_p->check_socket_connection()) && (batch_mode_driver_p->rnmark_ethernet_setting()))
  {
    QImage convert_alphamask_p;
    convert_alphamask_p = image_p->scaled(512, 896, Qt::KeepAspectRatio);
    convert_alphamask_p = convert_alphamask_p.convertToFormat(QImage::Format_MonoLSB, Qt::MonoOnly);
    convert_alphamask_p.save("rnmark_image", "BMP");

    rnmark_printer_p->clear_ram();
    rnmark_printer_p->retrieve_bmp_dimensions();
    rnmark_printer_p->set_printing_area_width();
    rnmark_printer_p->generate_bmp_bytearray();
    
    rnmark_printer_p->print_once_mode();
    rnmark_printer_p->resume_print();
  }

//  image_p -> invertPixels();
  
//  timer_p -> start(10000);

  printing = false;//return to colour for screen display
  refresh_image();

//  cout<<"end envelope::print\n";

}

void envelope::print(int line_number)
{
  if(line_number<0) return;//-1 is used to signal no more rows
  sample_row = line_number;
  refresh_image();
  print();
}
    
void envelope::feed()
{
  e_f_brother_p->feed();
}
