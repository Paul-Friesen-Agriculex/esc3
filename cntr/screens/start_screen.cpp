#include <iostream>
#include <QGridLayout>
#include <QGroupBox>
#include "centre.hpp"
#include "start_screen.hpp"
#include "button.hpp"

using namespace std;

start_screen::start_screen(centre* set_centre_p)
 :screen(set_centre_p)
{
  totalize_button_p_test = new QPushButton("Totalize Mode"); //QPushButton Alternative//
  batch_button_p_test = new QPushButton("Batch Mode");       //
  tools_button_p_test = new QPushButton("Tools");            //
  shutdown_button_p_test = new QPushButton("Shut Down");     //
  exit_button_p_test = new QPushButton("Exit Program");      //
  
  /*totalize_button_p = new button("Totalize Mode");
  batch_button_p = new button("Batch Mode");
  tools_button_p = new button("Tools");
  shutdown_button_p = new button("Shut Down");
  exit_button_p = new button("Exit Program");*/
  modebox_layout_p = new QGridLayout;
  shutdownbox_layout_p = new QGridLayout;
  modebox_p = new QGroupBox;
  shutdownbox_p = new QGroupBox;
  main_layout_p = new QGridLayout;

  
  modebox_layout_p->addWidget(totalize_button_p_test, 0, 0);        //QPushButton Alternatives//
  modebox_layout_p->addWidget(batch_button_p_test, 0, 1);           //
  modebox_layout_p->addWidget(tools_button_p_test, 0, 2);           //
  shutdownbox_layout_p->addWidget(shutdown_button_p_test, 0, 0);    //
  shutdownbox_layout_p->addWidget(exit_button_p_test, 0, 1);        //
  
  /*modebox_layout_p->addWidget(totalize_button_p, 0, 0);
  modebox_layout_p->addWidget(batch_button_p, 0, 1);
  modebox_layout_p->addWidget(tools_button_p, 0, 2);
  shutdownbox_layout_p->addWidget(shutdown_button_p, 0, 0);
  shutdownbox_layout_p->addWidget(exit_button_p, 0, 1);*/
  main_layout_p -> addWidget(modebox_p,0,0);
  main_layout_p -> addWidget(shutdownbox_p,1,0);
  
  modebox_p -> setLayout(modebox_layout_p);
  shutdownbox_p -> setLayout(shutdownbox_layout_p);
  setLayout(main_layout_p);
  
  connect(totalize_button_p_test, SIGNAL(clicked()), this, SLOT(totalize_clicked()));   //QPushButton Alternatives//
  connect(batch_button_p_test, SIGNAL(clicked()), this, SLOT(batch_clicked()));         //
  connect(tools_button_p_test, SIGNAL(clicked()), this, SLOT(tools_clicked()));         //
  connect(shutdown_button_p_test, SIGNAL(clicked()), this, SLOT(shutdown()));           //
  connect(exit_button_p_test, SIGNAL(clicked()), this, SLOT(exit_program()));           //

  /*connect(totalize_button_p, SIGNAL(clicked()), this, SLOT(totalize_clicked()));
  connect(batch_button_p, SIGNAL(clicked()), this, SLOT(batch_clicked()));
  connect(tools_button_p, SIGNAL(clicked()), this, SLOT(tools_clicked()));
  connect(shutdown_button_p, SIGNAL(clicked()), this, SLOT(shutdown()));
  connect(exit_button_p, SIGNAL(clicked()), this, SLOT(exit_program()));*/
  
  //Setstylesheet for gui elements//  TEST~~~
  //
  
  setStyleSheet(
    "QPushButton {"
      "border: 2px solid #54575f;"
      //"width: 13px;"
      "height: 50px;"
      "font: 25px;"
      "border-radius: 12px;"
      "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
      "stop: 0 #eff0f6, stop: 1 #c9cacf);"
      "min-width: 40px;}"
  );
}

void start_screen::totalize_clicked()
{
  centre_p->add_waiting_screen(5);//totalize
  centre_p->add_waiting_screen(1);//crop_choice
  centre_p->screen_done=true;
}

void start_screen::batch_clicked()
{
  centre_p->add_waiting_screen(13);//program_choice
  centre_p->add_waiting_screen(1);//crop_choice
  centre_p->screen_done=true;
}

void start_screen::tools_clicked()
{
  centre_p->add_waiting_screen(9);
  centre_p->screen_done=true;
}

void start_screen::shutdown()
{
  centre_p->~centre();
  exit(1);
}

void start_screen::exit_program()
{
  centre_p->~centre();
  exit(0);
}
