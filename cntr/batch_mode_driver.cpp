#include "batch_mode_driver.hpp"
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <iomanip>
#include <QMessageBox>

using namespace std;

batch_mode_driver::batch_mode_driver(centre* centre_p_s)
{
  centre_p = centre_p_s;
  count_rate_predictor_p = new count_rate_predictor(centre_p);
  pack_present = old_pack_present = false;
  pack_changed = true;
  timer_p = new QTimer;
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  mode = wait_for_seed_lot_barcode;
  old_count = 0;
  endgate_close_counter = 0;
  program_size = 0;
  
  //batch options
  require_seed_lot_barcode = true;
  require_pack_barcode = true;
  pack_match_lot = true;
  pack_contain_lot = false;
  lot_contain_pack = false;
  record_only = false;
  
  barcode_mode = seed_lot;
  seed_lot_barcode_old = true;//need a new scan
  pack_barcode_old = true;//need a new scan
  seed_lot_barcode_ok = false;
  pack_barcode_ok = false;
  
  dump_into_cut_gate_time.start();
  dump_into_end_time.start();
  dump_end_qtime.start();
  
  force_dump_out = false;
  
//  high_feed_speed = centre_p->crops[0].high_feed_speed;
//  low_feed_speed = centre_p->crops[0].low_feed_speed;
//  dump_speed = centre_p->crops[0].dump_speed;



//  print_message_count = 0;
}
  
batch_mode_driver::~batch_mode_driver()
{
  delete count_rate_predictor_p;
  count_rate_predictor_p = 0;
  for(int i=0; i<program_size; ++i) delete program[i];
  program.clear();
  timer_p -> stop();
  delete timer_p;
  if(set_p) delete set_p;
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
      cout<<"batch_mode_driver::load_file.  data type identifier not found.\n";
    }
  }
  current_set = 0;
  current_pack = 0;
  current_count_limit = program[current_set]->seeds;
  current_pack_limit = program[current_set]->packs;
//  cout<<"\n\nbm_program\n";
//  for(int i=0; i<program_size; ++i)
//  {
//    cout<<"packs "<<program[i]->packs<<"      seeds "<<program[i]->seeds<<endl;
//  }
//  cout<<endl;  
}

void batch_mode_driver::reset_program()
{
  if(program_size == 0) return;
  current_set = 0;
  current_pack = 0;
  current_count_limit = program[current_set]->seeds;
  current_pack_limit = program[current_set]->packs;
  mode = wait_for_seed_lot_barcode;
  barcode_mode = seed_lot;
  dump_end_qtime.restart();
//  cout<<"\n\nbatch_mode_driver::reset_program\n";
//  for(int i=0; i<program_size; ++i)
//  {
//    cout<<"packs "<<program[i]->packs<<"      seeds "<<program[i]->seeds<<endl;
//  }
//  cout<<endl;  
}  

void batch_mode_driver::clear_program()
{
  for(int i=0; i<program_size; ++i)
  {
    delete program[i];
  }
  program.clear();
  program_size = 0;
  
//  program_path = "";
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
//  cout<<"start batch_mode_driver::program_is_empty\n";
  if(program_size==0)
  {
//    cout<<"batch_mode_driver::program_is_empty returning true\n";
    return true;
  }
  else
  {
//    cout<<"batch_mode_driver::program_is_empty returning false\n";    
    return false;
  }
}

int batch_mode_driver::get_program_size()
{
  cout<<"batch_mode_driver::program_size.  returning "<<program_size<<endl;
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
  timer_p->start(10);
}

void batch_mode_driver::stop()
{
  timer_p->stop();
}

void batch_mode_driver::restart()
{
  dump_into_end_time.restart();
  mode = dump_into_end;
  cout<<"mode dump_into_end\n";
  pack_barcode_ok = true;
  pack_barcode_old = true;
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
  stream<<"record_only\n";
  stream<<record_only<<endl;
}
















void batch_mode_driver::list_program()
{
  cout<<"\nbatch_mode_driver::list_program\n";
//  if(program.isEmpty())
//  {
//    cout<<"  program is empty\n";
//    return;
//  }
  
  cout<<"program_size "<<program_size<<endl;
  
  for (int i=0; i<program_size; ++i)
  {
    cout<<program[i]->packs<<" packs, "<<program[i]->seeds<<" seeds each.\n" ;
  }
  cout<<endl;
  
  cout<<"end list_program\n";
}

void batch_mode_driver::run()
{
  float predicted_count_rate = (count_rate_predictor_p->get_rate());
  float time_to_end;
  if(predicted_count_rate > .001)
  {
    time_to_end = float( current_count_limit - (centre_p->count) )  /  predicted_count_rate;
  }
  else
  {
    time_to_end = 1000;
  }
  
  
  
  /*
  //testing
  if(print_message_count>=100)
  {
    cout<<"count "<<centre_p->count<<".  time_to_end "<<time_to_end<<".  predicted count rate "<<predicted_count_rate<<endl;
    emit send_message_time_to_end(QString("time_to_end %1.  predicted count rate %2\n").arg(time_to_end).arg(predicted_count_rate));
    print_message_count = 0;
  }
  ++print_message_count;
  */
  
  
  
  old_pack_present = pack_present;
  pack_present = centre_p->envelope_present;
/*
  if(centre_p->endgate_state == ENDGATE_CLOSED)
  {
    if(force_endgate_open==true || pack_present==true)
    {
      centre_p -> set_endgate_state(ENDGATE_OPEN);
    }
  }
  else //endgate is open
  {
    if(force_endgate_open==false && pack_present==false)
    {
      centre_p -> set_endgate_state(ENDGATE_CLOSED);
    }
  }
  */
  //barcode checking
  
//  seed_lot_barcode_ok = false;
  if(require_seed_lot_barcode == false)
  {
    seed_lot_barcode_ok = true;
  }
  if(require_pack_barcode == false)
  {
    pack_barcode_ok = true;
  }
  
  /*
  if(require_seed_lot_barcode == true)
  {
    if(seed_lot_barcode_old == false)
    {
      seed_lot_barcode_ok = true;
    }
  }
  
  if(require_pack_barcode == false)
  {
    pack_barcode_ok = true;
  }
  if(require_pack_barcode == true)
  {
    if(pack_match_lot == true)
    {
      if( (pack_barcode_old==false) && (pack_barcode==seed_lot_barcode) )
      {
        pack_barcode_old = true;
        pack_barcode_ok = true;
//        cout<<"set pack_barcode_ok true.  pack_barcode="<<pack_barcode.toStdString()<<".  seed_barcode="<<seed_lot_barcode.toStdString()<<endl;
      }
    }
    if(pack_contain_lot == true)
    {
      if( (pack_barcode_old==false) && (pack_barcode.contains(seed_lot_barcode)==true) )
      {
        pack_barcode_old = true;
        pack_barcode_ok = true;
      }
    }
    if(lot_contain_pack == true)
    {
      if( (pack_barcode_old==false) && (seed_lot_barcode.contains(pack_barcode)==true) )
      {
        pack_barcode_old = true;
        pack_barcode_ok = true;
      }
    }
  }
  */

  //testing
//  int dump_elapsed;
  QString message2;

  message2 = QString("mode = %1").arg(mode);
  send_message2(message2);

  if(force_dump_out)
  {
    mode = dump_into_end;
    dump_into_end_time.restart();
    cout<<"mode dump_into_end\n";
    force_dump_out = false;
  }

  switch(mode)
  {
    case wait_for_seed_lot_barcode:
      barcode_mode = seed_lot;
      if(centre_p->feed_speed != 0)
      {
        centre_p->set_speed(0);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if(seed_lot_barcode_ok == true)
      {
        
        mode = hi_open;
        cout<<"mode hi_open\n";
      }
      break;
    case hi_open:
      barcode_mode = pack;
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      else 
      {
        if(centre_p->feed_speed != high_feed_speed)
        {
          centre_p->set_speed(high_feed_speed);
        }
        centre_p->block_endgate_opening = !pack_barcode_ok;
        if(time_to_end<0.4)
        {
          mode = low_open;
          cout<<"mode low_open. count "<<centre_p->count<<"\n";
        }
      }
      break;
    case low_open:
      barcode_mode = pack;
      if(centre_p->feed_speed != low_feed_speed)
      {
        centre_p->set_speed(low_feed_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if(centre_p->count >= current_count_limit)
      {
        centre_p->count = 0;
        mode = gate_delay;
        cout<<"mode gate_delay. count "<<centre_p->count<<"\n";
        cutoff_gate_close_time.restart();
      }
      break;
    case gate_delay:
      barcode_mode = pack;
      if(centre_p->feed_speed != low_feed_speed)
      {
        centre_p->set_speed(low_feed_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if(cutoff_gate_close_time.elapsed() >= cutoff_gate_delay_time)
      {
        centre_p->set_cutgate_state(CUTGATE_CLOSED);

        //remember current numbers for display purposes
        pack_ready_pack = current_pack;
        pack_ready_count_limit = current_count_limit;
        pack_ready_pack_limit = current_pack_limit;
          
        emit pack_ready();
        ++current_pack;
  
//        cout<<"current_pack = "<<current_pack<<"   current_pack_limit = "<<current_pack_limit<<endl;
  
        if(current_pack >= current_pack_limit)
        {
          current_pack = 0;
          ++current_set;
  
//          cout<<"current_set = "<<current_set<<"  program.size() = "<<program.size()<<endl;
  
          if(current_set >= program_size)
          {
            mode = dump_into_cut_gate;
            cout<<"mode dump_into_cut_gate. count "<<centre_p->count<<"\n";
            dump_into_cut_gate_time.restart();
            old_count = centre_p -> count;
            dump_end_qtime.restart();
          }
          else
          {
            current_count_limit = program[current_set]->seeds;
            current_pack_limit = program[current_set]->packs;
            mode = hi_closed;
            cout<<"mode hi_closed. count "<<centre_p->count<<"\n";
          }
        }
        else
        {
          mode = hi_closed;
          cout<<"mode hi_closed. count "<<centre_p->count<<"\n";
        }
      }
      break;
    case hi_closed:
      barcode_mode = pack;
      if(centre_p->feed_speed != high_feed_speed)
      {
        centre_p->set_speed(high_feed_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_CLOSED)
      {        
        centre_p->set_cutgate_state(CUTGATE_CLOSED);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if( (old_pack_present==true) && (pack_present==false) && (pack_barcode_ok==true) )
      {
        emit pack_collected(pack_ready_count_limit);
        pack_barcode_ok = false;
        pack_barcode_old = true;
        mode = wait_for_endgate_to_close;
        cout<<"mode wait_for_endgate_to_close. count "<<centre_p->count<<"\n";
        endgate_close_counter = 0;
      }
      if(time_to_end<1)
      {
        mode = wait_for_pack;
        cout<<"mode wait_for_pack. count "<<centre_p->count<<"\n";
      }      
      break;
    case wait_for_endgate_to_close:
      barcode_mode = pack;
      if(centre_p->feed_speed != high_feed_speed)
      {
        centre_p->set_speed(high_feed_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_CLOSED)
      {        
        centre_p->set_cutgate_state(CUTGATE_CLOSED);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      ++endgate_close_counter;
      if(endgate_close_counter>=50)
      {
        mode = hi_open;
        cout<<"mode hi_open. count "<<centre_p->count<<"\n";
      }
      break;
    case wait_for_pack:
      barcode_mode = pack;
      if(centre_p->feed_speed != 0)
      {
        centre_p->set_speed(0);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_CLOSED)
      {        
        centre_p->set_cutgate_state(CUTGATE_CLOSED);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if( (old_pack_present==true) && (pack_present==false) && (pack_barcode_ok==true) )
      {
        emit pack_collected(pack_ready_count_limit);
        pack_barcode_ok = false;
        pack_barcode_old = true;
        mode = wait_for_endgate_to_close;
        cout<<"mode wait_for_endgate_to_close. count "<<centre_p->count<<"\n";
        endgate_close_counter = 0;
      }
      if(centre_p->count >= current_count_limit)//error.  Display message box.
      {
        QMessageBox msg_box;
        msg_box.setText("ERROR.  COUNT WENT OVER LIMIT.");
        msg_box.setInformativeText("This is probably because the speed is too high.  \
                                  Hopper will dump out so you can re-start         \
                                  this seed lot.");
        msg_box.exec();
        mode = dump_into_end;
        dump_into_end_time.restart();
        cout<<"mode dump_into_end\n";
      }
      break;
    case dump_into_cut_gate:
      barcode_mode = pack;
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_CLOSED)
      {        
        centre_p->set_cutgate_state(CUTGATE_CLOSED);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if( (old_pack_present==true) && (pack_present==false) && (pack_barcode_ok==true) )//final pack was collected
      {
        emit pack_collected(pack_ready_count_limit);
        pack_barcode_ok = false;
        pack_barcode_old = true;
        mode = dump_wait_for_endgate_to_close;
        endgate_close_counter = 0;
        cout<<"mode dump_wait_for_endgate_to_close\n";
        dump_into_end_time.restart();
      }
      if(dump_into_cut_gate_time.elapsed() >= dump_into_cut_gate_time_limit)
      {
        mode = wait_for_final_pack;
        cout<<"mode wait_for_final_pack\n";
      }
      break;
    case dump_wait_for_endgate_to_close:
      barcode_mode = pack;
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_CLOSED)
      {        
        centre_p->set_cutgate_state(CUTGATE_CLOSED);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      ++endgate_close_counter;
      if(endgate_close_counter>=50)      {
        mode = dump_into_end;
        cout<<"mode dump_into_end\n";
        emit dumping(true);        
      }
      break;
    case wait_for_final_pack:
      barcode_mode = pack;
      if(centre_p->feed_speed != 0)
      {
        centre_p->set_speed(0);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_CLOSED)
      {        
        centre_p->set_cutgate_state(CUTGATE_CLOSED);
      }
      centre_p->block_endgate_opening = !pack_barcode_ok;
      if( (old_pack_present==true) && (pack_present==false) && (pack_barcode_ok==true) )
      {
        emit pack_collected(pack_ready_count_limit);
        pack_barcode_ok = false;
        pack_barcode_old = true;
        mode = dump_wait_for_endgate_to_close;
        endgate_close_counter = 0;
        cout<<"mode dump_wait_for_endgate_to_close\n";
        dump_into_end_time.restart();
      }
      break;
    case dump_into_end:
      barcode_mode = seed_lot;
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = false;
      if( pack_present==true )
      {
        mode = dump_into_container;
        cout<<"mode dump_into_container\n";
      }
      if(dump_into_end_time.elapsed() >= dump_into_end_time_limit)
      {
        mode = wait_for_dump_container;
        cout<<"mode wait_for_dump_container\n";
      }
      break;
    case wait_for_dump_container:
      barcode_mode = seed_lot;
      if(centre_p->feed_speed != 0)
      {
        centre_p->set_speed(0);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = false;
      if(pack_present == true)
      {
        mode = dump_into_container;
        cout<<"mode dump_into_container\n";
      }
      break;
    case dump_into_container:
      barcode_mode = seed_lot;
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = false;
      
//      dump_elapsed = dump_end_qtime.elapsed();
//      cout<<"dump_end_qtime.elapsed = "<<dump_elapsed<<endl;
      
      if(dump_end_qtime.elapsed() >= dump_end_qtime_limit)//if count has not changed, dump is complete
      {
        dump_end_qtime.restart();
        if(centre_p->count == old_count)
        {
          emit dump_complete(old_count);
          centre_p -> count = 0;
          mode = wait_for_dump_container_removal;
          cout<<"mode wait_for_dump_container_removal\n";
        }
        old_count = centre_p->count;
      }      
      break;
    case wait_for_dump_container_removal:
      barcode_mode = seed_lot;
      if(centre_p->feed_speed != dump_speed)
      {
        centre_p->set_speed(dump_speed);
      }
      if(centre_p->get_cutgate_state() !=  CUTGATE_OPEN)
      {        
        centre_p->set_cutgate_state(CUTGATE_OPEN);
      }
      centre_p->block_endgate_opening = false;
      if(centre_p->get_endgate_state()==ENDGATE_CLOSED)
      {
        mode = wait_for_seed_lot_barcode;
        cout<<"mode wait_for_seed_lot_barcode\n";
        seed_lot_barcode_ok = false;
        seed_lot_barcode_old = true;
        emit dumping(false);
        reset_program();
      }      
      break;
      
    default: cout<<"mode not found\n";
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
//  cout<<"start batch_mode_driver::barcode_entered\n";
//  cout<<"barcode_entered value "<<value.toStdString()<<endl;
  QString value_trimmed = value.trimmed();//remove whitespace at start and end
  if(barcode_mode == seed_lot)
  {
    seed_lot_barcode_ok = true;
    seed_lot_barcode_old = false;
    seed_lot_barcode = value_trimmed;
    emit seed_lot_barcode_entered(value_trimmed);
  }
  if(barcode_mode == pack)
  {
    pack_barcode = value_trimmed;
    pack_barcode_old = false;
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
  //        cout<<"set pack_barcode_ok true.  pack_barcode="<<pack_barcode.toStdString()<<".  seed_barcode="<<seed_lot_barcode.toStdString()<<endl;
        }
      }
      if(pack_contain_lot == true)
      {
//        cout<<"pack must contain lot\n";
//        cout<<"pack_barcode = "<<pack_barcode.toStdString()<<endl;
//        cout<<"seed_lot_barcode = "<<seed_lot_barcode.toStdString()<<endl;
        if(pack_barcode.contains(seed_lot_barcode)==true) 
        {
//          cout<<"pack contains lot\n";
          pack_barcode_ok = true;
        }
//        else
//        {
//          cout<<"pack does not contain lot\n";
//        }
      }
      if(lot_contain_pack == true)
      {
        if(seed_lot_barcode.contains(pack_barcode)==true) 
        {
          pack_barcode_ok = true;
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
  }
//  cout<<"end batch_mode_driver::barcode_entered\n";
}

void batch_mode_driver::cutgate_timing_error()
{
  centre_p -> set_speed(0);
  QMessageBox msg_box;
  msg_box.setText("CUT GATE TIMING ERROR.");
  msg_box.setInformativeText("This is probably because the speed is too high.\n  \
                            Hopper will dump out so you can re-start\n         \
                            this seed lot.");
  msg_box.exec();
  force_dump_out = true;
}


count_rate_predictor::count_rate_predictor(centre* centre_p_s)
{
  centre_p = centre_p_s;
  count_rate_multiplier = 1;
  old_count = 0;
  connect(&timer, SIGNAL(timeout()), this, SLOT(run()));
  timer.start(100);
}

count_rate_predictor::~count_rate_predictor()
{
  timer.stop();
}

void count_rate_predictor::run()
{
  float current_rate = (centre_p->count-old_count)*10.0;
  old_count = centre_p->count;
  if(current_rate>0 && centre_p->feed_speed>0)//if rate is 0, feeder is empty.  do not calculate.
  {
    float current_count_rate_multiplier = current_rate/float(centre_p->feed_speed);
//    cout<<"current_count_rate_multiplier "<<setw(15)<<current_count_rate_multiplier;
//    cout<<"    count_rate_multiplier "<<setw(15)<<count_rate_multiplier<<endl;
    count_rate_multiplier = (1.0-averaging_weight) * count_rate_multiplier   +   averaging_weight * current_count_rate_multiplier;
  }
  
  emit send_message(QString("count rate multiplier %1").arg(count_rate_multiplier));
}

float count_rate_predictor::get_rate()
{
  return (count_rate_multiplier * centre_p->feed_speed);
}

    
