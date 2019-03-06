#include <iostream>
#include <QFileDialog>
#include <QString>
#include <QTimer>
#include "centre.hpp"
#include "spreadsheet_choice.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

spreadsheet_choice::spreadsheet_choice(centre* set_centre_p)
 :screen(set_centre_p)
{ 
  timer_p = new QTimer(this);
  timer_p -> setSingleShot(true);
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  timer_p -> start(10);
  show();
}

spreadsheet_choice::~spreadsheet_choice()
{
  timer_p -> stop();
  delete timer_p;
  timer_p = 0;
}

void spreadsheet_choice::run()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Choose Spreadsheet"), "/media/odroid", tr("csv files(*.csv)"));
  cout<<"sc1\n";
  centre_p->batch_mode_driver_p->spreadsheet_filename = filename;
  centre_p->batch_mode_driver_p->use_spreadsheet = true;
  centre_p->batch_mode_driver_p->load_spreadsheet(filename);
  centre_p->add_waiting_screen(22);
  centre_p->screen_done=true;
  cout<<"end of spreadsheet_choice::run\n";
}
