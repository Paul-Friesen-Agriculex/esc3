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

#include "batch_mode_driver.hpp"
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <iomanip>
#include <QMessageBox>
#include <QDateTime>
#include <QPushButton>
#include <QtMath> //2021_03_19


using namespace std;

batch_mode_driver::batch_mode_driver(centre* centre_p_s, cutgate* cutgate_p_s, endgate* endgate_p_s, envelope_sensor* envelope_sensor_p_s)
{
  centre_p = centre_p_s;
  cutgate_p = cutgate_p_s;
  endgate_p = endgate_p_s;
  envelope_sensor_p = envelope_sensor_p_s;
  connect(cutgate_p, SIGNAL(closed_while_opening()), this, SLOT(cutgate_closed_while_opening()));
  pack_present = old_pack_present = false;
  pack_changed = true;
  timer_p = new QTimer;
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  mode = entry;
  mode_changed = false;
  mode_new = true;
  old_count = 0;
  program_size = 0;
  high_feed_speed = 100;
  low_feed_speed = 10;
  dump_speed = 1000;
  
  //batch options
  require_seed_lot_barcode = false;
  require_pack_barcode = false;
  pack_match_lot = false;
  pack_contain_lot = false;
  lot_contain_pack = false;
  pack_match_spreadsheet = false;
  record_only = true;
  
  barcode_mode = seed_lot;
  barcode_is_new = false;
  seed_lot_barcode_old = true;//need a new scan
  pack_barcode_old = true;//need a new scan
  seed_lot_barcode_ok = false;
  pack_barcode_ok = false;
  pack_barcode_ok_message_posted = false;
  pack_barcode_wrong = false;
  pack_barcode_wrong_message_posted = false;
  release_pack = false;//true signals to release counted seed, even if barcode matching not satisfied.  For use in case of lost packet.
  
  dump_end_qtime.start();
  
  force_dump_out = false;

  use_spreadsheet = false;
  ss_setup_p = new ss_setup;
  clear_ss_setup();
  ss_setup_path = "";
  envelope_layout_path = "";
  envelope_p = new envelope;
  sample_row = 0;
  print_envelope = false;
  print_control_mode = start_on_pack_collect;
  field_data_source_flag = 'd'; //use spreadsheet data in print field (not heading).
  
  fill_extra_pack = false;
  extra_pack_filling = false;
  extra_pack_count_limit = 0;
  extra_pack_stored_count_limit = 0;
  extra_pack_finished = false;
  
  substitute_seed_lot = false;
  substitution_barcode_ok = false;
  slave_mode = false;
  
  for(int i=0; i<5; ++i)//initialize count_rate_observation _queue.  5 members-> 1 second of observations stored
  {
    count_rate_observation obs;
    obs.turntable_speed = 0;
    obs.count_rate = 0;
    count_rate_observation_queue.enqueue(obs);
  }
  
  slowdown_time = .75;
  count_rate_old_count = centre_p->count;
  count_rate_time.start();
  count_rate_interval = 0.2;
  count_rate = 0;//seeds/sec
  hi_rate = 0;
  slowdown_count_diff = 0;//seed feeder slows down when count is this many seeds from limit
  stop_count_diff = 0;//will stop feeder if reaches this in mode hi_closed
  ramp_down_counter = 0;
  
  cutgate_set = 0;
  cutgate_pack = 0;
  endgate_set = 0;
  endgate_pack = 0;
  out_set = 0;
  out_pack = 0;
  first_repeat_pack = true;
  cutgate_count_limit = 0;
  cutgate_pack_limit = 0;
  endgate_count_limit = 0;
  endgate_pack_limit = 0;
  
  dump_automatically = true;
  new_slave_mode_command_bool = false;
  
  //diagnostics
  cout_counter = 0;
  cout_counter_limit = 500;
}
  
batch_mode_driver::~batch_mode_driver()
{
  for(int i=0; i<program_size; ++i) delete program[i];
  program.clear();
  timer_p -> stop();
  delete timer_p;
  if(set_p) delete set_p;
}

void batch_mode_driver::set_normal_status_message()
{
  QString message;
  if(use_spreadsheet)
  {
    if(seed_lot_barcode_ok)
    {
      message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\nMaterial %4.\nPack %5.\n%6 Seeds\n%7 packs left for this material.")
        .arg(centre_p->crops[0].name) 
        .arg(centre_p->crops[0].sensitivity) 
        .arg(centre_p->crops[0].gate_setting)
        .arg(seed_lot_barcode)
        .arg(ss_envelope_id_p->data_list[end_valve_spreadsheet_line_number])
        .arg(cutgate_count_limit)
        .arg(lines_left_to_fill);
    }
    else
    {
      message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.")
        .arg(centre_p->crops[0].name) 
        .arg(centre_p->crops[0].sensitivity) 
        .arg(centre_p->crops[0].gate_setting);
    }    
    send_status_message(message, QColor(0,0,0), QColor(30, 200, 255), 14);
  }
  else
  {
    message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\nProgram %4.\nPack %5 of %6.\n%7 seeds/pack.") 
      .arg(centre_p->crops[0].name) 
      .arg(centre_p->crops[0].sensitivity) 
      .arg(centre_p->crops[0].gate_setting)
      .arg(program_name)
      .arg(endgate_pack+1)
      .arg(cutgate_pack_limit)
      .arg(cutgate_count_limit);
    send_status_message(message, QColor(0,0,0), QColor(30, 200, 255), 16);
  }
  
}

void batch_mode_driver::set_incorrect_barcode_message()
{
  QString bsm = "Incorrect Barcode\n               Scanned:  ";
  bsm.append(barcode_as_entered);
  if(use_spreadsheet)
  {
    bsm.append                  ("\n              Required:  ");
    bsm.append(spreadsheet_pack_barcode);
  }
  else
  {
    bsm.append                  ("\nSeed lot barcode:  ");
    bsm.append(seed_lot_barcode);
  }
  send_barcode_status_message(bsm, QColor(0,0,0), QColor(255,0,0), 12);
}

void batch_mode_driver::switch_mode(mode_enum new_mode, string str)
{
  cout<<"switching mode to "<<str<<"  count "<<centre_p->count<<endl;
  mode = new_mode;
  mode_changed = true;
}

void batch_mode_driver::load_program()//load the program indicated by program_path
{
  for(int i=0; i<program_size; ++i) delete program[i];
  program.clear();
  program_size = 0;
  QFile infile(program_path);
  infile.open(QIODevice::ReadOnly);
  QTextStream stream(&infile);
  QString line;
  QString subline;
  bool conversion_ok_flag = true;
  while(stream.readLineInto(&line))
  {
    if(line == "set")
    {
      set_p = new bm_set;
      stream.readLineInto(&line);
      set_p->packs = line.toInt();
      stream.readLineInto(&line);
      set_p->seeds = line.toInt();
      program.append(set_p);
      ++program_size;
    }
    else if(line == "high_feed_speed")
    {
      stream.readLineInto(&subline);
      high_feed_speed = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "low_feed_speed")
    {
      stream.readLineInto(&subline);
      low_feed_speed = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "dump_speed")
    {
      stream.readLineInto(&subline);
      dump_speed = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "require_seed_lot_barcode")
    {
      stream.readLineInto(&subline);
      require_seed_lot_barcode = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "require_pack_barcode")
    {
      stream.readLineInto(&subline);
      require_pack_barcode = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "pack_match_lot")
    {
      stream.readLineInto(&subline);
      pack_match_lot = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "pack_contain_lot")
    {
      stream.readLineInto(&subline);
      pack_contain_lot = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "lot_contain_pack")
    {
      stream.readLineInto(&subline);
      lot_contain_pack = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "pack_match_spreadsheet")
    {
      stream.readLineInto(&subline);
      pack_match_spreadsheet = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "record_only")
    {
      stream.readLineInto(&subline);
      record_only = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else
    {
      cout<<"batch_mode_driver::load_program.  data type identifier not found.\n";
    }
  }
    
  cutgate_set = 0;
  cutgate_pack = 0;
  endgate_set = 0;
  endgate_pack = 0;
  out_set = 0;
  out_pack = 0;
  first_repeat_pack = true;
  cutgate_count_limit = program[cutgate_set]->seeds;
  cutgate_pack_limit = program[cutgate_set]->packs;
}

void batch_mode_driver::reset_program()
{
  if(program_size == 0) return;

  cutgate_set = 0;
  cutgate_pack = 0;
  endgate_set = 0;
  endgate_pack = 0;
  out_set = 0;
  out_pack = 0;
  first_repeat_pack = true;
  cutgate_count_limit = program[cutgate_set]->seeds;
  cutgate_pack_limit = program[cutgate_set]->packs;

  mode = entry;
  barcode_mode = seed_lot;
  barcode_is_new = false;
  seed_lot_barcode_old = true;//need a new scan
  pack_barcode_old = true;//need a new scan
  seed_lot_barcode_ok = false;
  pack_barcode_ok = false;
  pack_barcode_ok_message_posted = false;
  pack_barcode_wrong = false;
  pack_barcode_wrong_message_posted = false;

  dump_end_qtime.restart();
}  

void batch_mode_driver::clear_program()
{
  for(int i=0; i<program_size; ++i)
  {
    delete program[i];
  }
  program.clear();
  program_size = 0;
}

void batch_mode_driver::add_line(int packs, int seeds)
{
  set_p = new bm_set;
  set_p -> packs = packs;
  set_p -> seeds = seeds;
  program.append(set_p);
  ++program_size;
}

bool batch_mode_driver::program_is_empty()
{
  if(program_size==0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int batch_mode_driver::get_program_size()
{
  return program_size;
}

int batch_mode_driver::get_packs(int index)
{
  if( (program_size<=0) || (index<0) || (index>program_size) )
  {
    return 0;
  }
  return program[index]->packs;
}

int batch_mode_driver::get_seeds(int index)
{
  if( (program_size<=0) || (index<0) || (index>program_size) )
  {
    return 0;
  }
  return program[index]->seeds;
}

void batch_mode_driver::program_remove_last()
{
  program.removeLast();
  --program_size;
}

void batch_mode_driver::start()
{
  cout<<"batch_mode_driver::start\n";
  timer_p->start(1);
  count_rate_old_count = centre_p->count;
  count_rate_time.start();
}

void batch_mode_driver::stop()
{
  cout<<"batch_mode_driver::stop\n";
  timer_p->stop();
}

void batch_mode_driver::restart()
{
  switch_mode(dump_into_end_gate, "dump_into_end_gate");
  pack_barcode_ok = false;
  pack_barcode_old = true;
  cutgate_pack = 0;
  cutgate_set = 0;
  if(use_spreadsheet)
  {
    for(int i=0; i<spreadsheet_number_of_lines; ++i)
    {
      if(ss_material_id_p->data_list[i] == seed_lot_barcode)
      {
        ss_first_column_p->data_list[i] = "N";
      }
    }
  }
}

void batch_mode_driver::save_program(QString filename)
{
  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    cout<<"batch_mode_driver::save_program.  File open failure\n";
    return;
  }
  QTextStream stream(&file);
  for(int i=0; i<program_size; ++i)
  {
    stream<<"set\n";
    stream<<program[i]->packs<<endl;
    stream<<program[i]->seeds<<endl;
  }
  stream<<"high_feed_speed\n";
  stream<<high_feed_speed<<endl;
  stream<<"low_feed_speed\n";
  stream<<low_feed_speed<<endl;
  stream<<"dump_speed\n";
  stream<<dump_speed<<endl;
  stream<<"require_seed_lot_barcode\n";
  stream<<require_seed_lot_barcode<<endl;
  stream<<"require_pack_barcode\n";
  stream<<require_pack_barcode<<endl;
  stream<<"pack_match_lot\n";
  stream<<pack_match_lot<<endl;
  stream<<"pack_contain_lot\n";
  stream<<pack_contain_lot<<endl;
  stream<<"lot_contain_pack\n";
  stream<<lot_contain_pack<<endl;
  stream<<"pack_match_spreadsheet\n";
  stream<<pack_match_spreadsheet<<endl;
  stream<<"record_only\n";
  stream<<record_only<<endl;
}

void batch_mode_driver::load_spreadsheet(QString filename)
{
  spreadsheet_filename = filename;
  QFile infile(filename);
  if(!infile.open(QIODevice::ReadOnly))
  {
  	cout<<"batch_mode_driver::load_spreadsheet.  file not open\n";
	  exit(1);
  }
  
  QTextStream stream(&infile);
  QString line;
  QStringList list;
  ss_first_column_p = 0;
  spreadsheet_column* ss_column_p;
  stream.readLineInto(&line);
  list = line.split(",");
  spreadsheet_number_of_columns = 0;
  for(int i=list.size()-1; i>=0; --i)
  {
    ss_column_p = ss_first_column_p;
    ss_first_column_p = new spreadsheet_column;
    ss_first_column_p->heading = list[i];
    ss_first_column_p->next = ss_column_p;
    ++spreadsheet_number_of_columns;
  }
  spreadsheet_number_of_lines=0;
  while(stream.readLineInto(&line))
  {
    spreadsheet_number_of_lines++;
    list = line.split(",");
    ss_column_p = ss_first_column_p;
    for(int i=0; i<list.size() && i<spreadsheet_number_of_columns; ++i)
    {
      ss_column_p->data_list.append(list[i]);
      ss_column_p = ss_column_p->next;
    }
    for(int i=list.size(); i<spreadsheet_number_of_columns; ++i)//in case of empty columns, append null strings.  Not expected to execute normally
    {
      ss_column_p->data_list.append("");
      ss_column_p = ss_column_p->next;
    }
  }
  infile.close();
  
  if(ss_first_column_p->heading != "Filled")//if spreadsheet does not have this column, create it
  {
    ss_column_p = ss_first_column_p;
    ss_first_column_p = new spreadsheet_column;
    ss_first_column_p->heading = "Filled";
    ss_first_column_p->next = ss_column_p;
    for(int i=0; i<spreadsheet_number_of_lines; ++i)
    {
      ss_first_column_p->data_list.append("N");
    }
  }
  mode = entry;
  ss_column_p = ss_first_column_p;

  //look for first unfilled row, initialize spreadsheet_line_number
  for(int i=0; i<spreadsheet_number_of_lines; ++i)
  {
    if(ss_first_column_p->data_list[i] == "N")
    {
      spreadsheet_line_number = i;
      break;
    }
  }
}

int batch_mode_driver::save_spreadsheet(QString filename)
{
  QFile outfile(filename);
  if(!outfile.open(QIODevice::WriteOnly)) return 0;
  QTextStream stream(&outfile);
  spreadsheet_column* ss_column_p = ss_first_column_p;
  while(ss_column_p->next != 0)
  {
    stream<<ss_column_p->heading;
    stream<<',';
    ss_column_p = ss_column_p->next;
  }
  stream<<ss_column_p->heading;
  stream<<'\n';
  for(int i=0; i<spreadsheet_number_of_lines; ++i)
  {
    ss_column_p = ss_first_column_p;
    while(ss_column_p->next != 0)
    {
      stream<<ss_column_p->data_list[i];
      stream<<',';
      ss_column_p = ss_column_p->next;
    }
    stream<<ss_column_p->data_list[i];
    stream<<'\n';
  }
  return 1;
}

void batch_mode_driver::print_seed_lot_envelopes(QString mat_id)
{
  for(int i=0; i<ss_material_id_p->data_list.size(); ++i)
  {
    if( (ss_material_id_p->data_list[i]==mat_id) && (ss_first_column_p->data_list[i]!="Y") )
    {
      envelope_p -> print(i);
    }
  }
}

void batch_mode_driver::list_program()
{
  for (int i=0; i<program_size; ++i)
  {
    cout<<program[i]->packs<<" packs, "<<program[i]->seeds<<" seeds each.\n" ;
  }
  cout<<endl;
}

void batch_mode_driver::run()
{
  
//  int ctime = test_time.restart();
//  if(ctime > 5) cout<< "batch mode run cycle "<<ctime<<" ms.  count "<<centre_p->count<<endl;
  
  old_pack_present = pack_present;
  pack_present = centre_p->envelope_present;
  
  if(record_only) pack_barcode_ok = true;
  
  if(count_rate_time.elapsed() > count_rate_interval*1000.0)
  {
    int new_count = centre_p->count;
    float measured_interval = float(count_rate_time.restart()) / 1000.0;//in seconds
    float inst_count_rate = float(new_count - count_rate_old_count) / measured_interval;//counts/sec
    count_rate_old_count = new_count;
    count_rate_observation cro;
    cro.turntable_speed = centre_p->feed_speed;
    cro.count_rate = inst_count_rate;
    count_rate_observation_queue.enqueue(cro);
    count_rate_observation_queue.dequeue();
    
    int historic_speed = count_rate_observation_queue[3].turntable_speed;//best predictor of current count rate
    if(  (historic_speed>0)  &&  (inst_count_rate>0)  )
    {
      float inst_count_rate_multiplier = float(inst_count_rate)/float(historic_speed);
      if(inst_count_rate_multiplier>count_rate_multiplier)
      {
        count_rate_multiplier = (count_rate_multiplier*3.0 + inst_count_rate_multiplier)/4.0;
      }
      else
      {
        count_rate_multiplier = (count_rate_multiplier*99.0 + inst_count_rate_multiplier)/100.0;
      }
    }
    centre_p->crops[0].count_rate_multiplier = count_rate_multiplier;
    hi_rate = high_feed_speed*count_rate_multiplier;
    slowdown_count_diff = hi_rate * slowdown_time;
    stop_count_diff = slowdown_count_diff*0.8;
  }
  
  if(mode_changed) mode_new = true;//this is the signal to run the setup part of the new mode.  It is set false at the end of run().
  mode_changed = false;
  
  switch(mode)
  {
    case entry:
      cout<<"mode = entry\n";
      cutgate_p->open();
      endgate_p->close();
      envelope_sensor_p->read_rise();//to eat any rise signal from dumping or anything else
      substitute_seed_lot = false;
      substitution_barcode_ok = false;
      count_rate_multiplier = centre_p->crops[0].count_rate_multiplier;
      if(require_seed_lot_barcode)
      {
        seed_lot_barcode_ok = false;
        switch_mode(wait_for_seed_lot_barcode, "wait_for_seed_lot_barcode");
      }
      else
      {
        send_barcode_status_message("Barcode Not Required", QColor(0,0,0), QColor(0,255,0), 25);
        switch_mode(hi_o_c, "hi_o_c");
      }
      break;
    case wait_for_seed_lot_barcode:
      if(mode_new)
      {
        set_normal_status_message();
        send_barcode_status_message("Scan Seed Lot Barcode", QColor(0,0,0), QColor(255,255,0), 25);
        barcode_mode = seed_lot;
        centre_p->set_speed(0);
      }
      if(seed_lot_barcode_ok)
      {
        if(use_spreadsheet == true)
        {
          spreadsheet_line_number = get_next_spreadsheet_line_number();
          if(spreadsheet_line_number>=0)
          {
            centre_p->bar_str_1 = ss_envelope_id_p->data_list[spreadsheet_line_number];
          }
          emit refresh_screen();
          end_valve_spreadsheet_line_number = spreadsheet_line_number;
          if(print_envelope)
          {
            if(print_control_mode==start_on_pack_collect)
            {
              envelope_p -> print(spreadsheet_line_number);
            }
            else if(print_control_mode==start_on_previous_pack_collect)
            {
              envelope_p -> print(spreadsheet_line_number);
              envelope_p -> print(get_spreadsheet_line_number_after(spreadsheet_line_number));
            }
          }
          if(spreadsheet_line_number==-1)//-1 value signals no more lines for this seed_lot_barcode
          {
            QMessageBox box;
            box.setText(QString("No unfilled rows for seed lot %1. ").arg(seed_lot_barcode));
            QPushButton* rescan_button_p = box.addButton(QString("Rescan"), QMessageBox::ActionRole);
            QPushButton* exit_button_p = box.addButton(QString("Exit"), QMessageBox::ActionRole);
            QPushButton* dump_button_p = box.addButton(QString("Dump out seed"), QMessageBox::ActionRole);
            box.exec();
            
            if(box.clickedButton() == rescan_button_p)
            {
              seed_lot_barcode_ok = false;
            }
            if(box.clickedButton() == exit_button_p)
            {
              stop();
              centre_p->add_waiting_screen(0);
              centre_p->screen_done = true;
            }
            else if (box.clickedButton() == dump_button_p)
            {
              seed_lot_barcode_ok = false;
              switch_mode(dump_into_end_gate, "dump_into_end_gate");
            }
          }
          else //have valid line number to fill
          {
            
            bool conversion_ok_flag;
            cutgate_count_limit = ss_required_count_p->data_list[spreadsheet_line_number].toInt(&conversion_ok_flag);
            if(conversion_ok_flag==false)
            {
              QString string("Problem with entry in required count column.  Unable to convert *");
              string.append(ss_required_count_p->data_list[spreadsheet_line_number]);
              string.append("* to integer.  Stopping processing.");
              QMessageBox box;
              box.setText(string);
              box.exec();
              stop();
              centre_p->add_waiting_screen(0);
              centre_p->screen_done = true;
            }  
            else
            {         
              send_barcode_status_message("Seed Lot Barcode Scanned", QColor(0,0,0), QColor(0,255,0), 25);
              barcode_mode = pack;
              switch_mode(hi_o_c, "hi_o_c");
            }
          }
        }
        else//use_spreadsheet is false
        {         
          send_barcode_status_message("Seed Lot Barcode Scanned", QColor(0,0,0), QColor(0,255,0), 25);
          barcode_mode = pack;
          switch_mode(hi_o_c, "hi_o_c");
        }
      }
      break;
    case hi_o_c:
      if(mode_new)
      {
        cutgate_p->open();
        endgate_p->close();
        endgate_count_limit = cutgate_count_limit;
        endgate_pack_limit = cutgate_pack_limit;
        endgate_set = cutgate_set;
        endgate_pack = cutgate_pack;
        barcode_mode = pack;
        if(require_pack_barcode)
        {
          if(use_spreadsheet)
          {
            QString bsm = "Scan Pack Barcode\n";
            bsm.append(ss_envelope_id_p->data_list[spreadsheet_line_number]);//end_valve_spreadsheet_line_number]);
            send_barcode_status_message(bsm, QColor(0,0,0), QColor(255,255,0), 14);
          }
          else
          {
            send_barcode_status_message("Scan Pack Barcode", QColor(0,0,0), QColor(255,255,0), 25);
          }
          pack_barcode_ok = false;
          pack_barcode_ok_message_posted = false;
          pack_barcode_wrong = false;
          pack_barcode_wrong_message_posted = false;
        }
        end_valve_spreadsheet_line_number = spreadsheet_line_number;
        set_normal_status_message();
        emit enable_substitute_button(true);
        release_pack = false;
      }
      if(use_spreadsheet)
      {
        pack_barcode_wrong_message_posted = false;
      }
      if(centre_p->feed_speed != high_feed_speed)
      {
        centre_p->set_speed(high_feed_speed);
      }
      if(cutgate_count_limit-centre_p->count <  slowdown_count_diff)
      {
        switch_mode(ramp_down_o_c, "ramp_down_o_c");
      }
      if(centre_p->count > lower_chamber_count_limit)
      {
        switch_mode(lower_chamber_full_o_c, "lower_chamber_full_o_c");
      }
      if(envelope_sensor_p->read_rise() && (pack_barcode_ok||release_pack)  )
      {
        switch_mode(hi_o_o, "hi_o_o");
      }
      if(require_pack_barcode == false)
      {
        send_barcode_status_message("Barcode Not Required", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
        break;
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      if(pack_barcode_wrong && pack_barcode_wrong_message_posted==false)
      {
        set_incorrect_barcode_message();
        pack_barcode_wrong_message_posted = true;
      }
      break;
    case lower_chamber_full_o_c:
      if(mode_new)
      {
        centre_p->set_speed(0);
        send_status_message("Waiting for\nPack", QColor(0,0,0), QColor(255,255,0), 16);
      }
      if(envelope_sensor_p->read_rise() && (pack_barcode_ok||release_pack))
      {
        switch_mode(hi_o_o, "hi_o_o");
      }
      if(require_pack_barcode == false)
      {
        send_barcode_status_message("Barcode Not Required", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
        break;
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      if(pack_barcode_wrong && pack_barcode_wrong_message_posted==false)
      {
        set_incorrect_barcode_message();
        pack_barcode_wrong_message_posted = true;
      }
      break;
    case ramp_down_o_c:
      ++ramp_down_counter;
      if(ramp_down_counter>50)
      {
        ramp_down_counter = 0;
        float ramp_speed = low_feed_speed + (high_feed_speed-low_feed_speed)*(cutgate_count_limit-centre_p->count)/slowdown_count_diff;
        if(ramp_speed < low_feed_speed) ramp_speed = low_feed_speed;
        centre_p->set_speed(ramp_speed);
      }
      if(centre_p->count >= cutgate_count_limit)
      {
        centre_p->count = 0;
        switch_mode(gate_delay_o_c, "gate_delay_o_c");
      }
      if(envelope_sensor_p->read_rise() && (pack_barcode_ok||release_pack))
      {
        switch_mode(ramp_down_o_o, "ramp_down_o_o");
      }
      if(require_pack_barcode == false)
      {
        send_barcode_status_message("Barcode Not Required", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
        break;
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      if(pack_barcode_wrong && pack_barcode_wrong_message_posted==false)
      {
        set_incorrect_barcode_message();
        pack_barcode_wrong_message_posted = true;
      }
      break;
    case gate_delay_o_c:
      if(mode_new)
      {
        cutoff_gate_close_time.restart();
      }
      if(cutoff_gate_close_time.elapsed() >= cutoff_gate_delay_time)
      {
        emit enable_substitute_button(false);
        cutgate_p->close();
        if(use_spreadsheet)
        {

          if(spreadsheet_line_number>=0) ss_first_column_p->data_list[spreadsheet_line_number] = "Y";
          centre_p->pack_count_str = QString::number(cutgate_count_limit);
          centre_p->communicate_out('p');

          spreadsheet_line_number = get_next_spreadsheet_line_number();
          if(spreadsheet_line_number>=0)
          {
            centre_p->bar_str_1 = ss_envelope_id_p->data_list[spreadsheet_line_number];
          }

          emit refresh_screen();
          if(spreadsheet_line_number == -1)
          {
            switch_mode(dump_into_cut_gate_c_c, "dump_into_cut_gate_c_c");
          }
          else
          {
            switch_mode(hi_c_c, "hi_c_c");
            bool conversion_ok_flag = false;
            cutgate_count_limit = ss_required_count_p->data_list[spreadsheet_line_number].toInt(&conversion_ok_flag);
            if(conversion_ok_flag==false)
            {
              QString string("Problem with entry in required count column.  Unable to convert *");
              string.append(ss_required_count_p->data_list[spreadsheet_line_number]);
              string.append("* to integer.  Stopping processing.");
              QMessageBox box;
              box.setText(string);
              box.exec();
              centre_p->add_waiting_screen(0);
              centre_p->screen_done = true;
            }
          }
        }
        else//not using spreadsheet
        {
          bool end_program = false;
          ++cutgate_pack;
          if(cutgate_pack>=cutgate_pack_limit)
          {
            cutgate_pack = 0;
            ++cutgate_set;
            if(cutgate_set>=program_size)
            {
              end_program = true;
              cutgate_set = 0;
            }
          }
          cutgate_count_limit = program[cutgate_set]->seeds;
          cutgate_pack_limit = program[cutgate_set]->packs;
          
          if(end_program)
          {
            if(dump_automatically == true)
            {
              switch_mode(dump_into_cut_gate_c_c, "dump_into_cut_gate_c_c");
            }
            else
            {
              emit slave_mode_set_finished();
              switch_mode(wait_for_slave_mode_command_c_c, "wait_for_slave_mode_command_c_c");
            }
          }
          else
          {
            switch_mode(hi_c_c, "hi_c_c");
          }
        }
      }
      break;
    case hi_o_o:
      if(mode_new)
      {
        centre_p->set_speed(high_feed_speed);
        cutgate_p->open();
        endgate_p->open();
        endgate_pack = cutgate_pack;
        out_pack = endgate_pack;
        endgate_set = cutgate_set;
        out_set = endgate_set;
        endgate_count_limit = cutgate_count_limit;
        endgate_pack_limit = cutgate_pack_limit;
        set_normal_status_message();
      }
      if(centre_p->feed_speed != high_feed_speed)
      {
        centre_p->set_speed(high_feed_speed);
      }
      if(cutgate_count_limit-centre_p->count <  slowdown_count_diff)
      {
        switch_mode(ramp_down_o_o, "ramp_down_o_o");
      }
      break;
    case ramp_down_o_o:
      if(mode_new)
      {
        cutgate_p->open();
        endgate_p->open();
        endgate_pack = cutgate_pack;
        out_pack = endgate_pack;
        endgate_set = cutgate_set;
        out_set = endgate_set;
        endgate_count_limit = cutgate_count_limit;
        endgate_pack_limit = cutgate_pack_limit;
      }
      ++ramp_down_counter;
      if(ramp_down_counter>50)
      {
        ramp_down_counter = 0;
        float ramp_speed = low_feed_speed + (high_feed_speed-low_feed_speed)*(cutgate_count_limit-centre_p->count)/slowdown_count_diff;
        if(ramp_speed < low_feed_speed) ramp_speed = low_feed_speed;
        centre_p->set_speed(ramp_speed);
      }
      if(centre_p->count >= cutgate_count_limit)
      {
        centre_p->count = 0;
        switch_mode(gate_delay_o_o, "gate_delay_o_o");
      }
      break;
    case gate_delay_o_o:
      if(mode_new)
      {
        cutoff_gate_close_time.restart();
      }
      if(cutoff_gate_close_time.elapsed() >= cutoff_gate_delay_time)
      {
        emit enable_substitute_button(false);
        cutgate_p->close();
        emit pack_ready();
        if(use_spreadsheet)
        {
          if(spreadsheet_line_number>=0) ss_first_column_p->data_list[spreadsheet_line_number] = "Y";
          centre_p->pack_count_str = QString::number(cutgate_count_limit);
          centre_p->communicate_out('p');
          spreadsheet_line_number = get_next_spreadsheet_line_number();
          if(spreadsheet_line_number>=0)
          {
            centre_p->bar_str_1 = ss_envelope_id_p->data_list[spreadsheet_line_number];
          }
          emit refresh_screen();
          if(spreadsheet_line_number == -1)
          {
            switch_mode(dump_into_cut_gate_c_c, "dump_into_cut_gate_c_c");
          }
          else
          {
            switch_mode(hi_c_o, "hi_c_o");
            bool conversion_ok_flag = false;
            cutgate_count_limit = ss_required_count_p->data_list[spreadsheet_line_number].toInt(&conversion_ok_flag);
            if(conversion_ok_flag==false)
            {
              QString string("Problem with entry in required count column.  Unable to convert *");
              string.append(ss_required_count_p->data_list[spreadsheet_line_number]);
              string.append("* to integer.  Stopping processing.");
              QMessageBox box;
              box.setText(string);
              box.exec();
              centre_p->add_waiting_screen(0);
              centre_p->screen_done = true;
            }
          }
        }
        else//use_spreadsheet false
        {
          bool end_program = false;
          ++cutgate_pack;
          if(cutgate_pack>=cutgate_pack_limit)
          {
            cutgate_pack = 0;
            ++cutgate_set;
            cout<<"mode gate_delay_o_o.  cutgate_pack zeroed.  cutgate_set = "<<cutgate_set<<endl;
            if(cutgate_set>=program_size)
            {
              end_program = true;
              cutgate_set = 0;
            }
          }
          cutgate_count_limit = program[cutgate_set]->seeds;
          cutgate_pack_limit = program[cutgate_set]->packs;
          
          if(end_program)
          {
            if(dump_automatically == true)
            {
              switch_mode(dump_into_cut_gate_c_o, "dump_into_cut_gate_c_o");
            }
            else
            {
              emit slave_mode_set_finished();
              switch_mode(wait_for_slave_mode_command_c_o, "wait_for_slave_mode_command_c_o");
            }
          }
          else
          {
            switch_mode(hi_c_o, "hi_c_o");
          }
        }
      }
      break;      
    case hi_c_o:
      if(mode_new)
      {
        centre_p->set_speed(high_feed_speed);
        cutgate_p->close();
        endgate_p->open();
        out_pack = endgate_pack;
        out_set = endgate_set;
        QString status_message;
        
        if(use_spreadsheet)
        {
          status_message = QString("Pack Ready.  Seed Lot:\n%1\nPack ID:\n%2.\n%3 seeds.\n%4 packs left for this lot.")
            .arg(seed_lot_barcode)
            .arg(ss_envelope_id_p->data_list[end_valve_spreadsheet_line_number])
            .arg(ss_required_count_p->data_list[end_valve_spreadsheet_line_number])
            .arg(lines_left_to_fill);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 14);
          if(print_envelope)
          {
            if(print_control_mode==start_on_pack_collect)
            {
              envelope_p -> print(spreadsheet_line_number);
            }
            else if(print_control_mode==start_on_previous_pack_collect)
            {
              envelope_p -> print(get_spreadsheet_line_number_after(spreadsheet_line_number));
            }
          }
        }
        else
        {
          status_message = QString("Pack Ready\nPack %1 of %2.\n%3 seeds.").arg(endgate_pack+1).arg(endgate_pack_limit).arg(endgate_count_limit);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 25);
        }
        end_chute_clear_time.restart();
      }
      if(cutgate_count_limit-centre_p->count <  stop_count_diff)
      {
        centre_p->set_speed(0);
      }
      if(end_chute_clear_time.elapsed() >= end_chute_clear_time_limit)
      {
        switch_mode(count_while_endgate_closing, "count_while_endgate_closing");
      }
      break;
    case count_while_endgate_closing:
      if(mode_new)
      {
        centre_p->set_speed(high_feed_speed);
        cutgate_p->close();
        endgate_p->close();
        endgate_close_time.restart();
        emit pack_collected(endgate_count_limit);
        if(use_spreadsheet)
        {
          if(ss_fill_time_p != 0)
          {
            QDateTime fill_time = QDateTime::currentDateTime();
            ss_fill_time_p -> data_list[end_valve_spreadsheet_line_number] = fill_time.toString(Qt::ISODate);
          }
        }
        refresh_screen();
      }
      if(cutgate_count_limit-centre_p->count <  stop_count_diff)
      {
        centre_p->set_speed(0);
      }
      if(endgate_close_time.elapsed()>endgate_close_time_limit)
      {
        switch_mode(hi_o_c, "hi_o_c");
      }
      break;
    case hi_c_c:
      if(mode_new)
      {
        centre_p->set_speed(high_feed_speed);
        cutgate_p->close();
        endgate_p->close();
        QString status_message;
        if(use_spreadsheet)
        {
          status_message = QString("Pack Ready.  Seed Lot:\n%1\nPack ID:\n%2.\n%3 seeds.\n%4 packs left for this lot.")
            .arg(seed_lot_barcode)
            .arg(ss_envelope_id_p->data_list[end_valve_spreadsheet_line_number])
            .arg(ss_required_count_p->data_list[end_valve_spreadsheet_line_number])
            .arg(lines_left_to_fill);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 14);
        }
        else
        {
          status_message = QString("Pack Ready\nPack %1 of %2.\n%3 seeds.").arg(endgate_pack+1).arg(endgate_pack_limit).arg(endgate_count_limit);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 25);
        }
      }
      if(centre_p->feed_speed != high_feed_speed)
      {
        centre_p->set_speed(high_feed_speed);
      }
      if(cutgate_count_limit-centre_p->count <  stop_count_diff)
      {
        switch_mode(wait_for_pack, "wait_for_pack");
      }
      if(centre_p->count > upper_chamber_count_limit)
      {
        switch_mode(wait_for_pack, "wait_for_pack");
      }
      if(envelope_sensor_p->read_rise() && (pack_barcode_ok||release_pack))
      {
        emit pack_ready();
        switch_mode(hi_c_o, "hi_c_o");
      }
      if(require_pack_barcode == false)
      {
        send_barcode_status_message("Barcode Not Required", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
        break;
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      if(pack_barcode_wrong && pack_barcode_wrong_message_posted==false)
      {
        set_incorrect_barcode_message();
        pack_barcode_wrong_message_posted = true;
      }
      break;
    case wait_for_endgate_to_close:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->close();
        endgate_close_time.restart();
        emit pack_collected(endgate_count_limit);
        if(use_spreadsheet)
        {
          if(ss_fill_time_p != 0)
          {
            QDateTime fill_time = QDateTime::currentDateTime();
            ss_fill_time_p -> data_list[end_valve_spreadsheet_line_number] = fill_time.toString(Qt::ISODate);
          }
        }
        refresh_screen();
      }
      if(endgate_close_time.elapsed()>endgate_close_time_limit)
      {
        if(pack_present==false)
        {
          switch_mode(hi_o_c, "hi_o_c");
        }
        else
        {
          switch_mode(hi_o_c_old_pack, "hi_o_c_old_pack");
        }
      }
      break;
    case hi_o_c_old_pack:
      if(mode_new)
      {
        centre_p->set_speed(high_feed_speed);
        cutgate_p->open();
        endgate_p->close();
        endgate_set = cutgate_set;
        endgate_pack = cutgate_pack;
      }
      if(centre_p->feed_speed != high_feed_speed)
      {
        centre_p->set_speed(high_feed_speed);
      }
      if(cutgate_count_limit-centre_p->count <  stop_count_diff)
      {
        switch_mode(wait_old_pack, "wait_old_pack");
      }
      if(pack_present == false)
      {
        switch_mode(hi_o_c, "hi_o_c");
      }
      break;
    case wait_old_pack:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->open();
        endgate_p->close();
      }
      if(pack_present == false)
      {
        switch_mode(hi_o_c, "hi_o_c");
      }
      break;
    case wait_for_pack:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->close();
        emit enable_repeat_pack_button(true);
      }
      if(envelope_sensor_p->read_rise() && (pack_barcode_ok||release_pack))
      {
        emit enable_repeat_pack_button(false);
        switch_mode(wait_for_endgate_to_clear, "wait_for_endgate_to_clear");
      }
      if(require_pack_barcode == false)
      {
        send_barcode_status_message("Barcode Not Required", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
        break;
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      if(pack_barcode_wrong && pack_barcode_wrong_message_posted==false)
      {
        set_incorrect_barcode_message();
        pack_barcode_wrong_message_posted = true;
      }
      break;
    case wait_for_endgate_to_clear:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->open();
        end_chute_clear_time.restart();
        if(use_spreadsheet)
        {
          if(print_envelope)
          {
            if(print_control_mode==start_on_pack_collect)
            {
              cout<<"mode wait_for_endgate_to_clear printing line "<<spreadsheet_line_number<<endl;
              envelope_p -> print(spreadsheet_line_number);
            }
            else if(print_control_mode==start_on_previous_pack_collect)
            {
              cout<<"mode wait_for_endgate_to_clear printing line "<<get_spreadsheet_line_number_after(spreadsheet_line_number)<<endl; 
              cout<<"  calling print(get_spreadsheet_line_number_after).  get_spreadsheet_line_number_after = "<<get_spreadsheet_line_number_after(spreadsheet_line_number)<<endl;
              envelope_p -> print(get_spreadsheet_line_number_after(spreadsheet_line_number));
            }
          }
        }
      }
      if(end_chute_clear_time.elapsed() > end_chute_clear_time_limit)
      {
        emit pack_ready();
        switch_mode(wait_for_endgate_to_close, "wait_for_endgate_to_close");
      }
      break;
    case dump_into_cut_gate_c_o:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->close();
        endgate_p->open();
        out_pack = endgate_pack;
        QString status_message;
        if(use_spreadsheet)
        {
          status_message = QString("Pack Ready.  Seed Lot:\n%1\nPack ID:\n%2.\n%3 seeds.\n%4 packs left for this lot.")
            .arg(seed_lot_barcode)
            .arg(ss_envelope_id_p->data_list[end_valve_spreadsheet_line_number])
            .arg(ss_required_count_p->data_list[end_valve_spreadsheet_line_number])
            .arg(lines_left_to_fill);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 14);
        }
        else
        {
          status_message = QString("Pack Ready\nPack %1 of %2.\n%3 seeds.").arg(endgate_pack+1).arg(endgate_pack_limit).arg(endgate_count_limit);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 25);
        }        
      }
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(pack_present==false)
      {
        switch_mode(dump_into_cut_gate_wait_for_endgate_to_close, "dump_into_cut_gate_wait_for_endgate_to_close");
      }
      if(centre_p->count > upper_chamber_count_limit)
      {
        switch_mode(dump_into_cut_gate_wait_for_pack_removal, "dump_into_cut_gate_wait_for_pack_removal");
      }
      break;
    case dump_into_cut_gate_c_c:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->close();
        endgate_p->close();
        QString status_message;
        if(use_spreadsheet)
        {
          status_message = QString("Pack Ready.  Seed Lot:\n%1\nPack ID:\n%2.\n%3 seeds.\n%4 packs left for this lot.")
            .arg(seed_lot_barcode)
            .arg(ss_envelope_id_p->data_list[end_valve_spreadsheet_line_number])
            .arg(ss_required_count_p->data_list[end_valve_spreadsheet_line_number])
            .arg(lines_left_to_fill);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 14);
        }
        else
        {
          status_message = QString("Pack Ready\nPack %1 of %2.\n%3 seeds.").arg(endgate_pack+1).arg(endgate_pack_limit).arg(endgate_count_limit);
          send_status_message(status_message, QColor(0,0,0), QColor(0,255,0), 25);
        }
      }
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(envelope_sensor_p->read_rise() && (pack_barcode_ok||release_pack))
      {
        switch_mode(dump_into_cut_gate_wait_for_endgate_to_clear, "dump_into_cut_gate_wait_for_endgate_to_clear");
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      if(pack_barcode_wrong && pack_barcode_wrong_message_posted==false)
      {
        set_incorrect_barcode_message();
        pack_barcode_wrong_message_posted = true;
      }
      if(centre_p->count > upper_chamber_count_limit)
      {
        switch_mode(dump_into_cut_gate_wait_for_pack, "dump_into_cut_gate_wait_for_pack");
      }
      break;
    case dump_into_cut_gate_wait_for_endgate_to_close:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->close();
        endgate_p->close();
        endgate_close_time.restart();
        emit pack_collected(endgate_count_limit);
        if(use_spreadsheet)
        {
          if(ss_fill_time_p != 0)
          {
            QDateTime fill_time = QDateTime::currentDateTime();
            ss_fill_time_p -> data_list[end_valve_spreadsheet_line_number] = fill_time.toString(Qt::ISODate);
          }
        }
        refresh_screen();
      }
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(endgate_close_time.elapsed() > endgate_close_time_limit)
      {
        switch_mode(dump_into_end_gate, "dump_into_end_gate");
      }
      break;
    case dump_into_cut_gate_wait_for_pack:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->close();
        emit enable_repeat_pack_button(true);
      }
      if(envelope_sensor_p->read_rise()==true && (pack_barcode_ok||release_pack))
      {
        emit enable_repeat_pack_button(false);
        switch_mode(dump_into_cut_gate_wait_for_endgate_to_clear, "dump_into_cut_gate_wait_for_endgate_to_clear");
      }
      if(pack_barcode_ok && pack_barcode_ok_message_posted==false)
      {
        send_barcode_status_message("Barcode OK", QColor(0,0,0), QColor(0,255,0), 25);
        pack_barcode_ok_message_posted = true;
      }
      break;
    case dump_into_cut_gate_wait_for_pack_removal:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->open();
      }
      if(pack_present==false)
      {
        switch_mode(dump_into_cut_gate_wait_for_endgate_to_close, "dump_into_cut_gate_wait_for_endgate_to_close");
      }
      break;
    case dump_into_cut_gate_wait_for_endgate_to_clear:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->open();
        end_chute_clear_time.restart();
      }
      if(centre_p->feed_speed != 0)
      {
        centre_p->set_speed(0);
      }
      if(end_chute_clear_time.elapsed() >= end_chute_clear_time_limit)
      {
        switch_mode(dump_into_cut_gate_wait_for_pack_removal, "dump_into_cut_gate_wait_for_pack_removal");
      }
      break;
    case dump_into_end_gate:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->open();
        endgate_p->close();
        send_status_message("Place Dump\nContainer", QColor(0,0,0), QColor(255,255,0), 25);
        send_barcode_status_message("", QColor(0,0,0), QColor(0,255,0), 25);
      }
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->count > lower_chamber_count_limit)
      {
        cout<<"centre_p->count = "<<centre_p->count<<".  lower_chamber_count_limit = "<<lower_chamber_count_limit<<endl;
        switch_mode(dump_into_end_gate_wait_for_container, "dump_into_end_gate_wait_for_container");
      }
      if(pack_present)
      {
        switch_mode(dump_into_container, "dump_into_container");
      }
      break;
    case dump_into_end_gate_wait_for_container:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->open();
        endgate_p->close();
      }
      if(envelope_sensor_p->read_rise())
      {
        switch_mode(dump_into_container, "dump_into_container");
      }
      break;    
    case dump_into_container:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->open();
        endgate_p->open();
        old_count = centre_p->count;
        dump_end_qtime.restart();
        send_status_message("Dumping", QColor(0,0,0), QColor(0,255,0), 25);
      }
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->count != old_count)
      {
        old_count = centre_p->count;
        dump_end_qtime.restart();
      }
      if(dump_end_qtime.elapsed() > dump_end_qtime_limit)
      {
        switch_mode(wait_for_dump_container_removal, "wait_for_dump_container_removal");
      }
      break;
    case wait_for_dump_container_removal:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->open();
        endgate_p->open();
        dump_end_qtime.restart();
      }
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(pack_present == false) 
      {
        if(use_spreadsheet)
        {
          if(  (ss_dump_count_p != 0)  &&  (end_valve_spreadsheet_line_number>=0)  )
          {
            ss_dump_count_p -> data_list[end_valve_spreadsheet_line_number] = QString::number(centre_p->count);
          }
        }
        refresh_screen();
        if(substitute_seed_lot)
        {
          emit substitution_barcode_entered(substitute_barcode);
        }
        emit dump_complete(centre_p->count);
        centre_p->dump_count_str = QString::number(centre_p->count);
        centre_p->communicate_out('d');
        centre_p->count = 0;
        count_rate = 0;//we do not want the rate established during dumping.  Will be re-set quickly on hi speed.
        hi_rate = 0;
        slowdown_count_diff = 0;
        stop_count_diff = 0;
        
        if(dump_automatically == true)
        {
          switch_mode(entry, "entry");
        }
        else
        {
          emit slave_mode_set_finished();
          switch_mode(wait_for_slave_mode_command_empty, "wait_for_slave_mode_command_empty");
        }
      }
      break;
    case repeat_wait_for_container:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->open();
        endgate_p->close();
        first_repeat_pack = false;
      }
      if(pack_present == true) 
      {
        emit enable_repeat_pack_button(false);
        first_repeat_pack = true;//reset for future use
        switch_mode(repeat_wait_for_container_removal, "repeat_wait_for_container_removal");
      }
      break;
    case repeat_wait_for_container_removal:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->open();
        endgate_p->open();
      }
      if(pack_present == false) 
      {
        centre_p->count = 0;
        count_rate = 0;//we do not want the rate established during dumping.  Will be re-set quickly on hi speed.
        hi_rate = 0;
        slowdown_count_diff = 0;
        stop_count_diff = 0;
        switch_mode(hi_o_c, "hi_o_c");
      }
      break;
    case substitute_wait_for_container:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->open();
        endgate_p->close();
        emit enable_substitute_button(false);
        substitute_seed_lot = true;
      }
      if(pack_present == true) 
      {
        switch_mode(substitute_wait_for_container_removal, "substitute_wait_for_container_removal");
      }
      break;
    case substitute_wait_for_container_removal:
      if(mode_new)
      {
        centre_p->set_speed(dump_speed);
        cutgate_p->open();
        endgate_p->open();
        old_count = centre_p->count;
        dump_end_qtime.restart();
      }
      if(centre_p->count != old_count)
      {
        old_count = centre_p->count;
        dump_end_qtime.restart();
      }
      if(dump_end_qtime.elapsed() > dump_end_qtime_limit)
      {
        if(use_spreadsheet)
        {
          if(ss_dump_count_p != 0)
          {
            ss_dump_count_p -> data_list[spreadsheet_line_number] = QString::number(centre_p->count);
          }
        }
        refresh_screen();
        centre_p->count = 0;
        count_rate = 0;//we do not want the rate established during dumping.  Will be re-set quickly on hi speed.
        hi_rate = 0;
        slowdown_count_diff = 0;
        stop_count_diff = 0;
        switch_mode(substitute_wait_for_substitute_barcode, "substitute_wait_for_substitute_barcode");
      }
      break;
    case substitute_wait_for_substitute_barcode:
      if(mode_new)
      {
        send_barcode_status_message("Scan Substitution Barcode", QColor(0,0,0), QColor(255,255,0), 25);
        barcode_mode = substitution;
        centre_p->set_speed(0);
        cutgate_p->open();
        endgate_p->close();
      }
      if(substitution_barcode_ok)
      {
        send_barcode_status_message("Substitute Barcode Scanned", QColor(0,0,0), QColor(0,255,0), 25);
        barcode_mode = pack;
        switch_mode(hi_o_c, "hi_o_c");
      }
      break;
    case wait_for_slave_mode_command_c_c:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->close();
      }
      if(pack_present == true)
      {
        switch_mode(wait_for_slave_mode_command_c_o, "wait_for_slave_mode_command_c_o");
      }
      break;
    case wait_for_slave_mode_command_c_o:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->open();
      }
      if(pack_present == false)
      {
        switch_mode(wait_for_slave_mode_command_empty, "wait_for_slave_mode_command_empty");
      }
      break;
    case wait_for_slave_mode_command_empty:
      if(mode_new)
      {
        centre_p->set_speed(0);
        cutgate_p->close();
        endgate_p->close();
      }
      emit slave_mode_program_finished();
      stop();
      break;

    default: cout<<"batch_mode_driver::run.  mode not found\n";
  }
      
  mode_new = false;
  
  //diagnostics
  ++ cout_counter;
  if(cout_counter >= cout_counter_limit)
  {
    cout_counter = 0;
//    cout<<"  cout_counter mode = "<<mode<<endl;
  }
}

void batch_mode_driver::set_high_feed_speed(int speed_s) 
{
  if(speed_s <= 500) speed_s = speed_s/2;         //~~~piece-wise linear, 2 parts~~~//
  else speed_s = 1.5*speed_s-500;                 //
  
  high_feed_speed = speed_s;
  centre_p->crops[0].high_feed_speed = speed_s;
}

void batch_mode_driver::set_low_feed_speed(int speed_s) 
{
  if(speed_s <= 500) speed_s = speed_s/2;         //~~~piece-wise linear, 2 parts~~~//
  else speed_s = 1.5*speed_s-500;                 //
  
  low_feed_speed = speed_s;
  centre_p->crops[0].low_feed_speed = speed_s;
}

void batch_mode_driver::set_dump_feed_speed(int speed_s) 
{
  if(speed_s <= 500) speed_s = speed_s/2;         //~~~piece-wise linear, 2 parts~~~//
  else speed_s = 1.5*speed_s-500;                 //
  
  dump_speed = speed_s;
  centre_p->crops[0].dump_speed = speed_s;
}

void batch_mode_driver::barcode_entered(QString value)
{
  QString value_trimmed = value.trimmed();//remove whitespace at start and end
  barcode_as_entered = value_trimmed;
  if(barcode_mode == seed_lot)
  {
    seed_lot_barcode_ok = true;
    seed_lot_barcode_old = false;
    seed_lot_barcode = value_trimmed;
    emit seed_lot_barcode_entered(value_trimmed);
  }
  if(barcode_mode == substitution)
  {
    substitution_barcode_ok = true;
    seed_lot_barcode_old = false;
    substitute_barcode = value_trimmed;
  }
  if(barcode_mode == pack)
  {
    pack_barcode = value_trimmed;
    pack_barcode_old = false;
    pack_barcode_ok_message_posted = false;
    if(require_pack_barcode == false)
    {
      pack_barcode_ok = true;
    }
    if(require_pack_barcode == true)
    {
      if(pack_match_lot == true)
      {
        if(pack_barcode==seed_lot_barcode) 
        {
          pack_barcode_ok = true;
        }
        else
        {
          pack_barcode_wrong = true;
        }
      }
      if(pack_contain_lot == true)
      {
        if(pack_barcode.contains(seed_lot_barcode)==true) 
        {
          pack_barcode_ok = true;
        }
        else
        {
          pack_barcode_wrong = true;
        }
      }
      if(lot_contain_pack == true)
      {
        if(seed_lot_barcode.contains(pack_barcode)==true) 
        {
          pack_barcode_ok = true;
        }
        else
        {
          pack_barcode_wrong = true;
        }
      }
      
      if(pack_match_spreadsheet == true)
      {
        spreadsheet_pack_barcode  = ss_envelope_id_p->data_list[end_valve_spreadsheet_line_number];
        if(pack_barcode.trimmed() == spreadsheet_pack_barcode.trimmed())
        {
          pack_barcode_ok = true;
        }
        else
        {
          pack_barcode_wrong = true;
        }
      }
      
      if(record_only == true)
      {
        if(pack_barcode_old == false)
        {
          pack_barcode_ok = true;
        }
      }
    }
    emit pack_barcode_entered(value_trimmed);
    if(substitute_seed_lot)
    {
      if(use_spreadsheet)
      {
        ss_substitution_p -> data_list[end_valve_spreadsheet_line_number] = substitute_barcode;
      }
      else
      {
        emit substitution_barcode_entered(substitute_barcode);
      }
    }
  }
  if(barcode_mode == substitution)
  {
    seed_lot_barcode_ok = true;
    seed_lot_barcode_old = false;
    substitute_barcode = value_trimmed;
    centre_p->substitution_str = substitute_barcode;
    centre_p->lotcode_str = substitute_barcode;
  }
}

spreadsheet_column* batch_mode_driver::get_spreadsheet_column_pointer(int column_number)
{
  if(column_number<0)//-1 used to indicate invalid data.
  {
    return 0;
  }
  spreadsheet_column* column_p = ss_first_column_p;
  for(int i=0; i<column_number; ++i)
  {
    if(column_p == 0)
    {
      return 0;//invalid column number
    }
    column_p = column_p->next;
  }
  return(column_p);
}

spreadsheet_column* batch_mode_driver::get_spreadsheet_column_pointer(QString column_heading)
{
  spreadsheet_column* column_p = ss_first_column_p;
  while(column_p)
  {
    if(column_p->heading == column_heading)
    {
      return column_p;
    }
    column_p = column_p->next;
  }
  cout<<"get_spreadsheet_column_pointer failed\n";
  return 0;//failed to find
}

void batch_mode_driver::clear_ss_setup()
{
  ss_setup_p -> material_id_column = -1;
  ss_setup_p -> required_count_column = -1;
  ss_setup_p -> envelope_id_column = -1;
  ss_setup_p -> actual_count_column = -1;
  ss_setup_p -> print_time_column = -1;
  ss_setup_p -> fill_time_column = -1;
  ss_setup_p -> dump_count_column = -1;
  ss_setup_p -> substitution_column = -1;
}

void batch_mode_driver::fill_ss_column_pointers()
{
  ss_material_id_p = get_spreadsheet_column_pointer(ss_setup_p -> material_id_column);
  ss_required_count_p = get_spreadsheet_column_pointer(ss_setup_p -> required_count_column);
  ss_envelope_id_p = get_spreadsheet_column_pointer(ss_setup_p -> envelope_id_column);
  ss_actual_count_p = get_spreadsheet_column_pointer(ss_setup_p -> actual_count_column);
  ss_print_time_p = get_spreadsheet_column_pointer(ss_setup_p -> print_time_column);
  ss_fill_time_p = get_spreadsheet_column_pointer(ss_setup_p -> fill_time_column);
  ss_dump_count_p = get_spreadsheet_column_pointer(ss_setup_p -> dump_count_column);
  ss_substitution_p = get_spreadsheet_column_pointer(ss_setup_p -> substitution_column);
}

void batch_mode_driver::save_ss_setup(QString filename)
{
  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    cout<<"batch_mode_driver::save_ss_setup.  File open failure\n";
    return;
  }
  QTextStream stream(&file);
  
  stream<<"material_id_column\n";
  stream<<ss_setup_p->material_id_column<<endl;
  stream<<"required_count_column\n";
  stream<<ss_setup_p->required_count_column<<endl;
  stream<<"envelope_id_column\n";
  stream<<ss_setup_p->envelope_id_column<<endl;
  stream<<"actual_count_column\n";
  stream<<ss_setup_p->actual_count_column<<endl;
  stream<<"print_time_column\n";
  stream<<ss_setup_p->print_time_column<<endl;
  stream<<"fill_time_column\n";
  stream<<ss_setup_p->fill_time_column<<endl;
  stream<<"dump_count_column\n";
  stream<<ss_setup_p->dump_count_column<<endl;
  stream<<"substitution_column\n";
  stream<<ss_setup_p->substitution_column<<endl;

  stream<<"high_feed_speed\n";
  stream<<high_feed_speed<<endl;
  stream<<"low_feed_speed\n";
  stream<<low_feed_speed<<endl;
  stream<<"dump_speed\n";
  stream<<dump_speed<<endl;
  stream<<"require_seed_lot_barcode\n";
  stream<<require_seed_lot_barcode<<endl;
  stream<<"require_pack_barcode\n";
  stream<<require_pack_barcode<<endl;
  stream<<"pack_match_lot\n";
  stream<<pack_match_lot<<endl;
  stream<<"pack_contain_lot\n";
  stream<<pack_contain_lot<<endl;
  stream<<"lot_contain_pack\n";
  stream<<lot_contain_pack<<endl;
  stream<<"pack_match_spreadsheet\n";
  stream<<pack_match_spreadsheet<<endl;
  stream<<"record_only\n";
  stream<<record_only<<endl;
  for(int i=0; i<display_column_numbers.size(); ++i)
  {
    stream<<"display_column\n";
    stream<<display_column_numbers[i]<<endl;
  }

  stream<<"print_envelope\n";
  stream<<print_envelope<<endl;
  stream<<"print_control_mode\n";
  stream<<print_control_mode<<endl;
  if(envelope_p)
  {
    stream<<"envelope_p->get_width\n";
    stream<<envelope_p->get_width()<<endl;
    stream<<"envelope_p->get_height\n";
    stream<<envelope_p->get_height()<<endl;
    for(int i=0; i<envelope_p->field_list.size(); ++i)
    {
      if(envelope_p->field_list[i].data_source_flag == 'd')
      {
        stream<<"field heading for data\n";
        stream<<envelope_p->field_list[i].column_p->heading<<endl;
        stream<<"field type\n";
        stream<<envelope_p->field_list[i].type<<endl;
        stream<<"field x\n";
        stream<<envelope_p->field_list[i].x<<endl;
        stream<<"field y\n";
        stream<<envelope_p->field_list[i].y<<endl;
        stream<<"field h\n";
        stream<<envelope_p->field_list[i].h<<endl;
      }
      if(envelope_p->field_list[i].data_source_flag == 'h')
      {
        stream<<"field heading for heading\n";
        stream<<envelope_p->field_list[i].column_p->heading<<endl;
        stream<<"field type\n";
        stream<<envelope_p->field_list[i].type<<endl;
        stream<<"field x\n";
        stream<<envelope_p->field_list[i].x<<endl;
        stream<<"field y\n";
        stream<<envelope_p->field_list[i].y<<endl;
        stream<<"field h\n";
        stream<<envelope_p->field_list[i].h<<endl;
      }
      if(envelope_p->field_list[i].data_source_flag == 't')
      {
        stream<<"field for text\n";
        stream<<envelope_p->field_list[i].text<<endl;
        stream<<"field type\n";
        stream<<envelope_p->field_list[i].type<<endl;
        stream<<"field x\n";
        stream<<envelope_p->field_list[i].x<<endl;
        stream<<"field y\n";
        stream<<envelope_p->field_list[i].y<<endl;
        stream<<"field h\n";
        stream<<envelope_p->field_list[i].h<<endl;
      }
    }
  }
  
  file.close();
}

envelope_field_type envelope_field_type_convert(int val)
{
  if (val==0) return Ubuntu_mono;
  if (val==1) return code_39;
  return none;
}

void batch_mode_driver::load_ss_setup()//load the ss_setup indicated by ss_setup_path
{
  QFile infile(ss_setup_path);
  infile.open(QIODevice::ReadOnly);
  QTextStream stream(&infile);
  QString line;
  QString subline;
  bool conversion_ok_flag = true;
  clear_ss_setup();
  while(stream.readLineInto(&line))
  {
    if(line == "material_id_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->material_id_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "required_count_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->required_count_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "envelope_id_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->envelope_id_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "actual_count_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->actual_count_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "print_time_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->print_time_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "fill_time_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->fill_time_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "dump_count_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->dump_count_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "substitution_column")
    {
      stream.readLineInto(&subline);
      ss_setup_p->substitution_column = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "high_feed_speed")
    {
      stream.readLineInto(&subline);
      high_feed_speed = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "low_feed_speed")
    {
      stream.readLineInto(&subline);
      low_feed_speed = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "dump_speed")
    {
      stream.readLineInto(&subline);
      dump_speed = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "require_seed_lot_barcode")
    {
      stream.readLineInto(&subline);
      require_seed_lot_barcode = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "require_pack_barcode")
    {
      stream.readLineInto(&subline);
      require_pack_barcode = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "pack_match_lot")
    {
      stream.readLineInto(&subline);
      pack_match_lot = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "pack_contain_lot")
    {
      stream.readLineInto(&subline);
      pack_contain_lot = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "lot_contain_pack")
    {
      stream.readLineInto(&subline);
      lot_contain_pack = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "pack_match_spreadsheet")
    {
      stream.readLineInto(&subline);
      pack_match_spreadsheet = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "record_only")
    {
      stream.readLineInto(&subline);
      record_only = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "display_column")
    {
      stream.readLineInto(&subline);
      display_column_numbers.append(subline.toInt(&conversion_ok_flag));
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "print_envelope")
    {
      stream.readLineInto(&subline);
      print_envelope = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "print_control_mode")
    {
      stream.readLineInto(&subline);
      int val = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
      
      print_control_mode = static_cast<print_control_mode_e>(val);
      
    }
    else if(line == "envelope_p->get_width")
    {
      stream.readLineInto(&subline);
      envelope_p->set_size(subline.toInt(&conversion_ok_flag), envelope_p->get_height());
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "envelope_p->get_height")
    {
      stream.readLineInto(&subline);
      envelope_p->set_size(envelope_p->get_width(), subline.toInt(&conversion_ok_flag));
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "field heading for data")
    {
      stream.readLineInto(&subline);
      envelope_field field;
      field.column_p = get_spreadsheet_column_pointer(subline);
      field.data_source_flag = 'd';
      envelope_p->field_list.append(field);
    }
    else if(line == "field heading for heading")
    {
      stream.readLineInto(&subline);
      envelope_field field;
      field.column_p = get_spreadsheet_column_pointer(subline);
      field.data_source_flag = 'h';
      envelope_p->field_list.append(field);
    }
    else if(line == "field for text")
    {
      stream.readLineInto(&subline);
      envelope_field field;
      field.text = subline;
      field.data_source_flag = 't';
      envelope_p->field_list.append(field);
    }
    else if(line == "field type")
    {
      stream.readLineInto(&subline);
      envelope_p->field_list[envelope_p->field_list.size()-1].type = envelope_field_type_convert(subline.toInt(&conversion_ok_flag));
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "field x")
    {
      stream.readLineInto(&subline);
      envelope_p->field_list[envelope_p->field_list.size()-1].x = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "field y")
    {
      stream.readLineInto(&subline);
      envelope_p->field_list[envelope_p->field_list.size()-1].y = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }
    else if(line == "field h")
    {
      stream.readLineInto(&subline);
      envelope_p->field_list[envelope_p->field_list.size()-1].h = subline.toInt(&conversion_ok_flag);
      if(conversion_ok_flag==false)
      {
        cout<<"failed to convert   "<<subline.toStdString()<<"   to int\n";
        return;
      }
    }

    else
    {
      cout<<"batch_mode_driver::load_ss_setup.  data type identifier not found.  "<<line.toStdString()<<"\n";
    }
  }
}

int batch_mode_driver::get_next_spreadsheet_line_number()//look for next line number not filled for current seed_lot_barcode.  Return -1 if no more.
//NOTE: also resets value of lines_left_to_fill
{
  int ret_val = -1;
  lines_left_to_fill = 0;
  for(int i=0; i<spreadsheet_number_of_lines; ++i)
  {
    if(   (ss_material_id_p->data_list[i] == seed_lot_barcode)     &&     (ss_first_column_p->data_list[i]=="N")     )
    {
      if(ret_val == -1) ret_val = i;
      ++lines_left_to_fill;
    }
  }
  return(ret_val);
}

int batch_mode_driver::get_spreadsheet_line_number_after(int val)//look for line number not filled for current seed_lot_barcode, following val.  Return -1 if no more.
{
  if ( (val<0) || (val>=spreadsheet_number_of_lines) ) return(-1);//-1 used to signal no more lines
  for(int i=val+1; i<spreadsheet_number_of_lines; ++i)
  {
    if(   (ss_material_id_p->data_list[i] == seed_lot_barcode)     &&     (ss_first_column_p->data_list[i]=="N")     )
    {
      return(i);
    }
  }
  return(-1);
}

void batch_mode_driver::chamber_count_limit_calculation() //2021_03_19
{
  //Chamber Volumes
  double lower_chamber_volume = 787*0.8;  //endgate to cutgate - approximated using CAD model (units are cm^3) 
  
  //Reference Variables (corn)
  double measured_seed_volume = 0.79650 * 0.49395 * 1.23505;        //(measured corn in centimetres)
  double virtual_seed_volume = qPow(1353, 1.5);                     //area_mean (pixel^3)
  double pixel_to_cm3 = measured_seed_volume / virtual_seed_volume; //conversion ratio
  double cm3_to_pixel = virtual_seed_volume/measured_seed_volume;   //
  
  //Selected Seed Volume
  double selected_average_seed_volume = qPow(centre_p->crops[0].area_mean, 1.5);  //selection dependent
  
  //Calculated From Selected Seed
  double lower_chamber_seed_limit = (lower_chamber_volume * cm3_to_pixel) / selected_average_seed_volume;
  lower_chamber_count_limit = lower_chamber_seed_limit*.5; //
  upper_chamber_count_limit = lower_chamber_count_limit;
  
//------------------------------------------------------------------------------------------------------//  
  cout<<"\t\t measured_seed_volume: "<<measured_seed_volume<<endl;
  cout<<"\t\t virtuial_seed_volume: "<<virtual_seed_volume<<endl;
  cout<<"\t\t pixel_to_cm3: "<<pixel_to_cm3<<endl;
  cout<<"\t\t selected_average_seed_volume: "<<selected_average_seed_volume<<endl;
  cout<<"\t\t lower_chamber_count_limit: "<<lower_chamber_count_limit<<endl;
  cout<<"\t\t upper_chamber_count_limit: "<<upper_chamber_count_limit<<endl;
}

void batch_mode_driver::repeat_pack()
{
  if(first_repeat_pack)
  {
    if(  (cutgate_set==0)  &&  (cutgate_pack==0)  )//program was reset.  go back to pack before end
    {
      cutgate_set = program_size-1;
      cutgate_pack = program[cutgate_set]->packs - 2;
    }
    else
    {
      cutgate_pack -= 2;
    }
  }
  else
  {
    cutgate_pack -= 1;
  }
  if(cutgate_pack<0)
  {
    cutgate_set--;
    if(cutgate_set<0)
    {
      cutgate_set = 0;
      cutgate_pack = 0;
    }
    else
    {
      cutgate_pack = (program[cutgate_set]->packs) - 1;
    }
  }
  
  cutgate_count_limit = program[cutgate_set]->seeds;
  cutgate_pack_limit = program[cutgate_set]->packs;
  
  QString message=QString("Program has been stepped back.\nClean out material in endgate.\nNext pack will repeat:\n  Pack %1 of %2:  %3 seeds.\nTo go back farther click again.") 
    .arg(cutgate_pack+1)
    .arg(cutgate_pack_limit)
    .arg(cutgate_count_limit);
  send_status_message(message, QColor(0,0,0), QColor(255, 255, 0), 16);
  send_barcode_status_message("", QColor(0,0,0), QColor(0,255,0), 16); 
  switch_mode(repeat_wait_for_container, "repeat_wait_for_container"); 
}

void batch_mode_driver::seed_lot_substitution()
{
  QString message;
  if(use_spreadsheet)
  {
    message=QString("Substituting seed lot.\nClean out material in endgate.\nFirst substituted pack will be:\n  Pack %1:  %2 seeds.") 
      .arg(ss_envelope_id_p->data_list[spreadsheet_line_number])
      .arg(cutgate_count_limit);
  }
  else
  {
    message=QString("Substituting seed lot.\nClean out material in endgate.\nFirst substituted pack will be:\n  Pack %1 of %2:  %3 seeds.") 
      .arg(cutgate_pack+1)
      .arg(cutgate_pack_limit)
      .arg(cutgate_count_limit);
  }
  send_status_message(message, QColor(0,0,0), QColor(255, 255, 0), 16);
  send_barcode_status_message("", QColor(0,0,0), QColor(0,255,0), 16); 
  switch_mode(substitute_wait_for_container, "substitute_wait_for_container"); 
  barcode_mode = substitution;
}

void batch_mode_driver::new_slave_mode_command()
{
  new_slave_mode_command_bool = true;
}

void batch_mode_driver::cutgate_closed_while_opening()
{
  QMessageBox box;
  box.setText("WARNING - Cutgate was closed while opening.  This can result in bad counts.  Probably the feed speed is too high.");
  box.exec();
}
  
