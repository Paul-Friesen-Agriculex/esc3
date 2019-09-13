#ifndef rnmark_declarations_cpp
#define rnmark_declarations_hpp

#include "centre.hpp"
#include <QString>
#include <QByteArray>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>

#define print_speed                   20
#define delay_distance                10
#define usbserialconnection           "/dev/TTL232RG"   //Write UDEV rule for static name//

class batch_mode_driver;

class rnmark_printer : public QObject
{
  Q_OBJECT
  
  public:
  rnmark_printer();
  ~rnmark_printer();
  
  public slots:
  void retrieve_bmp_dimensions();
  void generate_bmp_bytearray();   
  void send_packets_to_printer();
  void send_barcode_serial_transmission(QString serial_str);
  
  //private slots:
  void connect_printer();
  void clear_ram();
  void print_on();
  void print_off();
  void print_once_mode();
  void resume_print();
  void set_print_speed();
  void set_print_delay();
  void set_printing_area_width();
  
  void read_packets_from_printer();
  void check_output_bytearray();
  bool check_socket_connection();
  bool check_usb_connection();
  
  private:
  static const int start_of_heading = 1;
  static const int end_of_transmission = 4;
  
  QTcpSocket *tcpSocket;
  QDataStream in;
  QDataStream out;
  QTimer *initial_setup_timer_p;
  
  QImage* test_bitmap;
  int bitmap_width;
  int bitmap_height;
  int packet_size;
  int declaration_id;
  
  QByteArray structured_packet_array;
  QByteArray structured_packet_array_in;
};
#endif
