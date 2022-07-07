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


#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <QWidget>
#include <QString>
#include <QList>

class QImage;
struct spreadsheet_column;
class envelope_feeder_brother;
class QTimer;

enum envelope_field_type{Ubuntu_mono, code_39, none};

struct envelope_field
{
  spreadsheet_column* column_p;
  envelope_field_type type;
  int x;
  int y;
  int h;
  char data_source_flag;//'d' -> data.  'h' -> heading.  't' -> text entry
  QString text;//only used in case of text entry.  otherwise blank
};

class envelope : public QObject
{
  Q_OBJECT

  private:
  int selected_field;
  static const float pixels_per_mm = 47;
  void refresh_image();
  envelope_feeder_brother* e_f_brother_p;
  QTimer* timer_p;

  //envelope dimensions mm
  int width;
  int height;

  //default values for next field entry
  int x;
  int y;
  int h;
  envelope_field_type type;

  int sample_row;//row number of spreadsheet line to use as sample
  
  public:
  QList<envelope_field> field_list;
  envelope();
  ~envelope();
  QImage* image_p;
  void set_size(int width, int height);//dimensions mm - printed size
  int get_width();
  int get_height();
  bool printing;//true indicates envelope is printing, so selected text should change to black.
  
  //position of selected field
  int get_selected_x();
  int get_selected_y();
  int get_selected_h();
  envelope_field_type get_selected_type();
  
  void move_selected_x(int val);
  void move_selected_y(int val);
  void move_selected_h(int val);
  void change_selected_type(envelope_field_type val);
  
  void enter_ss_data_field(spreadsheet_column* column_p_s);
  void enter_ss_heading_field(spreadsheet_column* column_p_s);
  void enter_text_field(QString text_s);
  void delete_field();//delete selected field
  void clear_fields();//delete all fields
  void select_previous_field();
  void select_next_field();
  
  void print();
  void print(int line_number);
  void feed();
};

#endif

