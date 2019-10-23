#include <iostream>
#include <fstream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include "centre.hpp"
#include "totalize.hpp"
#include "button.hpp"
#include <QString>

//#include <QtMath> //exponential qslider// TEST~~~//

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

  top_layout_p -> addWidget(count_message_p, 0, 0);                      //NEW~~~ 11_16_2018//
  top_layout_p -> addWidget(options_button_p, 0, 1);
  top_layout_p -> addWidget(back_button_p, 0, 2);
  control_layout_p -> addWidget(zero_button_p, 0, 0);
  control_layout_p -> addWidget(endgate_button_p, 1, 0);
  //control_layout_p -> addWidget(speed_box_p, 2, 0);
  speed_layout_p -> addWidget(speed_label_p, 0, 1, 1, 3);
  speed_layout_p -> addWidget(startstop_button_p, 0, 0, 2, 1);
  speed_layout_p -> addWidget(speed_set_p, 1, 1, 1, 3);
  bottom_layout_p -> addWidget(status_box_p, 0, 0);
  bottom_layout_p -> addWidget(save_button_p, 0, 2);
  bottom_layout_p -> addWidget(clear_table_button_p, 0, 3);
  bottom_layout_p -> addWidget(quit_button_p, 0, 4);
  
  main_layout_p -> addWidget(top_box_p, 0, 0, 1, 2);
  main_layout_p -> addWidget(speed_box_p, 1, 0, 1, 4);
  main_layout_p -> addWidget(control_box_p, 2, 0);
  main_layout_p -> addWidget(table_p, 2, 1);
  //main_layout_p -> addWidget(speed_box_p, 2, 0, 1, 4); //TEST~~~
  main_layout_p -> addWidget(bottom_box_p, 3, 0, 1, 2); //TEST~~~
  
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
  
  centre_p->cutgate_p->open();
  
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
  
  speed_label_p->setAlignment(Qt::AlignCenter);
  top_layout_p->setContentsMargins(0,0,0,0);        //set layout margins to shrink to designated container dimensions//
  control_layout_p->setContentsMargins(0,0,0,0);
  speed_layout_p->setContentsMargins(0,0,0,0);
  bottom_layout_p->setContentsMargins(0,0,0,0);
  main_layout_p->setContentsMargins(0,0,10,10);
  
//  table_p->saved = false;
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
  
  //~~~feeder speed behaviour~~~//
  if(value <= 500) value = value/2;         //~~~piece-wise linear, 2 parts~~~//
  else value = 1.5*value-500;
  //value = pow(1.0069, value) - 1;          //~~~exponential~~~//
  
  
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
  if(table_p->saved == false)
  {
    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) 
    {
      case QMessageBox::Save:
        save_clicked();
        return;
      case QMessageBox::Discard:
        table_p->clear();
        return;
      case QMessageBox::Cancel:
        return;
      default:
        // should never be reached
        return;
    }
  }
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

  QString message=QString("Count:  %1").arg(centre_p->count);
  count_message_p->setText(message);
    centre_p->processor_display_blobs(false);
    message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.") 
    .arg(centre_p->crops[0].name) 
    .arg(centre_p->crops[0].sensitivity) 
    .arg(centre_p->crops[0].gate_setting);
  status_box_p->setText(message);                           //NEW~~~ 11_16_2018//
  
  count_message_p->setStyleSheet("QLabel {"     //TEST~~~
          "background-color: white;"            //
          "border: 3px solid black;"            //
          "font-size: 20pt;}");                 //

  int slider_position_num;
  if(speed_set_p->sliderPosition() <= 500) slider_position_num = (speed_set_p->sliderPosition())/2;      //piece-wise linear, 2 sections//
  else slider_position_num = 1.5*(speed_set_p->sliderPosition())-500;
  //slider_position_num = pow(1.0069, (speed_set_p->sliderPosition())) - 1;         //exponential function//

  QString slider_position_message = QString("Speed:  %1 \%").arg(1.0*slider_position_num/10);    //TEST~~~
  //QString slider_position_message = QString("Speed:  %1 \%").arg(1.0*speed_set_p->sliderPosition()/10);    //ORIGINAL~~~
  speed_label_p->setText(slider_position_message);                                                         //TEST~~~
  speed_label_p->setStyleSheet("QLabel {"
          "font: bold;}");
  
 
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
    
    if(!(centre_p->tm_macro_updated))
    {
      centre_p->load_macros();
      cout<<"loading macros"<<endl;
	  }
	  else
	  {
	    cout<<"macros already loaded"<<endl;
	  }
	  
    //load totalize table variables//
    QString bar_str_1, bar_str_2, bar_str_3, bar_str_4, totalize_str_count, totalize_str_weight;      //modified to handle barcodes as characters instead of integers// 11_02_2018~~~//
    int current_totalize_table_row;    
    current_totalize_table_row = table_p -> model_row - 1;
    
    bar_str_1 = (table_p -> model_p -> item(current_totalize_table_row,0) -> text());
    bar_str_2 = (table_p -> model_p -> item(current_totalize_table_row,1) -> text());
    bar_str_3 = (table_p -> model_p -> item(current_totalize_table_row,2) -> text()); 
    bar_str_4 = (table_p -> model_p -> item(current_totalize_table_row,3) -> text());
    totalize_str_count = (table_p -> model_p -> item(current_totalize_table_row,4) -> text());
    totalize_str_weight = (table_p -> model_p -> item(current_totalize_table_row,5) -> text());

    macro_screen_p -> usb_serial_out(bar_str_1, bar_str_2, bar_str_3, bar_str_4, totalize_str_count, totalize_str_weight);	  //TEST~~~ Strings instead of ints 11_02_2018~~~//

    table_p->save_file(QString("settings/totalize_backup"));
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

