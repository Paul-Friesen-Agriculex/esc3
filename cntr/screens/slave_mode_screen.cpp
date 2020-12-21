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
//  cout<<"slave_mode_screen::slave_mode_screen 1\n";
  
  batch_mode_driver_p = set_batch_mode_driver_p;
  help_screen_p = 0;
  
  back_button_p = new button("Back");
  function_as_server_1_button_p = new button("Function as TCP Server Using 192.168.100.1");
  function_as_server_2_button_p = new button("Function as TCP Server Using 192.168.200.1");
  function_as_client_button_p = new button("Function as TCP Client");
  help_button_p = new button("Help");
  exit_button_p = new button("Exit Slave Mode");
  connection_message_p = new QLabel("Choose communication method");
  command_screen_p = new QTextEdit;
  if(centre_p->tcp_link_established)
  {
    connection_message_p->setText("Set up to communicate by TCP.");
  }

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(function_as_server_1_button_p, 1, 0);
  main_layout_p->addWidget(function_as_server_2_button_p, 2, 0);
  main_layout_p->addWidget(function_as_client_button_p, 3, 0);
  main_layout_p->addWidget(help_button_p, 6, 0);
  main_layout_p->addWidget(connection_message_p, 1, 1);
  main_layout_p->addWidget(command_screen_p, 2, 1, 3, 1);
  main_layout_p->addWidget(exit_button_p, 6, 1);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(function_as_server_1_button_p, SIGNAL(clicked()), this, SLOT(function_as_server_1_clicked()));
  connect(function_as_server_2_button_p, SIGNAL(clicked()), this, SLOT(function_as_server_2_clicked()));
  connect(function_as_client_button_p, SIGNAL(clicked()), this, SLOT(function_as_client_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(exit_button_p, SIGNAL(clicked()), this, SLOT(exit_button_clicked()));
  connect(centre_p, SIGNAL(tcp_connection_detected_signal()), this, SLOT(connection_detected()));
  connect(centre_p, SIGNAL(char_from_tcp(QChar)), this, SLOT(command_char(QChar)));
  connect(batch_mode_driver_p, SIGNAL(slave_mode_set_finished()), this, SLOT(command_finished()));
  connect(batch_mode_driver_p, SIGNAL(pack_ready()), this, SLOT(pack_ready()));
  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  
//  command_input_mode = wait_start;
//  command_type_flag = 'X';//invalid
//  waiting_for_delimiter = false;
//  int_to_build = 0;
  executing_command_p = 0;
  previous_command_p = 0;
  pack_count = 0;
  speed = 0;
  command_finished_bool = false;
  batch_mode = false;
  
  end_gate_full = false;
  end_gate_opened_full = false;
  end_gate_filling = true;
  
  
  timer_p = new QTimer;
  connect(timer_p, SIGNAL(timeout()), this, SLOT(run()));
  timer_p->start(100);
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
  QByteArray array;
  array.append(QChar(2));
  array.append('c');
  array.append(QChar(31));
  array.append(QString("Full"));
  array.append(QChar(31));
  array.append(QChar(3));
  centre_p->tcp_socket_p->write(array);
}
  
void slave_mode_screen::pack_collected(int)
{
  QByteArray array;
  array.append(QChar(2));
  array.append('c');
  array.append(QChar(31));
  array.append(QString("Empty"));
  array.append(QChar(31));
  array.append(QChar(3));
  centre_p->tcp_socket_p->write(array);
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
  centre_p->tcp_socket_p->write(array);
}
  
void slave_mode_screen::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void slave_mode_screen::function_as_server_1_clicked()
{
  QString message_string = centre_p->choose_tcp_network(1);
  if(message_string=="")//success
  {
    connection_message_p->setText("Listening for connection to address 192.168.100.1");
    return;
  }
  connection_message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void slave_mode_screen::function_as_server_2_clicked()
{
  QString message_string = centre_p->choose_tcp_network(2);
  if(message_string=="")//success
  {
    connection_message_p->setText("Listening for connection to address 192.168.200.1");
    return;
  }
  connection_message_p->setText(QString("listen failure.   ") . append(message_string));
}    

void slave_mode_screen::function_as_client_clicked()
{
  centre_p->add_waiting_screen(60);//come back here when done
  centre_p->add_waiting_screen(42);//tcp_client_server_addr_entry
  centre_p->screen_done = true;
}

void slave_mode_screen::help_button_clicked()
{
  help_screen_p = new help_screen;
  
  help_screen_p -> set_text("This screen allows you to set up communications with a computer, PLC, etc.  "
                            "A message can be sent after each sample, containing things like the count, a "
                            "barcode, or text.  You set up what will be sent in another screen.  Here, you choose the method "
                            "of connection.\n\n"
                            
                            "The other communication options all involve communicating by TCP (transmission control protocol) "
                            "using a connection by ethernet cable.  This requires a program on the other computer "
                            "capable of communicationg using this method.  The three options all result in the same thing - "
                            "a TCP link to the other computer.  The options merely control the method by which the link "
                            "is established.\n\n"
                            
                            "FUNCTION AS TCP SERVER USING ... - With these two options, the ESC-3 will 'listen' for a connection "
                            "from the other computer, which should connect to the address chosen.\n\n"
                            
                            "FUNCTION AS TCP CLIENT - With this option, the other device must function as a server, and 'listen' "
                            "for a connection from the ESC-3.  You will enter the IP address it will connect to later. \n\n"
                            
                            "With all three TCP methods, the result is the same - a TCP connection to the other computer.\n\n");
  help_screen_p -> show();
}

void slave_mode_screen::connection_detected()
{
//  cout<<"connection_detected\n";

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
    cout<<"command_line_string\n";
    for(int i=0; i<command_line_string.size(); ++i)
    {
      cout<<"  command_line_string["<<i<<"] = "<<int(command_line_string[i].toLatin1())<<"  prints "<<command_line_string[i].toLatin1()<<endl;
    }
    QStringList list = command_line_string.split(QChar(31));
    cout<<"command received\n";
    for(int i=0; i<list.size(); ++i)
    {
      cout<<"  list["<<i<<"] = "<<list[i].toStdString()<<endl;
    }
    new_command_p = new slave_mode_command;
    bool ok=false;
    if(list.size()<2)
    {
      cout<<"list size <2\n";
      return;
    }
    QString type_flag_string = list[0];
    if(type_flag_string.size() != 1)
    {
      cout<<"type_flag_string format error\n";
      return;
    }
    new_command_p->type_flag = type_flag_string[0];
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
      batch_mode_driver_p -> dump_speed = 0;
      batch_mode_driver_p -> current_set = 0;
      batch_mode_driver_p -> current_pack = 0;
      batch_mode_driver_p -> current_count_limit = 0;
      batch_mode_driver_p -> current_pack_limit = 0;
      batch_mode_driver_p->mode = slave_mode_entry;
      batch_mode_driver_p->slave_mode = false;
  
      batch_mode_driver_p->stop();
      
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
      
    command_p_list.enqueue(new_command_p);
    display_command(new_command_p);
    return;
  }
  //if gets to here, character is neither 2 nor 3
  command_line_string.append(character);
}

void slave_mode_screen::command_finished()
{
  cout<<"slave_mode_screen::command_finished()\n";
  command_finished_bool = true;
}

void slave_mode_screen::run()
{
  if(batch_mode_driver_p->mode == wait_for_pack)
  {
    if(end_gate_filling == true)
    {
      end_gate_filling = false;
      end_gate_full = true;
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
        batch_mode_driver_p->pack_complete = true;
      }
    }
    else if(end_gate_closed_empty == true)
    {
      end_gate_closed_empty = false;
      end_gate_filling = true;
    }
  }
  
  QByteArray array;
  array.append(QChar(2));
  array.append('d');
  array.append(QChar(31));
  array.append(QString::number(centre_p->count));
  array.append(QChar(31));
  if(batch_mode)
  {
    array.append(QString::number(batch_mode_driver_p->current_pack));
  }
  else
  {
    array.append('0');
  }
  array.append(QChar(31));
  array.append(QChar(3));
  centre_p->tcp_socket_p->write(array);
  
//  if(batch_mode)
//  {
    
  
  if(executing_command_p == 0)
  {
    if(command_p_list.size() == 0) return;
    executing_command_p = command_p_list.dequeue();
    command_finished_bool = false;
  }
  if(executing_command_p->type_flag == 'C')
  {
    if(executing_command_p->command == "Start")
    {
      centre_p->set_speed(speed);
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
        batch_mode_driver_p->start();
        batch_mode_driver_p->slave_mode = true;
        batch_mode_driver_p->dump_speed = 1000;
        batch_mode_driver_p->restart();
      }
      if(command_finished_bool == true)
      {
        end_command();
        batch_mode_driver_p->stop();
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
      centre_p->totalize_force_endgate_open = true;
      end_command();
      return;
    }
    if(executing_command_p->command == "CloseEnd")
    {
      centre_p->totalize_force_endgate_open = false;
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
    end_command();
    return;
  }
  if(executing_command_p->type_flag == 'P')
  {
    if(command_finished_bool == true)
    {
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
      centre_p->set_speed(executing_command_p->speed);
      batch_mode_driver_p->high_feed_speed = executing_command_p->speed;
      batch_mode_driver_p->low_feed_speed = (executing_command_p->speed) / 10;
      if(executing_command_p->number_of_sets > 0)//batch mode
      {
        batch_mode_driver_p->clear_program();
        batch_mode_driver_p->use_spreadsheet = false;
        for(int i=0; i<executing_command_p->number_of_sets; ++i)
        {
          int packs = executing_command_p->number_of_packs[i];
          int seeds = executing_command_p->seeds_per_pack[i];
          batch_mode_driver_p->add_line(packs, seeds);
        }
        batch_mode_driver_p->require_seed_lot_barcode = false;
        batch_mode_driver_p->require_pack_barcode = false;
        batch_mode_driver_p->pack_match_lot = false;
        batch_mode_driver_p->pack_contain_lot = false;
        batch_mode_driver_p->lot_contain_pack = false;
        batch_mode_driver_p->pack_match_spreadsheet = false;
        batch_mode_driver_p->record_only = true;
        batch_mode_driver_p->mode = slave_mode_entry;
        batch_mode_driver_p->slave_mode = true;
    
        batch_mode_driver_p->start();
      }
    }
    return;
  }
}

void slave_mode_screen::end_command()
{
  cout<<"start slave_mode_screen::end_command()\n";
  if(previous_command_p != 0)
  {
    delete previous_command_p;
  }
  previous_command_p = executing_command_p;
  executing_command_p = 0;
  batch_mode_driver_p->slave_mode = false;
  cout<<"end slave_mode_screen::end_command()\n";
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
