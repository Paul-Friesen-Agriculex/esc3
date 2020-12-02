#include <iostream>
#include <QGridLayout>
#include <QGroupBox>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include "centre.hpp"
#include "start_screen.hpp"
#include "button.hpp"
#include "help_screen.hpp"

using namespace std;

start_screen::start_screen(centre* set_centre_p)
 :screen(set_centre_p)
{ 
  totalize_button_p = new button("Totalize Mode");
  batch_button_p = new button("Batch Mode");
  batch_from_spreadsheet_button_p = new button("Batch From Spreadsheet Mode");
  tools_button_p = new button("Tools/Options");
  slave_mode_button_p = new button("Slave Mode");
  eject_memory_sticks_button_p = new button("Safely remove memory sticks");
  shutdown_button_p = new button("Shut Down");
  help_button_p = new button("Help");
//  exit_button_p = new button("Exit Program");
  
  modebox_layout_p = new QGridLayout;
  shutdownbox_layout_p = new QGridLayout;
  modebox_p = new QGroupBox;
  shutdownbox_p = new QGroupBox;
  main_layout_p = new QGridLayout;

  modebox_layout_p->addWidget(totalize_button_p, 0, 0);
  modebox_layout_p->addWidget(batch_button_p, 0, 1);
  modebox_layout_p->addWidget(batch_from_spreadsheet_button_p, 0, 2);
  modebox_layout_p->addWidget(tools_button_p, 1, 0);
  modebox_layout_p->addWidget(slave_mode_button_p, 1, 2);
  shutdownbox_layout_p->addWidget(eject_memory_sticks_button_p, 0, 1);
  shutdownbox_layout_p->addWidget(help_button_p, 1, 0);
  shutdownbox_layout_p->addWidget(shutdown_button_p, 1, 1);
//  shutdownbox_layout_p->addWidget(exit_button_p, 0, 1);
  main_layout_p -> addWidget(modebox_p,0,0);
  main_layout_p -> addWidget(shutdownbox_p,1,0);
  
  modebox_p -> setLayout(modebox_layout_p);
  shutdownbox_p -> setLayout(shutdownbox_layout_p);
  setLayout(main_layout_p);

  connect(totalize_button_p, SIGNAL(clicked()), this, SLOT(totalize_clicked()));
  connect(batch_button_p, SIGNAL(clicked()), this, SLOT(batch_clicked()));
  connect(batch_from_spreadsheet_button_p, SIGNAL(clicked()), this, SLOT(batch_from_spreadsheet_clicked()));
  connect(tools_button_p, SIGNAL(clicked()), this, SLOT(tools_clicked()));
  connect(slave_mode_button_p, SIGNAL(clicked()), this, SLOT(slave_mode_clicked()));
  connect(eject_memory_sticks_button_p, SIGNAL(clicked()), this, SLOT(eject_memory_sticks_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(shutdown_button_p, SIGNAL(clicked()), this, SLOT(shutdown()));
//  connect(exit_button_p, SIGNAL(clicked()), this, SLOT(exit_program()));

  help_screen_p = 0;
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

void start_screen::batch_from_spreadsheet_clicked()
{
  centre_p->add_waiting_screen(21);//spreadsheet_choice
  centre_p->add_waiting_screen(1);//crop_choice
  centre_p->screen_done=true;
}

void start_screen::tools_clicked()
{
  centre_p->add_waiting_screen(9);//tool choice
  centre_p->screen_done=true;
}

void start_screen::slave_mode_clicked()
{
  centre_p->add_waiting_screen(60);//slave mode
  centre_p->screen_done=true;
}

void start_screen::eject_memory_sticks_clicked()
{
  QDir dir("/media/odroid/");
  QStringList directory_list = dir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
  int num_drives = directory_list.size();//number of USB memory sticks detected
  for (int i=0; i<num_drives; ++i)
  {
    QString command = "umount /media/odroid/";
    command.append(directory_list[i]);
    char* command_a = command.toLatin1().data();
    if(system (command_a) != 0)
    {
      cout<<"unmount command failed\n";
    }
  }
  QDir check_dir("/media/odroid/");
  QStringList check_list = check_dir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
  if(check_list.size() == 0) 
  {
    QMessageBox box;
    box.setText(QString("%1 USB stick(s) can be safely removed").arg(num_drives));
    box.exec();
  }
  else
  {
    QMessageBox box;
    box.setText(QString("WARNING - %1 USB stick(s) were NOT unmounted.  Shut down machine to remove them safely.").arg(check_list.size()));
    box.exec();
  }
}

void start_screen::help_button_clicked()
{
  help_screen_p = new help_screen;
  
  help_screen_p -> set_text("TOTALIZE MODE - This counts the number of seeds in existing samples.  "
                            "Counts are stored in a table, which you can save as a .csv file and "
                            "open with a spreadsheet or database.  "
                            "You can also record scanned barcodes.\n\n"
                            
                            "BATCH MODE - You can set up packet filling programs.  "
                            "You enter the number of packets and the number of seeds.  "
                            "Multiple entries of this sort are supported.  "
                            "There are options for barcode checking.  "
                            "At the end of a program, remaining seed is dumped.\n\n"
                            
                            "BATCH FROM SPREADSHEET MODE - This fills packets from a spreadsheet, which "
                            "should be in .csv format on a USB memory stick plugged into the machine.  "
                            "Each row of the spreadsheet represents one pack, except the top row, "
                            "which should be column headings.  "
                            "There must be columns to fill the following roles: a column of material identification codes, "
                            "a column of numbers which are the required number of seeds for the packet, "
                            "and a column of unique identifiers for each packet.  "
                            "You can choose columns to fill these roles, so order is not important.  "
                            "You can also choose columns for other optional roles.\n\n"
                            
                            "In operation, you scan a material identification code from your seed lot.  "
                            "The program picks all rows from your spreadsheet with that code for filling.  "
                            "After all packets are filled, remaining seed dumps automatically.\n\n"
                            
                            "TOOLS - You can add, remove, or modify the crops that the machine has been "
                            "calibrated for.  "
                            "The DIAGNOSTICS choice is mostly for program development.  "
                            "You can also exit the ESC-3 program, giving access to other features "
                            "of the Linux-based computer that runs it, like the file system.  "
                            "You can hook up a mouse and keyboard, and, if you like, a larger monitor.  "
                            "Double-click the ESC-3 icon to re-start the ESC-3 program.");
  
  help_screen_p -> show();
}

void start_screen::shutdown()
{
  centre_p->~centre();
  centre_p->application_p->exit(1);
}
/*
void start_screen::exit_program()
{
  centre_p->~centre();
  exit(0);
}
*/
