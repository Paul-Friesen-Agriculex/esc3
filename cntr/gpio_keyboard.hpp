#ifndef GPIO_KEYBOARD_HPP
#define GPIO_KEYBOARD_HPP

#include "centre.hpp"		//TEST to retrieve seed count
#include "table.hpp"		//TEST to retrieve barcode		//OMIT~~~
//#include "totalize.hpp"	//TEST~~~~~~
#include <QObject>
#include <QString>			//for string to ascii

class QTimer;

enum BIT_STATE{BIT_HIGH, BIT_LOW};
enum CLK_STATE{CLK_ON, CLK_OFF};

class gpio_keyboard:public QObject
{
  Q_OBJECT
  
  private:  
  QTimer* initialization_pulse_on_p;
  QTimer* initialization_pulse_off_p;
  QTimer* signal_pulse_p;
  QTimer* clock_pulse_p;
    
  centre* centre_p;	//TEST~~~
  //table* table_p;	//TEST~~~
  //totalize* totalize_p;	//TEST~~~
  
  BIT_STATE bit_value;
  CLK_STATE clk_value;
  static const long initialization_duration = 15;	//delay between two characters
  static const long signal_duration = 8;			//delay between two bits
  static const int binary_length = 8;
  //int binary_array[8] = {0,0,0,0,0,0,0,0};
  int binary_array[8];
  QList<int> ascii_int_list;
  QString count_string;	//TEST~~~
  
  public:
  gpio_keyboard();
  ~gpio_keyboard();	
  
  public slots:
  void gpio_send(QString string_input);
  
  private slots:
  void clock_on();							//gpio control corresponding to clock and data signals
  void clock_off();
  void bit_high();
  void bit_low();
  void dec2bit(int decimal_input);			//converting totalized seed count to bit value stored in int array
  void init_send();							//works similar to parity bits for asynchronous signaling
  void toggle_clock();
  void set_bit(int bitvalue);
  void string2ascii(QString string_input);	//converts string to hex
  void bittransfer();						//coordinates clock and output pins for bit transmission
  int macro_characters(int macro_int);		//handles macro strings leading with backslash "\"
  	
};

#endif
