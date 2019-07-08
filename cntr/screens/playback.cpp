#include <iostream>
#include <fstream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QSlider>
#include <QLineEdit>
#include "centre.hpp"
#include "processor.hpp"
#include "playback.hpp"
#include "button.hpp"
#include "display.hpp"


using namespace std;

extern int line_length;//declared in centre.cpp
extern const int images_to_record;//declared in centre.cpp
extern const int image_lines;//defined in centre.cpp

playback::playback(centre*set_centre_p)
 :screen(set_centre_p)
{
  title_p=new QLabel("Playback");
  autoadvance_button_p = new button("Turn on");
  autoadvance_slider_p = new QSlider(Qt::Horizontal);
  autoadvance_slider_p->setMinimum(0);
  autoadvance_slider_p->setMaximum(100);
  autoadvance_slider_p->setValue(0);
  autoadvance_box_p = new QGroupBox("Auto Advance");
  autoadvance_layout_p = new QGridLayout;
  autoadvance_timer_p = new QTimer;
  autoadvance_timer_p->setInterval(100);

  back_button_p=new button("Back");
  count_message_p=new QLabel;
  zero_button_p=new button("Zero");
  advance1_button_p=new button("Advance 1");
  advance10_button_p=new button("Advance 10");
  advance100_button_p=new button("Advance 100");
  file_button_p=new button("Save File");
  load_file_button_p=new button("Load File");
  quit_button_p=new button("Quit");
  display_p=new display(500, this);
//  save_image_button_p = new button("Save image");
  file_name_p = new QLineEdit("image");

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(title_p, 0, 0);
  autoadvance_layout_p->addWidget(autoadvance_button_p, 0, 0);  
  autoadvance_layout_p->addWidget(autoadvance_slider_p, 1, 0);
  autoadvance_box_p->setLayout(autoadvance_layout_p);
  main_layout_p->addWidget(autoadvance_box_p, 0, 1, 1, 3);
  main_layout_p->addWidget(count_message_p, 1, 0);
  main_layout_p->addWidget(zero_button_p, 2, 0);
  main_layout_p->addWidget(advance1_button_p, 3, 0);
  main_layout_p->addWidget(advance10_button_p, 4, 0);
  main_layout_p->addWidget(advance100_button_p, 5, 0);
  main_layout_p->addWidget(back_button_p, 0, 5);
  main_layout_p->addWidget(display_p, 1, 1, 3, 5);
  main_layout_p->addWidget(file_button_p, 6, 1);
  main_layout_p->addWidget(load_file_button_p, 6, 2);
  main_layout_p -> addWidget(file_name_p, 6, 3);
  main_layout_p->addWidget(quit_button_p, 6, 5);
  
  setLayout(main_layout_p);

  connect(zero_button_p, SIGNAL(clicked()), this, SLOT(zero_clicked()));  
  connect(advance1_button_p, SIGNAL(clicked()), this, SLOT(advance1_clicked()));  
  connect(advance10_button_p, SIGNAL(clicked()), this, SLOT(advance10_clicked()));  
  connect(advance100_button_p, SIGNAL(clicked()), this, SLOT(advance100_clicked()));  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));  
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));  
  connect(autoadvance_button_p, SIGNAL(clicked()), this, SLOT(autoadvance_clicked()));
  connect(autoadvance_timer_p, SIGNAL(timeout()), this, SLOT(advance_n()));
  connect(autoadvance_slider_p, SIGNAL(valueChanged(int)), this, SLOT(set_n(int)));
  connect(file_button_p, SIGNAL(clicked()), this, SLOT(save_image()));
  connect(load_file_button_p, SIGNAL(clicked()), this, SLOT(load_image()));
  
  QString message=QString("Count: %1").arg(centre_p->count);
  count_message_p->setText(message);

  qtimer_p = new QTimer;
  connect(qtimer_p, SIGNAL(timeout()), this, SLOT(run()));
  qtimer_p->start(100);
  centre_p->set_camera_processing(false);
  centre_p->processor_display_blobs(true);
  autoadvance_on = false;
  centre_p->end_of_playback_flag = false;
}
 
void playback::autoadvance_clicked()
{
  if(autoadvance_on)
  {
    autoadvance_on = false;
    autoadvance_button_p->setText("Turn on");
    autoadvance_timer_p->stop();
  }
  else
  {
    autoadvance_on = true;
    autoadvance_button_p->setText("Turn off");
    autoadvance_timer_p->start();
  }
}

playback::~playback()
{
  centre_p->processor_display_blobs(false);
  centre_p->set_camera_processing(true);
}

void playback::back_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}  

void playback::zero_clicked()
{
  centre_p->count = 0;
}

void playback::advance1_clicked()
{
  centre_p -> playback_line_f();
}
  
void playback::advance10_clicked()
{
  for(int i = 0; i<10; ++i)
  {
    advance1_clicked();
  }
}

void playback::advance100_clicked()
{
  for(int i = 0; i<100; ++i)
  {
    advance1_clicked();
  }
}

void playback::advance_n()
{
  for(int i=0; i<n; ++i)
  {
    advance1_clicked();
  }
}

void playback::set_n(int n_s)
{
  n = n_s;
}

void playback::quit_clicked()
{
  centre_p->add_waiting_screen(0);
  centre_p->screen_done=true;
}  

void playback::run()
{
  QString message;
  if(centre_p->end_of_playback_flag == false)message=QString("Count: %1").arg(centre_p->count);
  else message=QString("End.  Count: %1").arg(centre_p->count);
  count_message_p->setText(message);
  
  centre_p -> get_qimage_f();
  display_p -> image = centre_p -> qimage;
  display_p -> update();
}

void playback::save_image()
{
  centre_p -> save_image(file_name_p->text());
}

void playback::load_image()
{
  centre_p -> load_image(file_name_p->text());
}
