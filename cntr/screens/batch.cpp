#include <iostream>
#include <fstream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include "centre.hpp"
#include "batch.hpp"
#include "button.hpp"
//#include "batch_mode_driver.hpp"
#include "message_box.hpp"


using namespace std;

void barcode_line::keyPressEvent(QKeyEvent* event)
{
//  cout<<"batch_table::keyPressEvent\n";
  QTextEdit::keyPressEvent(event);
  if(event->key() == Qt::Key_Return)
  {
//    cout<<"about to emit barcode_entered\n";
    emit barcode_entered(toPlainText());
    clear();
  }
}


batch::batch(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
 :screen(set_centre_p)
{
//  cout<<"batch::batch 1\n";
  batch_mode_driver_p = set_batch_mode_driver_p;

  cout<<"batch::batch 1.  listing program\n";  
  batch_mode_driver_p->list_program();


  
  if (centre_p->crops[0].calibrated == false) 
  {
    centre_p->add_waiting_screen(15);//back here to batch
    centre_p->add_waiting_screen(10);//calibrate
    centre_p->screen_done = true;
    return;
  }
  count_message_p = new QLabel;
  options_button_p = new button("Options");
  back_button_p = new button("Back");
  barcode_line_p = new barcode_line;
  barcode_line_p->setMaximumSize(20,10);
//  zero_button_p = new button("Zero");
//  endgate_button_p = new button("");
  restart_button_p = new button("Dump out and\nrestart seed lot");
  high_speed_label_p = new QLabel("High");
  high_speed_set_p = new QSlider;
  high_speed_set_p->setMinimum(0);
  high_speed_set_p->setMaximum(1000);
  high_speed_set_p->setOrientation(Qt::Horizontal);
  high_speed_set_p->setFocusPolicy(Qt::NoFocus);
  low_speed_label_p = new QLabel("Low");
  low_speed_set_p = new QSlider;
  low_speed_set_p->setMinimum(0);
  low_speed_set_p->setMaximum(100);
  low_speed_set_p->setOrientation(Qt::Horizontal);
  low_speed_set_p->setFocusPolicy(Qt::NoFocus);
  dump_speed_label_p = new QLabel("Dump");
  dump_speed_set_p = new QSlider;
  dump_speed_set_p->setMinimum(0);
  dump_speed_set_p->setMaximum(1000);
  dump_speed_set_p->setOrientation(Qt::Horizontal);
  dump_speed_set_p->setFocusPolicy(Qt::NoFocus);
  
  high_speed_set_p->setValue(batch_mode_driver_p->high_feed_speed);
  low_speed_set_p->setValue(batch_mode_driver_p->low_feed_speed);
  dump_speed_set_p->setValue(batch_mode_driver_p->dump_speed);
  
  table_p = new batch_table(centre_p, batch_mode_driver_p);
//  cout<<"before new message_box\n";
  status_box_p = new message_box;
//  cout<<"after new message_box\n";
  status_box_p -> setMinimumSize(250,100);
  save_program_button_p = new button("Save Program");
  save_table_button_p = new button("Save Table");
  clear_table_button_p = new button("Clear Table");
  quit_button_p = new button("Quit");
//  cout<<"before barcode_status\n";
  barcode_status_p = new message_box;
//  cout<<"after barcode_status\n";
  
  
  top_box_p = new QGroupBox;
  control_box_p = new QGroupBox;
  speed_box_p = new QGroupBox;
  speed_box_p -> setTitle("Speeds");
  bottom_box_p = new QGroupBox;
  
  top_layout_p = new QGridLayout;
  control_layout_p = new QGridLayout;
  speed_layout_p = new QGridLayout;
  bottom_layout_p = new QGridLayout;
  
  main_layout_p = new QGridLayout;

  top_layout_p -> addWidget(count_message_p, 0, 0);
  top_layout_p -> addWidget(options_button_p, 0, 1);
  top_layout_p -> addWidget(back_button_p, 0, 2);
  top_layout_p -> addWidget(barcode_line_p, 1, 0);
//  control_layout_p -> addWidget(zero_button_p, 0, 0);
//  control_layout_p -> addWidget(endgate_button_p, 1, 0);  //ORIGINAL~~~
  control_layout_p -> addWidget(restart_button_p, 0, 0);  //ORIGINAL~~~
  //control_layout_p -> addWidget(endgate_button_p, 0, 1);    //TEST~~~
  control_layout_p -> addWidget(speed_box_p, 1, 0);       //ORIGINAL~~~
  //control_layout_p -> addWidget(speed_box_p, 1, 0);         //TEST~~~
  speed_layout_p -> addWidget(high_speed_label_p, 0, 0);
  speed_layout_p -> addWidget(high_speed_set_p, 0, 1);
  speed_layout_p -> addWidget(low_speed_label_p, 1, 0);
  speed_layout_p -> addWidget(low_speed_set_p, 1, 1);
  speed_layout_p -> addWidget(dump_speed_label_p, 2, 0);
  speed_layout_p -> addWidget(dump_speed_set_p, 2, 1);
  bottom_layout_p -> addWidget(status_box_p, 0, 0, 2, 1);
  bottom_layout_p -> addWidget(save_program_button_p, 0, 2);
  bottom_layout_p -> addWidget(save_table_button_p, 0, 3);
  bottom_layout_p -> addWidget(clear_table_button_p, 0, 4);
  bottom_layout_p -> addWidget(quit_button_p, 0, 5);
  bottom_layout_p -> addWidget(barcode_status_p, 1, 2, 1, 3);
  main_layout_p -> addWidget(top_box_p, 0, 0, 1, 2);
  main_layout_p -> addWidget(control_box_p, 1, 0);
  main_layout_p -> addWidget(table_p, 1, 1);
  main_layout_p -> addWidget(bottom_box_p, 2, 0, 1, 2);
  
  top_box_p -> setLayout(top_layout_p);
  control_box_p -> setLayout(control_layout_p);
  speed_box_p -> setLayout(speed_layout_p);
  bottom_box_p -> setLayout(bottom_layout_p);
  setLayout(main_layout_p);
  
  connect(options_button_p, SIGNAL(clicked()), this, SLOT(options_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
//  connect(zero_button_p, SIGNAL(clicked()), this, SLOT(zero_clicked()));
//  connect(endgate_button_p, SIGNAL(clicked()), this, SLOT(endgate_clicked()));
  connect(restart_button_p, SIGNAL(clicked()), this, SLOT(restart_clicked()));
  connect(high_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_high_feed_speed(int)));
  connect(low_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_low_feed_speed(int)));
  connect(dump_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_dump_feed_speed(int)));
  connect(save_program_button_p, SIGNAL(clicked()), this, SLOT(save_program_clicked()));
  connect(save_table_button_p, SIGNAL(clicked()), this, SLOT(save_table_clicked()));
  connect(clear_table_button_p, SIGNAL(clicked()), this, SLOT(clear_table_clicked()));
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));
  connect(barcode_line_p, SIGNAL(barcode_entered(QString)), batch_mode_driver_p, SLOT(barcode_entered(QString)));
  connect(batch_mode_driver_p, SIGNAL(seed_lot_barcode_entered(QString)), table_p, SLOT(enter_seed_lot_barcode(QString)));
  connect(batch_mode_driver_p, SIGNAL(pack_barcode_entered(QString)), table_p, SLOT(enter_pack_barcode(QString)));
  connect(low_speed_set_p, SIGNAL(sliderReleased()), this, SLOT(focus_on_barcode()));

  centre_p->set_endgate_state(ENDGATE_CLOSED);
  if(centre_p->totalize_force_endgate_open == true)
  {
    screen_endgate=ENDGATE_OPEN;
  }
  else
  {
    screen_endgate=ENDGATE_CLOSED;
  }
//  endgate_button_p->setText("Open Endgate\nRelease Seed");
  
  centre_p->set_cutgate_state(CUTGATE_OPEN);
  
  feeder_is_running = true;
  
  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(100);
  
  count = 0;
//  pack_ready_count = -1;//negative number indicates new value has not been sent by batch_mode_driver
  batch_mode_driver_p->start();
  
  pack_was_placed = false;
  pack_was_removed = false;
  seed_for_pack_ready = false;
  pack_can_be_removed = false;
  pack_removed_too_soon = false;
  pack_ready_message = "Pack Ready";
  pack_removed_too_soon_message = "Pack Not Ready\nPut Back";
  dump_container_was_placed = false;
  dump_container_can_be_removed = false;
  dump_container_removed_too_soon = false;
  dump_container_needed_message = "Place Dump\nContainer";
  dump_container_ready_message = "Finished Dumping";
  dump_container_removed_too_soon_message = "Dumping Not\nFinished";
  dump_flag = false;
  old_barcode_mode = pack;
  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  connect(batch_mode_driver_p, SIGNAL(dumping(bool)), this, SLOT(dumping(bool)));
  connect(table_p, SIGNAL(focus_on_barcode()), this, SLOT(focus_on_barcode()));
  connect(batch_mode_driver_p, SIGNAL(pack_ready()), this, SLOT(pack_ready()));
  
//  high_speed_set_p->setValue(500);
//  low_speed_set_p->setValue(50);
//  dump_speed_set_p->setValue(1000);
  
//  barcode_line_p -> hide();
  
//  diagnostics_box_p = new message_box;
//  diagnostics_box_p -> setMinimumSize(250,100);
//  diagnostics_box_p -> move(900, 0);
//  diagnostics_box_p -> show();

  cout<<"batch::batch end.  listing program\n";  
  batch_mode_driver_p->list_program();

  table_p->load_file("settings/batch_backup");
  
  if(batch_mode_driver_p->bm_save_program_filename != "")//returning from batch_save_program screen with name of file to save
  {
    batch_mode_driver_p->save_program(batch_mode_driver_p->bm_save_program_filename);
//    batch_mode_driver_p->save_program(batch_mode_driver_p->bm_save_program_filename + "test");
    batch_mode_driver_p->bm_save_program_filename = "";
  }
  
  if(batch_mode_driver_p->bm_save_table_filename != "")//returning from batch_save_table_file screen with name of file to save
  {
    
    cout<<"batch::batch.  save file name = "<<batch_mode_driver_p->bm_save_table_filename.toStdString()<<endl;
    
    table_p->save_file(batch_mode_driver_p->bm_save_table_filename);
    batch_mode_driver_p->bm_save_table_filename = "";
  }
  
  barcode_line_p->setFocus();
//  batch_mode_driver_p->list_program();
  
  //TEST~~ Stylesheet//
  setStyleSheet(
    "button {"
        "border: 2px solid #8f8f91;"
        "height: 30px;"
        "font: 18px;"
        "border-radius: 12px;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "min-width: 80px;}"
                                  
    ".QSlider::groove:horizontal {"
        "border:none;"
        "margin-top: 10px;"
        "margin-bottom: 10px;"
        "height: 10px;}"
    
    ".QSlider::sub-page {"
        "background: #009900;}"

    ".QSlider::add-page {"
        "background: rgb(70, 70, 70);}"

    ".QSlider::handle {"
        "background: e1e8a7;"
        "border: 3px solid #8f8f91;"
        "width: 60px;"
        "margin: -30px 0;}"
        
    /*".QGridLayout {"
        "padding: 0px;"
        "margin: 0px;"
        "border: 0px;"
        "max-height: 480px;}"*/
  );
  
  top_layout_p->setContentsMargins(0,0,0,0);        //TEST~~~
  control_layout_p->setContentsMargins(0,0,0,0);    //
  speed_layout_p->setContentsMargins(0,0,0,0);      //
  bottom_layout_p->setContentsMargins(0,0,0,0);     //
  main_layout_p->setContentsMargins(0,0,0,0);       //
  
  this->setFixedSize(800,480);  //TEST~~~ works to adjust entire window
}

batch::~batch()
{
  run_timer_p -> stop();
  table_p->save_file(QString("settings/batch_backup"));
  batch_mode_driver_p -> stop();
  delete count_message_p;
  delete options_button_p;
  delete back_button_p;
//  delete zero_button_p;
//  delete endgate_button_p;
  delete restart_button_p;
  delete high_speed_label_p;
  delete high_speed_set_p;
  delete low_speed_label_p;
  delete low_speed_set_p;
  delete dump_speed_label_p;
  delete dump_speed_set_p;
  delete table_p;
  delete status_box_p;
//  delete save_button_p;
  delete clear_table_button_p;
  delete quit_button_p;
  delete top_layout_p;
  delete top_box_p;
  delete speed_box_p;
  delete bottom_box_p;
  delete control_layout_p;
  delete main_layout_p;
  
//  delete diagnostics_box_p;
//  diagnostics_box_p = 0;
}

void batch::pack_ready()
{
  seed_for_pack_ready = true;//will stay true until pack is removed
//  pack_ready_count = batch_mode_driver_p->current_count_limit;
}  

void batch::pack_collected(int count_to_record)
{
  
//  pack_was_removed = false;
  pack_can_be_removed = false;
  pack_removed_too_soon = false;
  pack_was_placed = false;
  table_p->enter_seeds(count_to_record);
  
//--------------------------------------------------------------------------------------------------------------//
    /*centre_p->count = count_to_record;	//TEST~~~

    gpio_keyboard_p = new gpio_keyboard;
    
    if(!(centre_p->tm_macro_updated))
    {
      centre_p->load_macros();
      cout<<"loading macros"<<endl;
	}
	else
	{
	  cout<<"macros already loaded"<<endl;
	}
	//gpio_keyboard_p -> gpio_send((centre_p->combined_macro_functions));	//original
	
	//macro_screen_p = new macro_screen;	//TEST~~~
	//macro_screen_p -> usb_serial_out();	//TEST~~~
	//macro_screen_p -> usb_serial_out(centre_p->combined_macro_functions);	//TEST~~~
  //load totalize table variables//
    //int current_totalize_table_row;    
    //int barcode_1, barcode_2, barcode_3, barcode_4, totalize_count, weight;

  //int k;
  //int l;
  //int m;
    
  //k = table_p -> model_row;
  //l = table_p -> model_p -> item(k-1, 0) -> data().toInt();
  //m = table_p -> model_p -> item(k-1, 0) -> text().toInt();
  
  int row;
  int lotcode;
  int packcode;
  int batch_count;
  int substitution;
  
  row = table_p -> model_row;
  lotcode = table_p -> model_p -> item(row-1, 0) -> text().toInt();
  packcode = table_p -> model_p -> item(row-1, 1) -> text().toInt();
  batch_count = table_p -> model_p -> item(row-1, 2) -> text().toInt();
  substitution = table_p -> model_p -> item(row-1, 3) -> text().toInt();
  
  macro_screen_p -> usb_serial_out(lotcode,packcode,batch_count,substitution,0);	//TEST~~~*/
  
  //void macro_screen::usb_serial_out(int lotcode, int packcode, int batch_count, int substitution, int dump_count)	//batch mode
//--------------------------------------------------------------------------------------------------------------//
//TEST Serial Keyboard Output// 10_31_2018
  int row;
  int lotcode;
  int packcode;
  int batch_count;
  int substitution;
  
  row = table_p -> model_row;
  lotcode = table_p -> model_p -> item(row-1, 0) -> text().toInt();
  packcode = table_p -> model_p -> item(row-1, 1) -> text().toInt();
  //batch_count = table_p -> model_p -> item(row-1, 2) -> text().toInt();
  batch_count = count_to_record;
  substitution = table_p -> model_p -> item(row-1, 3) -> text().toInt();
  
  //macro_screen_p -> usb_serial_out(lotcode,packcode,batch_count,substitution,0);	//ORIGINAL~~~
  
  //===========================================================================//11_02_2018~~~//
  //QString Alternative  to Integers//
  QString lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str;
  
  lotcode_str = table_p -> model_p -> item(row-1, 0) -> text();
  packcode_str = table_p -> model_p -> item(row-1, 1) -> text();
  batch_count_str = QString::number(count_to_record);
  substitution_str = table_p -> model_p -> item(row-1, 3) -> text();
  
  macro_screen_p -> usb_serial_out(lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str);	//TEST~~~ 11_02_2018//
  //===========================================================================//

//--------------------------------------------------------------------------------------------------------------//
  
//  pack_ready_count = count;
  barcode_line_p->setFocus();
//  pack_was_placed = false;
  pack_was_removed = true;
}  

void batch::dump_complete(int dump_count)
{
  dump_container_can_be_removed = true;
  dump_container_ready_count = dump_count;
  barcode_line_p->setFocus();
  macro_screen_p -> usb_serial_out(0,0,0,0,dump_count); //TEST~~~
  
  //QString variant to integers//	11_02_2018//
  QString lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str;
  
  dump_count_str = QString::number(dump_count);
  
  macro_screen_p -> usb_serial_out(lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str);

}

void batch::dumping(bool value)
{
  cout<<"batch::dumping.  value = "<<value<<endl;
  dump_flag = value;
}

void batch::focus_on_barcode()
{
  barcode_line_p -> setFocus();
}

void batch::options_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(15);//come back to batch
  centre_p->add_waiting_screen(16);//batch_options
  centre_p->screen_done=true;
}

void batch::back_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}
/*  
void batch::zero_clicked()
{
  centre_p->count = 0;
//  table_p->setFocus();
  barcode_line_p->setFocus();
}

void batch::endgate_clicked()
{
  if(centre_p->get_endgate_state()==ENDGATE_OPEN)
  {
    screen_endgate = ENDGATE_CLOSED;
  }
  else
  {
    screen_endgate = ENDGATE_OPEN;
  }
  barcode_line_p->setFocus();
}
*/

void batch::restart_clicked()
{
//  batch_mode_driver_p -> mode = dump_into_end;
//  cout<<"mode dump_into_end\n";
//  batch_mode_driver_p -> pack_barcode_ok = true;
//  batch_mode_driver_p -> pack_barcode_old = true;
  batch_mode_driver_p -> restart();
  pack_can_be_removed = true;
  pack_removed_too_soon = false;
  dump_container_can_be_removed = false;
  dump_flag = true;
  dump_container_removed_too_soon = false;
  
  /*
  pack_was_placed = false;
  pack_was_removed = false;
  seed_for_pack_ready = false;
  pack_can_be_removed = false;
  pack_removed_too_soon = false;
  dump_container_was_placed = false;
  dump_container_can_be_removed = false;
  dump_container_removed_too_soon = false;
  */
  
  
}

void batch::save_program_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(17);//batch_save_program
  centre_p->screen_done = true;
}

void batch::save_table_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(20);//batch_save_table
  centre_p->screen_done = true;
}

void batch::clear_table_clicked()
{
  table_p->clear();
  barcode_line_p->setFocus();
}

void batch::quit_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(0);
  centre_p->screen_done = true;
}

void batch::run()
{
  QString message=QString("Count: %1").arg(centre_p->count);
  count_message_p->setText(message);
  centre_p->processor_display_blobs(false);
  /*
  if(screen_endgate==ENDGATE_OPEN)
  {
    batch_mode_driver_p -> force_endgate_open = true;
    endgate_button_p->setText("Close Endgate\nHold Seed");
  }
  else //endgate is closed
  {      
    batch_mode_driver_p -> force_endgate_open = false;
    endgate_button_p->setText("Open Endgate\nDischarge Seed");
  }
  */
  count = centre_p->count;
//  cout<<"count = "<<count<<endl;
  
  if( (centre_p->envelope_present==true) && (pack_was_placed==false) && (centre_p->count>0) )
  {
//    cout<<"setting pack_was_placed true\n";
    pack_was_placed = true;
  }
  if( (centre_p->envelope_present==false) && (pack_was_placed==true) && (centre_p->count>0) )
  {
    pack_was_removed = true;
    if( (dump_flag==false) && (count>0) && (centre_p->get_cutgate_state()==CUTGATE_OPEN) && (pack_removed_too_soon==false) )
    {
      
      cout<<"setting pack_removed_too_soon true\n";
      
      pack_removed_too_soon = true;
    }
    if( (dump_flag==true) && (count>0) && (centre_p->get_cutgate_state()==CUTGATE_OPEN) && (dump_container_removed_too_soon==false) )
    {
      dump_container_removed_too_soon = true;
    }
  }
  if(pack_removed_too_soon==true)
  {
    if(centre_p->envelope_present==true)
    {
      pack_removed_too_soon = false;
      pack_was_removed = false;
    }
  }
  if( (seed_for_pack_ready==true) && (batch_mode_driver_p->pack_barcode_ok==true) )
  {
    pack_can_be_removed = true;
    seed_for_pack_ready = false;
//    pack_barcode_ok = false;
  }
  
  
  /*
  if( (pack_can_be_removed==true) && (pack_was_removed==true) ) // && (pack_removed_too_soon==false) ) //&& (batch_mode_driver_p->pack_barcode_ok==true) )
  {
    pack_was_removed = false;
    pack_can_be_removed = false;
    pack_removed_too_soon = false;
    pack_was_placed = false;
    table_p->enter_seeds(pack_ready_count);
  }
  */
  
  
  if(centre_p->envelope_present==true)
  {
    pack_removed_too_soon = false;
    pack_was_removed = false;
    dump_container_removed_too_soon = false;
  }
  if( (dump_container_can_be_removed==true) && (centre_p->envelope_present==false) )
  {
    //new seed lot.  reset dump variables.
    dump_container_can_be_removed = false;
    dump_container_removed_too_soon = false;
    dump_container_was_placed = false;
    cout<<"setting dump_flag false 1 \n";
    dump_flag = false;
    pack_was_removed = false;
    pack_can_be_removed = false;
    pack_removed_too_soon = false;
    pack_was_placed = false;
  }



  if( (pack_can_be_removed==true) && (pack_removed_too_soon==false) )
  {
    QString number;
    pack_ready_message = "PACK READY\nPackage ";
    pack_ready_message.append(number.setNum(batch_mode_driver_p->pack_ready_pack+1));
    pack_ready_message.append(" of ");
    pack_ready_message.append(number.setNum(batch_mode_driver_p->pack_ready_pack_limit));
    pack_ready_message.append("\n");
    pack_ready_message.append(number.setNum(batch_mode_driver_p->pack_ready_count_limit));
    pack_ready_message.append(" seeds");
    status_box_p->set_text(pack_ready_message);
    status_box_p->set_text_size(20);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  else if(pack_removed_too_soon)
  {
    status_box_p->set_text(pack_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if(dump_container_can_be_removed)
  {
    status_box_p->set_text(dump_container_ready_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  else if(dump_container_removed_too_soon)
  {
    status_box_p->set_text(dump_container_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else
  {
    if(dump_flag == true)
    {
      status_box_p->set_text(dump_container_needed_message);
      status_box_p->set_text_size(30);
      status_box_p->set_background(255, 255, 0);
      status_box_p->update();
    }  
    else
    {
      message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\nPack %4 of %5.\n%6 seeds/pack.\nfile %7") 
        .arg(centre_p->crops[0].name) 
        .arg(centre_p->crops[0].sensitivity) 
        .arg(centre_p->crops[0].gate_setting)
        .arg(batch_mode_driver_p->current_pack+1)
        .arg(batch_mode_driver_p->current_pack_limit)
        .arg(batch_mode_driver_p->current_count_limit)
        .arg(batch_mode_driver_p->program_path);
      status_box_p->set_text(message);
      status_box_p->set_text_size(16);
      status_box_p->set_background(30, 200, 255);
      status_box_p->update();
    }
  } 
  
  //barcode status
  if(batch_mode_driver_p->barcode_mode == seed_lot)
  {
    if(batch_mode_driver_p->mode==wait_for_seed_lot_barcode)
    {
      if(batch_mode_driver_p->seed_lot_barcode_ok==false)
      {
        barcode_status_p->set_text("Scan Seed Lot Barcode");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(255, 255, 0);
        barcode_status_p->update();
      } 
    }
    else//mode is not wait_for_seed_lot_barcode
    {
      barcode_status_p->set_text("");
      barcode_status_p->set_text_size(20); 
      barcode_status_p->set_background(30, 200, 255);
      barcode_status_p->update();
    }       
  }
  /*
  if( (batch_mode_driver_p->barcode_mode==pack) && (old_barcode_mode==seed_lot) )
  {
    table_p -> prepare_for_pack_barcode();
  }
  if( (batch_mode_driver_p->barcode_mode==seed_lot) && (old_barcode_mode==pack) )
  {
    table_p -> prepare_for_seed_lot_barcode();
  }
  */
  if(batch_mode_driver_p->barcode_mode == pack)
  {
    if(batch_mode_driver_p->require_pack_barcode == true)
    {
      if(batch_mode_driver_p->pack_barcode_old==true)
      {
        barcode_status_p->set_text("Scan Pack Barcode");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(255, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==true) )
      {
        barcode_status_p->set_text("Barcode OK");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(0, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==false) )
      {
        barcode_status_p->set_text("Incorrect Barcode");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(255, 0, 0);
        barcode_status_p->update();
      }
    }
    else//barcode not required
    {
      barcode_status_p->set_text("Barcode not required");
      barcode_status_p->set_text_size(30); 
      barcode_status_p->set_background(0, 255, 0);
      barcode_status_p->update();
    }
  }
    
  old_barcode_mode = batch_mode_driver_p->barcode_mode;
  
//  barcode_line_p -> setFocus();
  /*
  message.clear();
  QString tempstr;
  message.append("\ncentre_p->count ");
  tempstr.setNum(centre_p->count);
  message.append(tempstr);
  message.append("\npack_was_placed ");
  if(pack_was_placed) message.append("true");
  else message.append("false");
  message.append("\npack_was_removed ");
  if(pack_was_removed) message.append("true");
  else message.append("false");
  message.append("\npack_can_be_removed ");
  if(pack_can_be_removed) message.append("true");
  else message.append("false");
//  message.append("\npack_ready_count ");
//  tempstr.setNum(pack_ready_count);
//  message.append(tempstr);
  message.append("\npack_removed_too_soon ");
  if(pack_removed_too_soon) message.append("true");
  else message.append("false");
  message.append("\ndump_container_was_placed ");
  if(dump_container_was_placed) message.append("true");
  else message.append("false");
  message.append("\ndump_container_can_be_removed ");
  if(dump_container_can_be_removed) message.append("true");
  else message.append("false");
  message.append("\ndump_container_ready_count ");
  tempstr.setNum(dump_container_ready_count);
  message.append(tempstr);
  message.append("\ndump_container_removed_too_soon ");
  if(dump_container_removed_too_soon) message.append("true");
  else message.append("false");
  message.append("\ndump_flag ");
  if(dump_flag) message.append("true");
  else message.append("false");
  message.append("\ncentre_p->envelope_present ");
  if(centre_p->envelope_present) message.append("true");
  else message.append("false");
  message.append("\nbatch_mode_driver_p->pack_barcode_ok ");
  if(batch_mode_driver_p->pack_barcode_ok) message.append("true");
  else message.append("false");
  diagnostics_box_p->set_text(message);
  */
}

