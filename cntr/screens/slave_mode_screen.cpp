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

#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QFileInfo>
#include <QTextEdit>
#include <QTimer>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <QDir>
#include <fcntl.h>	//library used to use system call command "open()" used to check available serial
#include <unistd.h>	//library to enable write() function
#include <termios.h>
#include "centre.hpp"
#include "slave_mode_screen.hpp"
#include "button.hpp"
#include "help_screen.hpp"
#include "crop.hpp"
#include "endgate.hpp"
#include "cutgate.hpp"

using namespace std;

slave_mode_screen::slave_mode_screen(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  help_screen_p = 0;
  envelope_sensor_p = centre_p->envelope_sensor_p;
  
  count_label_p = new QLabel;
  back_button_p = new button("Back");
  function_as_server_1_button_p = new button("Function as TCP Server Using 192.168.100.1");
  function_as_server_2_button_p = new button("Function as TCP Server Using 192.168.200.1");
  function_as_client_button_p = new button("Function as TCP Client");
  communicate_by_serial_port_button_p = new button("Communicate by Serial Port");
  opcua_button_p = new button("Communicate by OPC UA");
  help_button_p = new button("Help");
  exit_button_p = new button("Exit Slave Mode");
  connection_message_p = new QLabel("Choose communication method");
  command_message_p = new QLabel;
  command_screen_p = new QTextEdit;
  display_data_requests_p = new QRadioButton("Display data requests");
  display_data_requests_p->setCheckable(true);
  if(centre_p->tcp_link_established)
  {
    connection_message_p->setText("Set up to communicate by TCP.");
  }

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(count_label_p,0,0);
  main_layout_p->addWidget(back_button_p,0,1,1,2);
  main_layout_p->addWidget(function_as_server_1_button_p, 1, 0);
  main_layout_p->addWidget(function_as_server_2_button_p, 2, 0);
  main_layout_p->addWidget(function_as_client_button_p, 3, 0);
  main_layout_p->addWidget(communicate_by_serial_port_button_p, 4, 0);
//  main_layout_p->addWidget(opcua_button_p, 5, 0);
  main_layout_p->addWidget(help_button_p, 6, 0);
  main_layout_p->addWidget(connection_message_p, 1, 1, 1, 2);
  main_layout_p->addWidget(command_screen_p, 2, 1, 3, 2);
  main_layout_p->addWidget(display_data_requests_p, 5, 1);
  main_layout_p->addWidget(command_message_p, 5, 2);
  main_layout_p->addWidget(exit_button_p, 6, 2);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(function_as_server_1_button_p, SIGNAL(clicked()), this, SLOT(function_as_server_1_clicked()));
  connect(function_as_server_2_button_p, SIGNAL(clicked()), this, SLOT(function_as_server_2_clicked()));
  connect(function_as_client_button_p, SIGNAL(clicked()), this, SLOT(function_as_client_clicked()));
  connect(communicate_by_serial_port_button_p, SIGNAL(clicked()), this, SLOT(communicate_by_serial_port_clicked()));
  connect(opcua_button_p, SIGNAL(clicked()), this, SLOT(opcua_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(exit_button_p, SIGNAL(clicked()), this, SLOT(exit_button_clicked()));
  connect(centre_p, SIGNAL(tcp_connection_detected_signal()), this, SLOT(connection_detected()));
  connect(centre_p, SIGNAL(char_from_tcp(QChar)), this, SLOT(command_char(QChar)));
  connect(centre_p, SIGNAL(char_from_serial_port(QChar)), this, SLOT(command_char(QChar)));
  connect(centre_p, SIGNAL(char_from_serial_port(QChar)), this, SLOT(opcua_command_char(QChar)));
  
  connect(batch_mode_driver_p, SIGNAL(slave_mode_program_finished()), this, SLOT(slave_mode_program_finished()));
  connect(batch_mode_driver_p, SIGNAL(pack_ready()), this, SLOT(pack_ready()));
//  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  
  executing_command_p = 0;
  previous_command_p = 0;
  pack_count = 0;
  pack_ready_bool = false;
  speed = 0;
  remembered_speed = 20;
  command_finished_bool = false;
  batch_mode = false;
  
  end_gate_full = false;
  end_gate_opened_full = false;
  end_gate_filling = true;
  
  batch_mode_driver_p -> dump_automatically = false;
  
  opcua_mode = 0;
  opcua_count = 0;
  just_starting = true;//wish to ignore any command on opc device when starting.  enter command only if changed.
  opcua_message_to_write = false;
  mode_to_start = entry;

  timer_p = new QTimer;
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  timer_p->start(500);
}

slave_mode_screen::~slave_mode_screen()
{
  timer_p -> stop();
  delete timer_p;
  batch_mode_driver_p->slave_mode = false;
  batch_mode_driver_p->stop();
  centre_p->set_speed(0);
}

void slave_mode_screen::pack_ready()
{
  cout<<"slave_mode_screen::pack_ready\n";
  pack_ready_bool = true;
}

void slave_mode_screen::dump_complete(int)
{
  QByteArray array;
  array.append(QChar(2));
  array.append('c');
  array.append(QChar(31));
  array.append(QString("BatchF"));
  array.append(QChar(31));
  array.append(QChar(3));
  if(centre_p->communicate_by_tcp)
  {
    centre_p->tcp_socket_p->write(array);
  }
  else if(centre_p->communicate_by_serial_port)
  {
    centre_p->serial_port_write(QString(array));
  }
  else if(centre_p->communicate_by_opcua)
  {
    opcua_message = "Dump Complete";
    opcua_message_to_write = true;
  }
  else cout<<"no communication method for slave mode\n";
}

void slave_mode_screen::slave_mode_program_finished()  
{
  end_command();
}

void slave_mode_screen::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void slave_mode_screen::function_as_server_1_clicked()
{
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = true;
  centre_p->communicate_by_serial_port = false;
  centre_p->communicate_by_opcua = false;
  QString message_string = centre_p->choose_tcp_network(1);
  if(message_string=="")//success
  {
    connection_message_p->setText("Listening for connection to\naddress 192.168.100.1");
    return;
  }
  connection_message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void slave_mode_screen::function_as_server_2_clicked()
{
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = true;
  centre_p->communicate_by_serial_port = false;
  centre_p->communicate_by_opcua = false;
  QString message_string = centre_p->choose_tcp_network(2);
  if(message_string=="")//success
  {
    connection_message_p->setText("Listening for connection to\naddress 192.168.200.1");
    return;
  }
  connection_message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void slave_mode_screen::function_as_client_clicked()
{
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = true;
  centre_p->communicate_by_serial_port = false;
  centre_p->communicate_by_opcua = false;
  centre_p->add_waiting_screen(60);//come back here when done
  centre_p->add_waiting_screen(42);//tcp_client_server_addr_entry
  centre_p->screen_done = true;
}

void slave_mode_screen::communicate_by_serial_port_clicked()
{
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = false;
  centre_p->communicate_by_serial_port = true;
  centre_p->communicate_by_opcua = false;
  centre_p->add_waiting_screen(60);//come back here
  centre_p->add_waiting_screen(61);//serial_port_setup
  centre_p->screen_done = true;
}  

void slave_mode_screen::opcua_clicked()
{
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = false;
  centre_p->communicate_by_serial_port = false;
  centre_p->communicate_by_opcua = true;
  connection_message_p->setText("Set to communicate with Agriculex\nOPC UA device");
}

void slave_mode_screen::help_button_clicked()
{
  help_screen_p = new help_screen;
  help_screen_p -> setGeometry(geometry());
  help_screen_p -> set_text(
                            "The ESC-3 has 2 basic types of communication.  "
                            "The first is 1-way communication that can be used in all totalize and batch modes.  "
                            "This sends out data at various points, in ways that you can control flexibly.  "
                            "You can set it up from the totalize and batch screens.  "
                            "\n\n"
                            
                            "This screen allows you to set up 2-way communication between the ESC-3 and a computer, PLC, etc.  "
                            "Commands can be sent from the other device to control the ESC-3.  "
                            "You can send a command to make the machine count out a series of packets with a "
                            "different number of seeds in each, for example.  "
                            "You can also command the ESC-3 to send out data that can be read by the other device.  "
                            "See the manual for details on the commands to be used and the data sent out. "
                            "On this screen, you can choose the communication method.  "
                            "\n\n"
                            
                            "The first 3 communication options all involve communicating by TCP (transmission control protocol) "
                            "using a connection by ethernet cable.  This requires a program on the other computer "
                            "capable of communicating using this method.  The three options all result in the same thing - "
                            "a TCP link to the other computer.  The options merely control the method by which the link "
                            "is established.\n\n"
                            
                            "FUNCTION AS TCP SERVER USING ... - With these two options, the ESC-3 will 'listen' for a connection "
                            "from the other computer, which should connect to the address chosen.\n\n"
                            
                            "FUNCTION AS TCP CLIENT - With this option, the other device must function as a server, and 'listen' "
                            "for a connection from the ESC-3.  You will enter the IP address it will connect to later. \n\n"
                            
                            "With all three TCP methods, the result is the same - a TCP connection to the other computer.\n\n"
                            
                            "The final communication option is by serial port.  "
                            "The easiest way to use it is with the serial port cable from Agriculex.  "
                            "It might well also be possible to use a different USB-serial converter.  "
                            "Contact Agriculex if you want to attempt this.  "
                            "NOTE - YOU DO NEED TO USE A USB-SERIAL CONVERTER.  "
                            "DO NOT TRY TO USE THE 9-PIN CONNECTORS ON THE BACK OF THE ESC-3.  "
                            "THEY ARE NOT SERIAL PORTS AND MIGHT DAMAGE SERIAL PORT DEVICES.\n\n"
                            
                            "We would generally recommend using serial port connection, at least for short distance communication."
                            "Once the connection is correctly set up, it generally re-establishes itself automatically if the computer and/or the ESC-3 are re-started."
                            "This is not always the case if using TCP.");
  help_screen_p -> show();
}

void slave_mode_screen::connection_detected()
{
  if(centre_p->network == 1)
  {
    connection_message_p->setText("Connected using address 192.168.100.1");
  }
  if(centre_p->network == 2)
  {
    connection_message_p->setText("Connected using address 192.168.200.1");
  }
  centre_p->communicate_by_tcp = true;
  centre_p->tcp_link_established = true;
  centre_p->communicate_by_serial_port = false;
  centre_p->communicate_by_opcua = false;
}
  
void slave_mode_screen::exit_button_clicked() 
{ 
  centre_p->add_waiting_screen(0);//start_screen
  centre_p->screen_done = true;
}

void slave_mode_screen::command_char(QChar character)
{
  if(character==2)//start of new command
  {
    command_line_string.clear();
    return;
  }
  if(character==3)//end of command.  create slave_mode_command from command_line_string and add it to command_p_list for execution.
  {
    QStringList list = command_line_string.split(QChar(31));
    new_command_p = new slave_mode_command;
    bool ok=false;
    if(list.size()<2)
    {
      cout<<"list size <2\n";
      return;
    }
    QString display_string;
    if(  (list[0]=="P")  &&  (list.size()>2)  ) //program
    {
      display_string = list[0] + "  Program " + list[1];

      display_string.append("     "+list[2]+" sets\n");
      for(int i=3; i<list.size()-5; ++i)
      {
        if(i%2 == 1)
        {
          display_string.append("          "+list[i]+" seeds   ");
        }
        else
        {
          display_string.append(list[i]+" packs\n");
        }
      }
      display_string.append("     Speed "+list[list.size()-2]+"\n");
      display_string.append("     "+list[list.size()-1]+"\n");
    }
    else
    {
      display_string = list[0] + "     " + list[1];
    }
    
    QString type_flag_string = list[0];
    if(type_flag_string.size() != 1)
    {
      cout<<"type_flag_string format error\n";
      return;
    }
    new_command_p->type_flag = type_flag_string[0];

    if(new_command_p->type_flag == 'D')
    {
      if(display_data_requests_p->isChecked())
      {
        command_screen_p->append(display_string);
      }
    }
    else
    {
      command_screen_p->append(display_string);
    }

    if(new_command_p->type_flag == 'C')
    {
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = 0;
      new_command_p->command = list[1];
    }
    else if(new_command_p->type_flag == 'M')
    {
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = list[1].toInt(&ok);
      if(ok == false)
      {
        cout<<"speed conversion error\n";
        cout<<"list[1] = "<<list[1].toStdString()<<endl;
        return;
      }
      new_command_p->command = "Set_speed";
    }
    else if(new_command_p->type_flag == 'P')
    {
      new_command_p->crop_name=list[1];
      if(list.size()<3)
      {
        cout<<"list size <3\n";
        return;
      }
      new_command_p->number_of_sets = list[2].toInt(&ok);
      if(ok == false)
      {
        cout<<"number_of_sets conversion error\n";
        return;
      }
      int expected_list_size = (new_command_p->number_of_sets)*2 + 8;
      if(list.size() != expected_list_size)
      {
        cout<<"inconsistent number_of_sets\n";
        return;
      }
      for(int i=0; i<new_command_p->number_of_sets; ++i)
      {
        int seeds_per_pack = list[3+i*2].toInt(&ok);
        if(ok == false)
        {
          cout<<"seeds_per_pack conversion error\n";
          return;
        }
        new_command_p->seeds_per_pack.append(seeds_per_pack);
        int number_of_packs = list[4+i*2].toInt(&ok);
        if(ok == false)
        {
          cout<<"number_of_packs conversion error\n";
          return;
        }
        new_command_p->number_of_packs.append(number_of_packs);
      }
      new_command_p->speed = list[2*(new_command_p->number_of_sets) + 6].toInt(&ok);
      if(ok == false)
      {
        cout<<"speed conversion error\n";
        return;
      }
      new_command_p->command = list[2*(new_command_p->number_of_sets) + 7];
    }
    else if(  (new_command_p->type_flag=='D')  ||  (new_command_p->type_flag=='I')  )
    {
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = 0;
      new_command_p->command = list[1];
    }
    
    else
    {
      cout<<"type flag not recognized\n";
      return;
    }
    if(new_command_p->command == "Reset")//this command needs immediate action.  Other commands join queue for action when previous command finished.
    {
      centre_p->count = 0;
      centre_p->set_speed(0);
      batch_mode_driver_p -> high_feed_speed = 0;
      batch_mode_driver_p -> low_feed_speed = 0;
      batch_mode_driver_p -> dump_speed = 1000;
      batch_mode_driver_p -> cutgate_set = 0;
      batch_mode_driver_p -> cutgate_pack = 0;
      batch_mode_driver_p -> cutgate_count_limit = 0;
      batch_mode_driver_p -> cutgate_pack_limit = 0;
      batch_mode_driver_p->slave_mode = false;
      
      for(int i=0; i<command_p_list.size(); ++i)
      {
        if(command_p_list[i] != 0)
        {
          delete command_p_list[i];
          command_p_list[i] = 0;
        }
      }
      command_p_list.clear();
      if(executing_command_p != 0)
      {
        delete executing_command_p;
        executing_command_p = 0;
      }
      if(previous_command_p != 0)
      {
        delete previous_command_p;
        previous_command_p = 0;
      }
      return;
    }
    
    if(  (new_command_p->type_flag == 'D')  ||  (new_command_p->type_flag == 'I')  )//these commands need immediate action
    {
      if(new_command_p->command == "PackSet")
      {
        envelope_sensor_p->software_trigger();
        return;
      }

      QByteArray array;
      if(new_command_p->command == "Count")
      {
        array.append(QChar(2));
        array.append('c');
        array.append(QChar(31));
        array.append(QString::number(centre_p->count));
        array.append(QChar(3));
      }
      if(new_command_p->command == "Status")
      {
        array.append(QChar(2));
        array.append('t');
        array.append(QChar(31));
        array.append("Ready");
        array.append(QChar(3));
      }
      if(new_command_p->command == "Pack")
      {
        array.append(QChar(2));
        array.append('c');
        array.append(QChar(31));
        array.append(QString::number(batch_mode_driver_p->cutgate_pack));
        array.append(QChar(3));
      }

      if(new_command_p->command == "PStatus")
      {
        array.append(QChar(2));
        array.append('s');
        array.append(QChar(31));
        if(pack_ready_bool)
        {
          array.append("Full");
          pack_ready_bool = false;
        }
        else
        {
          array.append("Filling");
        }
        array.append(QChar(3));
      }
      if(centre_p->communicate_by_tcp)
      {
        if(centre_p->tcp_link_established)
        {
          centre_p->tcp_socket_p->write(array);
        }
      }
      else if(centre_p->communicate_by_serial_port)
      {
        centre_p->serial_port_write(QString(array));
      }
      return;//at end of command requiring immediate action.  do not enqueue
    }
      
    command_p_list.enqueue(new_command_p);
    return;
  }
  //if gets to here, character is neither 2 nor 3
  command_line_string.append(character);
}

void slave_mode_screen::opcua_command_char(QChar character)
{
  opcua_line.append(character);
}

void slave_mode_screen::run()
{
  count_label_p->setText(QString("Count: ") + (QString::number(centre_p->count)));
  if(batch_mode_driver_p->mode == wait_for_pack)
  {
    if(end_gate_filling == true)
    {
      end_gate_filling = false;
      end_gate_full = true;
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_gate_full = false;
        end_gate_opened_full = true;
      }
    }
    else if(end_gate_full == true)
    {
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_gate_full = false;
        end_gate_opened_full = true;
      }
    }
    else if(end_gate_opened_full == true)
    {
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_gate_opened_full = false;
        end_gate_closed_empty = true;
      }
    }
    else if(end_gate_closed_empty == true)
    {
      end_gate_closed_empty = false;
      end_gate_filling = true;
    }
  }

  if(centre_p->communicate_by_serial_port == true)
  {
    QString messg("Set to communicate by serial port\n");
    if(centre_p->serial_port_opened == true)
    {
      messg.append("Serial port adapter cable detected.");
    }
    else
    {
      messg.append("NO SERIAL PORT ADAPTER CABLE DETECTED.");
    }
    connection_message_p->setText(messg);
  }
  
  if(centre_p->communicate_by_opcua == true)
  {
    connection_message_p->setText("Set to communicate with Agriculex\nOPC UA device");
    run_opcua();
  }

  if(executing_command_p == 0)
  {
    if(command_p_list.size() == 0) return;
    executing_command_p = command_p_list.dequeue();
    command_finished_bool = false;
    emit new_slave_mode_command();
  }
  if(executing_command_p->type_flag == 'C')
  {
    if(executing_command_p->command == "Start")
    {
      centre_p->set_speed(remembered_speed);
      end_command();
      return;
    }
    if(executing_command_p->command == "Stop")
    {
      centre_p->set_speed(0);
      end_command();
      return;
    }
    if(executing_command_p->command == "Zero")
    {
      centre_p->count = 0;
      end_command();
      return;
    }
    if(executing_command_p->command == "Dump")
    {
      if(batch_mode_driver_p->slave_mode == false)//execution of this command is starting.  set-up needed.
      {
        batch_mode_driver_p->chamber_count_limit_calculation();
        batch_mode_driver_p->start();
        batch_mode_driver_p->slave_mode = true;
        batch_mode_driver_p->dump_speed = 1000;
        batch_mode_driver_p->restart();
      }
      if(command_finished_bool == true)
      {
        end_command();
        return;
      }
    }
    if(executing_command_p->command == "Recycle")
    {
      executing_command_p = previous_command_p;
      previous_command_p = 0;
      command_finished_bool = false;
      return;
    }

    if(executing_command_p->command == "OpenCut")
    {
      centre_p->cutgate_p->open();
      end_command();
      return;
    }
    if(executing_command_p->command == "CloseCut")
    {
      centre_p->cutgate_p->close();
      end_command();
      return;
    }
    if(executing_command_p->command == "OpenEnd")
    {
      centre_p->endgate_p->open();
      end_command();
      return;
    }
    if(executing_command_p->command == "CloseEnd")
    {
      centre_p->endgate_p->close();
      end_command();
      return;
    }
  }
  if(executing_command_p->type_flag == 'M')
  {
    speed = executing_command_p->speed;
    batch_mode_driver_p->high_feed_speed = speed;
    batch_mode_driver_p->low_feed_speed = speed/10;
    centre_p->set_speed(speed);
    remembered_speed = speed;
    end_command();
    return;
  }
  if(executing_command_p->type_flag == 'P')
  {
    if(command_finished_bool == true)
    {
      cout<<"ending command\n";
      end_command();
      return;
    }
    if(  (executing_command_p->number_of_packs.size()!=executing_command_p->number_of_sets)  ||  (executing_command_p->seeds_per_pack.size()!=executing_command_p->number_of_sets)  )
    {
      cout<<"inconsistent number of sets\n";
    }
    if(batch_mode_driver_p->slave_mode == false)//execution of this command is starting.  set-up needed.
    {
      batch_mode_driver_p->slave_mode = true;//do not do set-up on subsequent passes
      for(int i=0; i<100; ++i)
      {
        if(centre_p->crops[i].name == executing_command_p->crop_name)
        {
          centre_p->set_crop_f(centre_p->crops[i]);
          cout<<"set crop to "<<(executing_command_p->crop_name).toStdString()<<endl;
          break;
        }
      }
      speed = executing_command_p->speed;
      centre_p->set_speed(executing_command_p->speed);
      batch_mode_driver_p->high_feed_speed = executing_command_p->speed;
      batch_mode_driver_p->low_feed_speed = (executing_command_p->speed) / 10;
      if(executing_command_p->number_of_sets > 0)//batch mode
      {
        batch_mode_driver_p->clear_program();
        for(int i=0; i<executing_command_p->number_of_sets; ++i)
        {
          int packs = executing_command_p->number_of_packs[i];
          int seeds = executing_command_p->seeds_per_pack[i];
          batch_mode_driver_p->add_line(packs, seeds);
        }
        batch_mode_driver_p -> seed_lot_barcode = "";
        batch_mode_driver_p -> pack_barcode = "";
        batch_mode_driver_p -> use_spreadsheet = false;
        batch_mode_driver_p->require_seed_lot_barcode = false;
        batch_mode_driver_p->require_pack_barcode = false;
        batch_mode_driver_p->pack_match_lot = false;
        batch_mode_driver_p->pack_contain_lot = false;
        batch_mode_driver_p->lot_contain_pack = false;
        batch_mode_driver_p->pack_match_spreadsheet = false;
        batch_mode_driver_p->record_only = true;
        batch_mode_driver_p->chamber_count_limit_calculation();
        batch_mode_driver_p->reset_program();
        
        batch_mode_driver_p->mode = mode_to_start;
        batch_mode_driver_p->start();
      }
      envelope_sensor_p->software_trigger();
    }
    return;
  }
}

void slave_mode_screen::run_opcua()
{
  
  cout<<"slave_mode_screen::run_opcua.  opcua_mode = "<<opcua_mode<<endl;
  
  QByteArray array;
  if(opcua_mode == 0)//send count to opcua device
  {
    array.clear();
    opcua_line.clear();
    array.append("OPCUAW.001=");
    array.append(QString::number(centre_p->count));
    array.append("          ");
    centre_p->serial_port_write(QString(array));
    opcua_mode = 1;
    opcua_count = 0;
    return;
  }
  if(opcua_mode == 1)//wait for response from device
  {
    ++opcua_count;
    if(opcua_count > 10)//timeout
    {
      connection_message_p->setText(QString("OPCUA device not responding."));
      cout<<"opcua_line = "<<opcua_line.toStdString()<<endl;
    }
    if(opcua_line.startsWith("count="))//response started, but might not be complete
    {
      connection_message_p->setText("Set to communicate with Agriculex\nOPC UA device");
      opcua_count = 0;
      opcua_mode = 2;
    }
    return;
  }
  if(opcua_mode == 2)//wait to make sure response complete
  {
    ++opcua_count;
    if(opcua_count > 3)//assume complete
    {
      opcua_mode = 3;
    }
    return;
  }
  if(opcua_mode == 3)//send pack number to opcua device
  {
    array.clear();
    opcua_line.clear();
    array.append("OPCUAW.002=");
    array.append(QString::number(batch_mode_driver_p->cutgate_pack));
    array.append("          ");
    centre_p->serial_port_write(QString(array));
    opcua_mode = 4;
    opcua_count = 0;
    return;
  }
  if(opcua_mode == 4)//wait for response from device
  {
    ++opcua_count;
    if(opcua_count > 10)//timeout
    {
      connection_message_p->setText(QString("OPCUA device not responding."));
    }
    if(opcua_line.startsWith("pack number="))//response started, but might not be complete
    {
      connection_message_p->setText("Set to communicate with Agriculex\nOPC UA device");
      opcua_count = 0;
      opcua_mode = 5;
    }
    return;
  }
  if(opcua_mode == 5)//wait to make sure response complete
  {
    ++opcua_count;
    if(opcua_count > 3)//assume complete
    {
      opcua_mode = 6;
    }
    return;
  }
  if(opcua_mode == 6)//send request for command
  {
    opcua_line.clear();
    centre_p->serial_port_write("OPCUAR.003");//request command string from opcua device
    opcua_count = 0;
    opcua_mode = 7;
    return;
  }
  if(opcua_mode == 7)//wait for command
  {
    ++opcua_count;
    if(opcua_count > 10)//timeout
    {
      connection_message_p->setText(QString("OPCUA device not responding."));
    }
    if(opcua_line.startsWith("command:="))//response started, but might not be complete
    {
      connection_message_p->setText("Set to communicate with Agriculex\nOPC UA device");
      opcua_count = 0;
      opcua_mode = 8;
    }
    return;
  }
  if(opcua_mode == 8)//wait to make sure response complete
  {
    ++opcua_count;
    if(opcua_count > 3)//assume complete
    {
      opcua_mode = 9;
    }
    return;
  }
  if(opcua_mode == 9)//interpret command
  {
    opcua_line.remove(0,9);//remove "command:=" from start of line
    opcua_line.trimmed();//remove possible whitespace from ends
    if(opcua_line == old_command)//command same as before.  ignore.
    {
      if(opcua_message_to_write == true)
      {
        opcua_mode = 10;
        opcua_message_to_write = false;
      }
      else
      {
        opcua_mode = 0;
      }
      return;
    }
    old_command = opcua_line;
    if(just_starting == true)//wish to ignore any command on opc device when starting.  enter command only if changed.
    {
      just_starting = false;
      opcua_mode = 0;
      return;
    }
    QString command_line = opcua_line;
    new_command_p = new slave_mode_command;
    if(opcua_line == "start")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "Start";
    }
    else if(opcua_line == "stop")
    {
      cout<<"opcua_line == stop\n";
      delete new_command_p;
      new_command_p = 0;
      delete executing_command_p;
      executing_command_p = 0;
      centre_p->set_speed(0);
      command_message_p->setText(QString("Command ") + command_line + QString(" received"));
      opcua_mode= 0;
      batch_mode_driver_p->mode = entry;
      batch_mode_driver_p->stop();
      return;
    }
    else if(opcua_line == "zero")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "Zero";
    }
    else if(opcua_line == "dump")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = 1000;
      new_command_p->command = "Dump";
    }
    else if(opcua_line == "repeat")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "Recycle";
    }
    else if(opcua_line == "reset")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "Reset";
    }
    else if(opcua_line == "open cut gate")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "OpenCut";
    }
    else if(opcua_line == "close cut gate")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "CloseCut";
    }
    else if(opcua_line == "open end gate")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "OpenEnd";
    }
    else if(opcua_line == "close end gate")
    {
      new_command_p->type_flag = 'C';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      new_command_p->speed = speed;
      new_command_p->command = "CloseEnd";
    }
    else if(opcua_line.startsWith("speed"))
    {
      new_command_p->type_flag = 'M';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->number_of_sets = 0;
      bool conversion_result = false;
      int conversion_value = 0;
      opcua_line.remove(0, 5);
      conversion_value = opcua_line.toInt(&conversion_result);
      if(conversion_result == false)
      {
        command_message_p->setText(QString("Failed to set speed.\nFormat eg.: speed 50"));
        delete new_command_p;
        new_command_p = 0;
        opcua_mode = 0;
        return;
      }
      speed = conversion_value;
      new_command_p->speed = conversion_value;
      new_command_p->command = "";
    }
    else if(opcua_line.startsWith("program"))
    {
      new_command_p->type_flag = 'P';
      new_command_p->crop_name = centre_p->crops[0].name;
      new_command_p->speed = speed;
      int number_of_sets = opcua_line.count("packs");
      bool program_error = false;
      if(opcua_line.count("seeds") != number_of_sets)
      {
        command_message_p->setText(QString("Incorrect program format.\nFormat eg.: program 5 packs 100 seeds 10 packs 50 seeds"));
        delete new_command_p;
        new_command_p = 0;
        opcua_mode = 0;
        return;
      }
      new_command_p->number_of_sets = number_of_sets;
      new_command_p->command = "program";
      
      bool conversion_result = false;
      QStringList list = opcua_line.split(" ");
      int list_index = 1;//list[0] will contain "program"
      while(1)
      {
        int opc_packs = list[list_index].toInt(&conversion_result);
        if(conversion_result == false)
        {
          program_error = true;
          break;
        }
        ++ list_index;
        if(list_index >= list.size())
        {
          program_error = true;
          break;
        }
        if(list[list_index] != "packs")
        {
          program_error = true;
          break;
        }
        ++list_index;
        if(list_index >= list.size())
        {
          program_error = true;
          break;
        }
        int opc_seeds = list[list_index].toInt(&conversion_result);
        if(conversion_result == false)
        {
          program_error = true;
          break;
        }
        ++ list_index;
        if(list_index >= list.size())
        {
          program_error = true;
          break;
        }
        if(list[list_index] != "seeds")
        {
          program_error = true;
          break;
        }
        
        new_command_p->number_of_packs.append(opc_packs);
        new_command_p->seeds_per_pack.append(opc_seeds);
        
        ++list_index;
        if(list_index >= list.size())//end of program
        {
          break;
        }
      }
      if(program_error == true)
      {
        command_message_p->setText(QString("Incorrect program format.\nFormat eg.: program 5 packs 100 seeds 10 packs 50 seeds"));
        delete new_command_p;
        new_command_p = 0;
        opcua_mode = 0;
        return;
      }
    }
    else if(opcua_line == "clear")
    {
      delete new_command_p;
      new_command_p = 0;
      command_message_p->setText(QString("Command ") + command_line + QString(" received"));
      opcua_mode= 0;
      return;
    }
    else
    {
      command_message_p->setText(QString("Unknown command ") + opcua_line);
      cout<<"unknown command = "<<opcua_line.toStdString()<<endl;
      delete new_command_p;
      new_command_p = 0;
      opcua_mode = 0;
      return;
    }
    //following only runs if command recognized
    command_message_p->setText(QString("Command ") + command_line + QString(" received"));
    command_p_list.enqueue(new_command_p);
    return;
  }
  if(opcua_mode == 10)//send message to opcua device
  {
    array.clear();
    opcua_line.clear();
    array.append("OPCUAW.004=");
    array.append(opcua_message);
    array.append("          ");
    centre_p->serial_port_write(QString(array));
    opcua_mode = 11;
    opcua_count = 0;
    return;
  }
  if(opcua_mode == 11)//wait for response from device
  {
    ++opcua_count;
    if(opcua_count > 10)//timeout
    {
      connection_message_p->setText(QString("OPCUA device not responding."));
      cout<<"opcua_line = "<<opcua_line.toStdString()<<endl;
    }
    if(opcua_line.startsWith("message="))//response started, but might not be complete
    {
      connection_message_p->setText("Set to communicate with Agriculex\nOPC UA device");
      opcua_count = 0;
      opcua_mode = 12;
    }
    return;
  }
  if(opcua_mode == 12)//wait to make sure response complete
  {
    ++opcua_count;
    if(opcua_count > 3)//assume complete
    {
      opcua_mode = 0;
    }
    return;
  }

  //following should not execute
  cout<<"opcua_mode not found "<<opcua_mode<<endl;
}        

void slave_mode_screen::end_command()
{
  if(previous_command_p != 0)
  {
    delete previous_command_p;
  }
  previous_command_p = executing_command_p;
  executing_command_p = 0;
  batch_mode_driver_p->slave_mode = false;
  mode_to_start = entry;
  return;
}

void slave_mode_screen::display_command(slave_mode_command* command_p)
{
  QString string;
  
  string.append("type flag: ");
  string.append(command_p->type_flag);
  string.append("  crop: ");
  string.append(command_p->crop_name);
  string.append("  sets: ");
  string.append(QString::number(command_p->number_of_sets));
  for(int i=0; i<command_p->seeds_per_pack.size() && i<command_p->number_of_packs.size(); ++i)
  {
    string.append("\n  seeds: ");
    string.append(QString::number(command_p->seeds_per_pack[i]));
    string.append("   packs: ");
    string.append(QString::number(command_p->number_of_packs[i]));
  }
  string.append("\nspeed: ");
  string.append(QString::number(command_p->speed));
  string.append("   command: ");
  string.append(command_p->command);
  
  command_screen_p->append(string);
}

