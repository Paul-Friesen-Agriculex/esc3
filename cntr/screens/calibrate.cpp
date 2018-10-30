#include <iostream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QTextEdit>
#include <QTimer>
#include "centre.hpp"
#include "calibrate.hpp"
#include "button.hpp"

using namespace std;

calibrate::calibrate(centre* set_centre_p)
 :screen(set_centre_p)
{
  count_message_p = new QLabel;
  gateset_button_p = new button("Change Gate\nSetting");
  back_button_p = new button("Back");
  zero_button_p = new button("Re-start\nCalibration");
  endgate_button_p = new button("");
  speed_label_p = new QLabel("Speed");
  startstop_button_p = new button("");
  speed_set_p = new QSlider;
  speed_set_p->setMinimum(1);
  speed_set_p->setMaximum(50);
  speed_set_p->setOrientation(Qt::Horizontal);
  message_box_p = new QTextEdit;
  skip_button_p = new button("Skip Calibration");
  done_button_p = new button("Done");
  
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
  top_layout_p -> addWidget(gateset_button_p, 0, 1);
  top_layout_p -> addWidget(back_button_p, 0, 2);
  control_layout_p -> addWidget(zero_button_p, 0, 0);
  control_layout_p -> addWidget(endgate_button_p, 1, 0);
  control_layout_p -> addWidget(speed_box_p, 2, 0);
  speed_layout_p -> addWidget(speed_label_p, 0, 0);
  speed_layout_p -> addWidget(startstop_button_p, 0, 1);
  speed_layout_p -> addWidget(speed_set_p, 1, 0, 1, 2);
  bottom_layout_p -> addWidget(skip_button_p, 0, 2);
  bottom_layout_p -> addWidget(done_button_p, 0, 3);
  main_layout_p -> addWidget(top_box_p, 0, 0, 1, 2);
  main_layout_p -> addWidget(control_box_p, 1, 0);
  main_layout_p -> addWidget(message_box_p, 1, 1);
  main_layout_p -> addWidget(bottom_box_p, 2, 0, 1, 2);
  
  top_box_p -> setLayout(top_layout_p);
  control_box_p -> setLayout(control_layout_p);
  speed_box_p -> setLayout(speed_layout_p);
  bottom_box_p -> setLayout(bottom_layout_p);
  setLayout(main_layout_p);
  
  connect(gateset_button_p, SIGNAL(clicked()), this, SLOT(gateset_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(zero_button_p, SIGNAL(clicked()), this, SLOT(zero_clicked()));
  connect(endgate_button_p, SIGNAL(clicked()), this, SLOT(endgate_clicked()));
  connect(startstop_button_p, SIGNAL(clicked()), this, SLOT(startstop_clicked()));
  connect(speed_set_p, SIGNAL(valueChanged(int)), this, SLOT(change_speed(int)));
  connect(skip_button_p, SIGNAL(clicked()), this, SLOT(skip_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_clicked()));
  
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
  
  speed_set_p->setValue(10);
  centre_p->set_speed(speed_set_p->value());
  feeder_is_running = true;
  
  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(10);
  
  centre_p->processor_display_blobs_f(true);
  
  QString message;
  if(centre_p->recalibrating == true)
  {
    message=QString("Old %1 calibration:") .arg(centre_p->crops[0].name);// .arg(centre_p->crops[0].sensitivity) .arg(centre_p->crops[0].gate_setting);
    message_box_p->append(message);
    message=QString("Sensitivity %1.") .arg(centre_p->crops[0].sensitivity);
    message_box_p->append(message);
    message=QString("Gate Setting %1.").arg(centre_p->crops[0].gate_setting);
    message_box_p->append(message);
    message=QString("Area Mean %1.") .arg(centre_p->crops[0].area_mean);
    message_box_p->append(message);
    message=QString("Area Standard Deviation %1.") .arg(centre_p->crops[0].area_stdev);
    message_box_p->append(message);
    message=QString("Maximum Inflection %1.") .arg(centre_p->crops[0].max_inflection);
    message_box_p->append(message);
    message=QString("Inflection Count Mean %1.") .arg(centre_p->crops[0].inflection_count_mean);
    message_box_p->append(message);
    message=QString("Recalibrating.  Drop 50 seeds slowly.");
    message_box_p->append(message);

    centre_p->crops[0].calibrated = false;
    centre_p->restart_calibration_f();
    centre_p->recalibrating = false;
  }
  else
  {
    message=QString("Crop: %1.  Calibrating new crop.\nDrop 50 seeds slowly.") .arg(centre_p->crops[0].name);
    message_box_p->append(message);
  }
  
  centre_p->count = 0;
  calibration_message_posted = false;
}

calibrate::~calibrate()
{
  run_timer_p -> stop();
  delete run_timer_p;
}

void calibrate::gateset_clicked()
{
  centre_p->add_waiting_screen(10);//come back here when done
  centre_p->add_waiting_screen(11);//gateset
  centre_p->screen_done=true;
  centre_p->set_speed(0);
}

void calibrate::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
  centre_p->set_speed(0);
}
  
void calibrate::zero_clicked()
{
  centre_p->count = 0;
  centre_p->set_speed(speed_set_p->value());
  centre_p->restart_calibration_f();
}

void calibrate::endgate_clicked()
{
  if(centre_p->get_endgate_state()==ENDGATE_OPEN)
  {
    screen_endgate = ENDGATE_CLOSED;
  }
  else
  {
    screen_endgate = ENDGATE_OPEN;
  }
}

void calibrate::startstop_clicked()
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
}

void calibrate::change_speed(int value)
{
  if(feeder_is_running)
  {
    centre_p->set_speed(value);
  }
}
void calibrate::skip_clicked()
{
  centre_p->restart_calibration_f();//resets calibration parameters in processor
  centre_p->set_speed(0);
  centre_p->count = 0;
  centre_p->add_waiting_screen(0);
  centre_p->screen_done = true;
}

void calibrate::done_clicked()
{
  centre_p->restart_calibration_f();
  centre_p->set_speed(0);
  centre_p->count = 0;
  centre_p->screen_done = true;
  centre_p->recalibrating = false;
} 

void calibrate::run()
{
  QString message=QString("Calibrating.  Count: %1").arg(centre_p->count);
  count_message_p->setText(message);
  
  if (centre_p->crops[0].calibrated)
  {
    if(calibration_message_posted==false)
    {
      calibration_message_posted = true;
      centre_p->processor_display_blobs(false);
      centre_p->set_speed(500);
      message=QString("Crop %1 calibrated.") .arg(centre_p->crops[0].name);// .arg(centre_p->crops[0].sensitivity) .arg(centre_p->crops[0].gate_setting);
      message_box_p->append(message);
      message=QString("Sensitivity %1.") .arg(centre_p->crops[0].sensitivity);
      message_box_p->append(message);
      message=QString("Gate Setting %1.").arg(centre_p->crops[0].gate_setting);
      message_box_p->append(message);
      message=QString("Area Mean %1.") .arg(centre_p->crops[0].area_mean);
      message_box_p->append(message);
      message=QString("Area Standard Deviation %1.") .arg(centre_p->crops[0].area_stdev);
      message_box_p->append(message);
      message=QString("Maximum Inflection %1.") .arg(centre_p->crops[0].max_inflection);
      message_box_p->append(message);
      message=QString("Inflection Count Mean %1.") .arg(centre_p->crops[0].inflection_count_mean);
      message_box_p->append(message);
    }
  }
  if(screen_endgate==ENDGATE_OPEN)
  {
    centre_p -> totalize_force_endgate_open = true;
    endgate_button_p->setText("Close Endgate\nHold Seed");
  }
  else //endgate is closed
  {      
    centre_p -> totalize_force_endgate_open = false;
    endgate_button_p->setText("Open Endgate\nDischarge Seed");
  }
}
