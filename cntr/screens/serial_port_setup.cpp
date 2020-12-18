#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
//#include <fcntl.h>	//library used to use system call command "open()" used to check available serial
//#include <unistd.h>	//library to enable write() function
//#include <termios.h>

#include "centre.hpp"
#include "serial_port_setup.hpp"
#include "button.hpp"
#include "help_screen.hpp"

using namespace std;

serial_port_setup::serial_port_setup(centre*set_centre_p)
:screen(set_centre_p)
{
  back_button_p = new button("Back");
  b9600_p = new QRadioButton("9600");
  b19200_p = new QRadioButton("19200");
  b38400_p = new QRadioButton("38400");
  b57600_p = new QRadioButton("57600");
  b115200_p = new QRadioButton("115200");
  done_button_p = new button("Done");

  QLabel* message_p = new QLabel("Select transmission speed.\nThis must match the speed of the attached device");
  baud_box_p = new QGroupBox;
  baud_layout_p = new QVBoxLayout;
  main_layout_p = new QGridLayout;
  
  baud_layout_p -> addWidget(b9600_p);
  baud_layout_p -> addWidget(b19200_p);
  baud_layout_p -> addWidget(b38400_p);
  baud_layout_p -> addWidget(b57600_p);
  baud_layout_p -> addWidget(b115200_p);
  baud_box_p -> setLayout(baud_layout_p);
  
  main_layout_p -> addWidget(back_button_p, 0, 1);
  main_layout_p -> addWidget(message_p, 1, 0, 1, 2);
  main_layout_p -> addWidget(baud_box_p, 2, 0, 1, 2);
  main_layout_p -> addWidget(done_button_p, 3, 1);
  setLayout(main_layout_p);
  
  if(centre_p->baud_rate == 9600) b9600_p -> setChecked(true);
  if(centre_p->baud_rate == 19200) b19200_p -> setChecked(true);
  if(centre_p->baud_rate == 38400) b38400_p -> setChecked(true);
  if(centre_p->baud_rate == 57600) b57600_p -> setChecked(true);
  if(centre_p->baud_rate == 115200) b115200_p -> setChecked(true);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
}
  
serial_port_setup::~serial_port_setup()
{
}
  
void serial_port_setup::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void serial_port_setup::done_button_clicked()
{
  /*
  centre_p->communicate_by_keyboard_cable = false;
  centre_p->communicate_by_tcp = false;
  centre_p->communicate_by_serial_port = true;

  int fd = centre_p->serial_port_fd;
  struct termios tio;
  memset(&tio,0,sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag=0;
  tio.c_cflag=CS8|CREAD|CLOCAL;
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=5;
  
  if(fd<0)fd = open("/dev/ttyACM0", O_RDWR | O_NONBLOCK);
  if(fd<0) cout<<"serial port opening error\n";
  */
  if(b9600_p->isChecked())
  {
    cout<<"set 9600 baud\n";
    centre_p->setup_serial_communications(9600);
//    cfsetospeed(&tio,B9600);          
//    cfsetispeed(&tio,B9600);          
  }
  if(b19200_p->isChecked())
  {
    centre_p->setup_serial_communications(19200);
  }
  if(b38400_p->isChecked())
  {
    centre_p->setup_serial_communications(38400);
  }
  if(b57600_p->isChecked())
  {
    centre_p->setup_serial_communications(57600);
  }
  if(b115200_p->isChecked())
  {
    centre_p->setup_serial_communications(115200);
  }
  centre_p->screen_done = true;
}
