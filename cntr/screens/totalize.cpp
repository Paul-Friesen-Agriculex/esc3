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
#include "totalize.hpp"
#include "button.hpp"
#include <QString>				//for gpio_keyboard, send count to keyboardout

using namespace std;

totalize::totalize(centre* set_centre_p)
 :screen(set_centre_p)
{
//  cout<<"start totalize::totalize\n";
  
  if (centre_p->crops[0].calibrated == false) 
  {
    centre_p->add_waiting_screen(5);//back here to totalize
    centre_p->add_waiting_screen(10);//calibrate
    centre_p->screen_done = true;
//    return;
  }
  count_message_p = new QLabel;
  options_button_p = new button("Options");
  back_button_p = new button("Back");
  zero_button_p = new button("Zero");
  endgate_button_p = new button("");
  speed_label_p = new QLabel("Speed");
  startstop_button_p = new button("");
  speed_set_p = new QSlider;
  speed_set_p->setMinimum(0);
  speed_set_p->setMaximum(1000);
  speed_set_p->setOrientation(Qt::Horizontal);
  table_p = new table(centre_p);
  status_box_p = new QLabel;
  save_button_p = new button("Save File");
  clear_table_button_p = new button("Clear Table");
  quit_button_p = new button("Quit");
  
  
  top_box_p = new QGroupBox;
  control_box_p = new QGroupBox;
  speed_box_p = new QGroupBox;
  bottom_box_p = new QGroupBox;
  
  top_layout_p = new QGridLayout;
  control_layout_p = new QGridLayout;
  speed_layout_p = new QGridLayout;
  bottom_layout_p = new QGridLayout;
  
  main_layout_p = new QGridLayout;

  top_layout_p -> addWidget(count_message_p, 0, 0);
  top_layout_p -> addWidget(options_button_p, 0, 1);
  top_layout_p -> addWidget(back_button_p, 0, 2);
  control_layout_p -> addWidget(zero_button_p, 0, 0);
  control_layout_p -> addWidget(endgate_button_p, 1, 0);
  control_layout_p -> addWidget(speed_box_p, 2, 0);
  speed_layout_p -> addWidget(speed_label_p, 0, 0);
  speed_layout_p -> addWidget(startstop_button_p, 0, 1);
  speed_layout_p -> addWidget(speed_set_p, 1, 0, 1, 2);
  bottom_layout_p -> addWidget(status_box_p, 0, 0);
  bottom_layout_p -> addWidget(save_button_p, 0, 2);
  bottom_layout_p -> addWidget(clear_table_button_p, 0, 3);
  bottom_layout_p -> addWidget(quit_button_p, 0, 4);
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
  connect(zero_button_p, SIGNAL(clicked()), this, SLOT(zero_clicked()));
  connect(endgate_button_p, SIGNAL(clicked()), this, SLOT(endgate_clicked()));
  connect(startstop_button_p, SIGNAL(clicked()), this, SLOT(startstop_clicked()));
  connect(speed_set_p, SIGNAL(valueChanged(int)), this, SLOT(change_speed(int)));
  connect(save_button_p, SIGNAL(clicked()), this, SLOT(save_clicked()));
  connect(clear_table_button_p, SIGNAL(clicked()), this, SLOT(clear_table_clicked()));
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));
  
  centre_p->set_endgate_state(ENDGATE_CLOSED);
  if(centre_p->totalize_force_endgate_open == true)
  {
    screen_endgate=ENDGATE_OPEN;
  }
  else
  {
    screen_endgate=ENDGATE_CLOSED;
  }
  endgate_button_p->setText("Open Endgate\nRelease Seed");
  startstop_button_p->setText("Stop");
  
  centre_p->set_cutgate_state(CUTGATE_OPEN);
  
  speed_set_p->setValue(centre_p->totalize_feed_speed);
  centre_p->set_speed(speed_set_p->value());
  feeder_is_running = true;
  
  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(100);
  
  cout<<"totalize::totalize svc ";
  table_p->load_file(QString("settings/totalize_backup"));
  table_p->set_visible_columns(centre_p->tm_barcode_columns);
  
  count = old_count = 0;
  table_p->setFocus();
  
  centre_p->processor_display_blobs(true);
  centre_p->set_to_record(true);
  centre_p->end_of_playback_flag = false;
  
  if(centre_p->tm_save_filename != "")//returning from totalize_save_file screen with name of file to save
  {
    table_p->save_file(centre_p->tm_save_filename);
    centre_p->tm_save_filename = "";
  }
  
//  cout<<"end totalize::totalize\n";
  
}

totalize::~totalize()
{
  
//  cout<<"start totalize::~totalize\n";
  
  run_timer_p -> stop();
  centre_p->set_to_record_f(false);
  table_p->save_file(QString("settings/totalize_backup"));
  
//  cout<<"end totalize::~totalize\n";
}

void totalize::options_clicked()
{
  centre_p->add_waiting_screen(5);//come back to totalize
  centre_p->add_waiting_screen(6);//totalize_options
  centre_p->screen_done=true;
  centre_p->set_speed(0);
}

void totalize::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
  centre_p->set_speed(0);
}
  
void totalize::zero_clicked()
{
  centre_p->count = 0;
  table_p->setFocus();
}

void totalize::endgate_clicked()
{
  if(centre_p->get_endgate_state()==ENDGATE_OPEN)
  {
    screen_endgate = ENDGATE_CLOSED;
  }
  else
  {
    screen_endgate = ENDGATE_OPEN;
  }
  table_p->setFocus();
}

void totalize::startstop_clicked()
{
  if(feeder_is_running)
  {
    centre_p->set_speed(0);
    startstop_button_p->setText("Start");
    feeder_is_running = false;
  }
  else
  {
    centre_p->set_speed(speed_set_p->value());
    startstop_button_p->setText("Stop");
    feeder_is_running = true;
  }
  table_p->setFocus();
}

void totalize::change_speed(int value)
{
  centre_p->totalize_feed_speed=value;
//  cout<<"totalize_feed_speed set to "<<value<<endl;
  if(feeder_is_running)
  {
    centre_p->set_speed(value);
  }
  table_p->setFocus();
}

void totalize::save_clicked()
{
  centre_p->add_waiting_screen(12);
  centre_p->screen_done = true;
  centre_p->set_speed(0);
}

void totalize::clear_table_clicked()
{
  table_p->clear();
}

void totalize::quit_clicked()
{
  centre_p->add_waiting_screen(0);
  centre_p->screen_done = true;
  centre_p->set_speed(0);
}

void totalize::run()
{

  QString message=QString("Count: %1").arg(centre_p->count);
  count_message_p->setText(message);
    centre_p->processor_display_blobs(false);
    message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\nLine frequency %4\nDust Streak Prcnt %5") 
    .arg(centre_p->crops[0].name) 
    .arg(centre_p->crops[0].sensitivity) 
    .arg(centre_p->crops[0].gate_setting)
    .arg(centre_p->measured_line_frequency)
    .arg(centre_p->dust_streak_percentage(), 0, 'f', 3);
  status_box_p->setText(message);
 
  if(screen_endgate==ENDGATE_OPEN)
  {
    centre_p -> totalize_force_endgate_open = true;
    endgate_button_p->setText("Close Endgate\nHold Seed");
    centre_p->tm_zero_when_seed_discharged = false;
  }
  else //endgate is closed
  {      
    centre_p -> totalize_force_endgate_open = false;
    endgate_button_p->setText("Open Endgate\nDischarge Seed");
    centre_p->tm_zero_when_seed_discharged = true;
  }
  
  old_count = count;
  count = centre_p->count;
  if(count<old_count)//count was zeroed.  record old_count
  {
    table_p -> enter_seeds(old_count);
    centre_p -> tm_autosave_count ++;
    
    if((centre_p->tm_autosave_count) > (centre_p->tm_autosave_count_limit))
    {
      centre_p->tm_autosave_count = 0;
      table_p->save_file(QString("settings/totalize_backup"));
    }
    
    count_string = QString::number(old_count);
    gpio_keyboard_p = new gpio_keyboard;
    //macro_screen_p = new macro_screen;  //TEST~~~
    
    if(!(centre_p->tm_macro_updated))
    {
      centre_p->load_macros();
      cout<<"loading macros"<<endl;
	  }
	  else
	  {
	    cout<<"macros already loaded"<<endl;
	  }
	  
//--------------------------------------------------------------------------------------------------------------------------//
    //load totalize table variables//
    int current_totalize_table_row;    
    int barcode_1, barcode_2, barcode_3, barcode_4, totalize_count, weight;
    
    current_totalize_table_row = table_p -> model_row - 1;
    barcode_1 = (table_p -> model_p -> item(current_totalize_table_row,0) -> text().toInt());       //Barcode 1
    barcode_2 = (table_p -> model_p -> item(current_totalize_table_row,1) -> text().toInt());       //Barcode 2
    barcode_3 = (table_p -> model_p -> item(current_totalize_table_row,2) -> text().toInt());       //Barcode 3
    barcode_4 = (table_p -> model_p -> item(current_totalize_table_row,3) -> text().toInt());       //Barcode 4
    totalize_count = (table_p -> model_p -> item(current_totalize_table_row,4) -> text().toInt());  //Count
    weight = (table_p -> model_p -> item(current_totalize_table_row,5) -> text().toInt());          //Weight
    
    /*cout<<endl<<"bar1: "<<barcode_1<<endl;        //OMIT~~~
    cout<<endl<<"bar2: "<<barcode_2<<endl;        //OMIT~~~
    cout<<endl<<"bar3: "<<barcode_3<<endl;        //OMIT~~~
    cout<<endl<<"bar4: "<<barcode_4<<endl;        //OMIT~~~
    cout<<endl<<"count: "<<totalize_count<<endl;  //OMIT~~~
    cout<<endl<<"weight: "<<weight<<endl;         //OMIT~~~*/
//--------------------------------------------------------------------------------------------------------------------------//
    //gpio_keyboard_p -> gpio_send(count_string+(centre_p->combined_macro_functions));
	  //gpio_keyboard_p -> gpio_send((centre_p->combined_macro_functions));	//Original~~~
    
    //macro_screen_p = new macro_screen;  //TEST~~~
    //macro_screen_p -> usb_serial_out(centre_p->combined_macro_functions);	//TEST~~~
    macro_screen_p -> usb_serial_out(barcode_1, barcode_2, barcode_3, barcode_4, totalize_count, weight);	//TEST~~~
  }

  if(centre_p->tm_zero_when_seed_discharged == true)
  {
    if(centre_p->envelope_present == true)
    {
      old_envelope_present = true;
    }
    else
    {
      if(old_envelope_present == true)//envelope just removed
      {
        old_envelope_present = false; 
        centre_p->count = 0;
      }
    }
  }
}

