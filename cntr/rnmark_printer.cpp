#include "rnmark_printer.hpp"
#include <iostream>
#include <QDebug>
#include <QFile>
#include <QPixmap>
#include <fcntl.h>
#include <unistd.h>

//RN MARK test Commands and Feedback//

using namespace std;

rnmark_printer::rnmark_printer()
{  
  tcpSocket = new QTcpSocket;  
  in.setDevice(tcpSocket);
  out.setDevice(tcpSocket);
    
  connect_printer();
    
  initial_setup_timer_p = new QTimer;  
  connect(initial_setup_timer_p, SIGNAL(timeout()), this, SLOT(clear_ram()));
  connect(initial_setup_timer_p, SIGNAL(timeout()), this, SLOT(set_print_speed()));
  connect(initial_setup_timer_p, SIGNAL(timeout()), this, SLOT(set_print_delay()));
  connect(initial_setup_timer_p, SIGNAL(timeout()), this, SLOT(print_on()));
  connect(initial_setup_timer_p, SIGNAL(timeout()), this, SLOT(print_once_mode()));
  
  initial_setup_timer_p->setSingleShot(true);
  initial_setup_timer_p->start(800);
}

rnmark_printer::~rnmark_printer()
{
  print_off();
  
  if((tcpSocket->state()) == 3)
  {
    cout<<endl<<"Disconnecting from Host"<<endl;
    tcpSocket->disconnectFromHost();
  }
  cout<<"SocketState: "<<tcpSocket->state()<<endl;
  
  tcpSocket->abort();
}

bool rnmark_printer::check_socket_connection()
{
  if(tcpSocket->state() == 3) return true;
  else  return false;
}

bool rnmark_printer::check_usb_connection()
{
  int filedesc = open(usbserialconnection, O_WRONLY);
  if(filedesc < 0)  return false;
  else return true;
}

void rnmark_printer::connect_printer()
{
  if(((tcpSocket->state()) != 3) || ((tcpSocket->state()) != 2))
  {
    cout<<endl<<"Connecting to Host"<<endl;
    tcpSocket->connectToHost("192.168.0.90", 17170);
    if (tcpSocket->waitForConnected(800))
    {
      qDebug("Connected!");
    }
  }
}

void rnmark_printer::send_packets_to_printer()
{
  if((tcpSocket->state()) == 3)
  {
    tcpSocket->write(structured_packet_array, structured_packet_array.size());
  }
  else
  {
    qDebug()<<endl<<"Printer Unavailable (check socket connection)"<<endl;
  }
}

void rnmark_printer::read_packets_from_printer()
{
  structured_packet_array_in.clear();
  if((tcpSocket->bytesAvailable())>0)
  {
    qDebug()<<endl<<"Feedback Message Available"<<endl;
    structured_packet_array_in.append(tcpSocket->read((tcpSocket->bytesAvailable())));
    
    for(int q=0; q<structured_packet_array_in.size(); ++q)
    {
      qDebug()<<(QString("%1").arg((unsigned char)structured_packet_array_in[q] , 0, 10));
    }
  }
  //else cout<<"-no FDB-";  //OMIT~~~//
}

void rnmark_printer::check_output_bytearray()
{  
  for(int q=0; q<packet_size; ++q)
  {
    qDebug()<<(QString("%1").arg((unsigned char)structured_packet_array[q] , 0, 10));
  }
}

void rnmark_printer::send_barcode_serial_transmission(QString serial_str)
{
  cout<<endl<<"barcode_serial_transmission()"<<endl;
  
  int system_int;
  system_int = system("stty -F /dev/TTL232RG cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
  serial_str = serial_str + "\n";

  int filedesc = open(usbserialconnection, O_WRONLY);
  cout<<endl<<"serial string buffer length: "<<write(filedesc,(serial_str.toUtf8().constData()), serial_str.size())<<","<<system_int<<endl;
  
  qDebug()<<serial_str;
}

void rnmark_printer::clear_ram()
{  
  cout<<endl<<"52 (Clear message from RAM)"<<endl;
  declaration_id = 52;
  packet_size = 3;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 52;
  structured_packet_array[2] = end_of_transmission;
  
  send_packets_to_printer();
}

void rnmark_printer::print_on()
{  
  cout<<endl<<"53 (Print ON/OFF)"<<endl;
  declaration_id = 53;
  packet_size = 4;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 53;
  structured_packet_array[2] = 1;
  structured_packet_array[packet_size-1] = end_of_transmission;
  
  send_packets_to_printer();
}

void rnmark_printer::print_off()
{  
  cout<<endl<<"53 (Print ON/OFF)"<<endl;
  declaration_id = 53;
  packet_size = 4;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 53;
  structured_packet_array[2] = 0;
  structured_packet_array[packet_size-1] = end_of_transmission;
  
  send_packets_to_printer();
}

void rnmark_printer::print_once_mode()
{  
  cout<<endl<<"55 (Operation Mode)"<<endl;
  declaration_id = 55;
  packet_size = 4;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 55;
  structured_packet_array[2] = 1;
  structured_packet_array[3] = end_of_transmission;
  
  send_packets_to_printer();
}

void rnmark_printer::resume_print()
{  
  cout<<endl<<"56 (Resume next print)"<<endl;
  declaration_id = 56;
  packet_size = 3;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 56;
  structured_packet_array[2] = end_of_transmission;
  
  send_packets_to_printer();
}

void rnmark_printer::set_print_speed()
{  
  cout<<endl<<"60 (Print Speed) - "<<print_speed<<endl;
  declaration_id = 60;
  packet_size = 5;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 60;
  structured_packet_array[2] = print_speed/256;
  structured_packet_array[3] = print_speed-(print_speed/256)*print_speed;
  structured_packet_array[4] = end_of_transmission;
  
  send_packets_to_printer();  
}

void rnmark_printer::set_print_delay()
{  
  cout<<endl<<"61 (Print Delay) - "<<delay_distance<<endl;
  declaration_id = 61;
  packet_size = 5;

  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 61;
  structured_packet_array[2] = delay_distance/256;
  structured_packet_array[3] = delay_distance-(delay_distance/256)*delay_distance;
  structured_packet_array[4] = end_of_transmission;
  
  send_packets_to_printer();  
}

void rnmark_printer::set_printing_area_width()
{  
  cout<<endl<<"62 (Printing are Width) - ("<<bitmap_width<<","<<bitmap_height<<")"<<endl;
  declaration_id = 62;
  packet_size = 5;
  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 62;
  structured_packet_array[2] = bitmap_width/256;
  structured_packet_array[3] = bitmap_width - (bitmap_width/256)*256;
  structured_packet_array[4] = end_of_transmission;
  
  send_packets_to_printer();
}

void rnmark_printer::generate_bmp_bytearray()
{
  packet_size = 9 + (bitmap_width*bitmap_height)/8;  
  structured_packet_array.clear();
  structured_packet_array.resize(packet_size);
  
  structured_packet_array[0] = start_of_heading;
  structured_packet_array[1] = 63;
  structured_packet_array[2] = 0;
  structured_packet_array[3] = 0;
  structured_packet_array[4] = 0;
  structured_packet_array[5] = bitmap_width/256;
  structured_packet_array[6] = bitmap_width - (bitmap_width/256)*256;
  structured_packet_array[7] = bitmap_height/8;
  structured_packet_array[packet_size-1] = end_of_transmission;
  
  QFile bmp_file("rnmark_image");  //populate bytearray with BMP data//
  if (!bmp_file.open(QFile::ReadOnly)) return;
  QByteArray test_bitmap_bytearray = bmp_file.readAll();
  
  qDebug()<<"test_bitmap_bytearray: "<<test_bitmap_bytearray.size();
  for(int q=0; q<((bitmap_height*bitmap_width)/8); ++q)
  {
    structured_packet_array[q+8] = test_bitmap_bytearray[(bitmap_height*bitmap_width/8)-q];
  }

  send_packets_to_printer();  
  cout<<endl<<"63 (Bitmap Data) - "<<test_bitmap_bytearray.size()<<endl;
}

void rnmark_printer::retrieve_bmp_dimensions()
{
  QPixmap bitmap_pixmap;
  bitmap_pixmap.load("rnmark_image", "BMP");
  
  bitmap_height = bitmap_pixmap.width();  //inverted width+height for RN Mark bitmap orientation//
  bitmap_width = bitmap_pixmap.height();
    
  qDebug()<<"width: "<<bitmap_pixmap.width()<<"\theight: "<<bitmap_pixmap.height();
}



