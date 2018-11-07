#include "centre.hpp"		//TEST to retrieve seed count
#include "gpio_keyboard.hpp"
#include <QObject>
#include <QString>			//for string to ascii
#include <QTimer>
#include <stdio.h>
#include <iostream>			//OMIT~~~ for cout
#include <QByteArray>		//test for toLatin1
#include <QList>			//storage option for array of integers
#include "totalize.hpp"

using namespace std;	//OMIT~~~ for output dialogue

gpio_keyboard::gpio_keyboard()
{
  initialization_pulse_on_p = new QTimer;
  initialization_pulse_off_p = new QTimer;
  signal_pulse_p = new QTimer;
  initialization_pulse_on_p -> setSingleShot(true);
  initialization_pulse_off_p -> setSingleShot(true);
  signal_pulse_p -> setSingleShot(true);
  connect(initialization_pulse_on_p, SIGNAL(timeout()), this, SLOT(bit_high()));
  connect(initialization_pulse_off_p, SIGNAL(timeout()), this, SLOT(bit_low()));
  connect(signal_pulse_p, SIGNAL(timeout()), this, SLOT(bittransfer()));

//=========TEST and OMIT~~~=========//
  clock_pulse_p = new QTimer;
  clock_pulse_p -> setSingleShot(true);
  connect(clock_pulse_p, SIGNAL(timeout()), this, SLOT(toggle_clock()));
//==================================//

  bit_low();
  bit_value = BIT_LOW;
  clock_on();
  clk_value = CLK_ON;
  
  for(int i=0; i<8; ++i)
  {
    binary_array[i] = 0;
  }
}

gpio_keyboard::~gpio_keyboard()
{
  initialization_pulse_on_p -> stop();
  initialization_pulse_off_p -> stop();
  signal_pulse_p -> stop();
  clock_pulse_p -> stop();
  delete initialization_pulse_on_p;
  delete initialization_pulse_off_p;
  delete signal_pulse_p;
  delete clock_pulse_p;
}

void gpio_keyboard::gpio_send(QString string_input)
{  
  ascii_int_list.prepend(0);	//variables for number of characters sent + bits sent
  ascii_int_list.prepend(0);	//
  
  string2ascii(string_input); 	//creates Qbytearray
 
  init_send();
  signal_pulse_p -> start(2*initialization_duration);
}
//=============================================================================================================================//
void gpio_keyboard::string2ascii(QString string_input)
{
  //TEST variables
  //string_input = "HELLO WORLD";	//TEST~~~
  //string_input = "ªªª";			//TEST~~~	//01010101
  
  QByteArray byte_array;
  byte_array = (string_input).toLatin1();
  
  int array_length;
  array_length = byte_array.size();
  int array_int[array_length]; 				//ORIGINAL
  
  cout<<"Array_int_len: "<<array_length<<endl;	//OMIT~~~
  for(int qwe=0; qwe<array_length; ++qwe){		//
    cout<<" "<<byte_array.at(qwe);				//
  }												//

  for(int h=0; h<array_length; ++h)
  {
    if(byte_array[h] == 92)	//leading backslash '\' for macro commands
    {
	  byte_array.remove(h,1);
	  byte_array[h] = macro_characters(byte_array[h]);
	  --array_length;
    }
  }

  cout<<endl<<string_input.toStdString()<<endl;	//OMIT~~~
  ascii_int_list.append(array_length);
  
  for(int g=0; g<array_length; ++g)
  {
	array_int[g] = byte_array.at(g);			//implicit conversion of bytes to int
	 
    ascii_int_list.append(byte_array.at(g));
	cout<<array_int[g]<<"\t";					//OMIT~~~
	dec2bit(array_int[g]);
  }
}

void gpio_keyboard::dec2bit(int decimal_input)
{
  memset(binary_array, 0, sizeof(binary_array));	//reset contents to 0
  //binary_array[8];
  int i = 0;

  while(decimal_input != 0)
  {
    binary_array[i] = ((decimal_input % 2 == 0) ? 0:1) ;
    ++i;
    //decimal_input = decimal_input/=2; //Original//
    decimal_input = decimal_input/2; //Test//
  }
  
  for(int j=0; j<8; ++j)
  {
	cout<<binary_array[j];						//OMIT~~~
	ascii_int_list.append(binary_array[j]);
  }
  cout<<endl;
}
//=============================================================================================================================//
void gpio_keyboard::bittransfer()
{
  int char_buffer;	//number of characters in send buffer
  int char_num;		//number of chars sent
  int bit_num;		//number of bits sent
  int list_position;
  
  char_num = ascii_int_list.at(0);
  bit_num = ascii_int_list.at(1);
  char_buffer = ascii_int_list.at(2);
  list_position = 4 + char_num*9 + bit_num;
  
  if(bit_num == 0){
	cout<<endl<<ascii_int_list.at(list_position-1)<<endl;	//OMIT~~~
  }
  
  cout<<ascii_int_list.at(list_position);		//OMIT~~~
  //cout<<ascii_int_list.at(list_position);		//OMIT~~~
  //cout<<" "<<list_position;					//OMIT~~~
  set_bit(ascii_int_list.at(list_position));
  toggle_clock();
  
  if(bit_num < 7)
  {
    ++bit_num;
    ascii_int_list.replace(1, bit_num);
  }
  else
  {
	bit_num = 0;
	++char_num;
	ascii_int_list.replace(0, char_num);
	ascii_int_list.replace(1, bit_num);
  }
  
  if((char_num < char_buffer) && (bit_num != 0))
  {
	signal_pulse_p -> start(signal_duration);
  }  
  else if((char_num < char_buffer) && (bit_num == 0))		//last bit of current char
  {
	cout<<endl;		//OMIT~~~
	init_send();
	signal_pulse_p -> start(signal_duration + 2*initialization_duration);
  }
  
  if(char_num >= char_buffer)								//last bit of last char
  {
	cout<<endl<<endl;			//OMIT~~~
	clock_pulse_p -> start();	//OMIT~~~
	clock_pulse_p -> start();	//OMIT~~~
  }
}
//=============================================================================================================================//
void gpio_keyboard::init_send()		//duration currently set to 1500ms
{
  //cout<<endl<<"init_low";		//OMIT~~~
  if(clk_value == CLK_ON)
  {
	clock_pulse_p -> start(signal_duration);	//OMIT~~~
  }
  else
  {
	clock_on();
	clock_pulse_p -> start(signal_duration);	//OMIT~~~
  }
  initialization_pulse_off_p -> start(signal_duration);
  //clock_pulse_p -> start(signal_duration);		//OMIT~~~
  initialization_pulse_on_p -> start(signal_duration + initialization_duration);
  clock_pulse_p -> start(signal_duration + initialization_duration);
}
//=============================================================================================================================//
void gpio_keyboard::set_bit(int bitvalue)
{
  if(bitvalue == 1)
  {
	bit_high();
  }  
  else
  {
	bit_low();
  } 
}

void gpio_keyboard::bit_high()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio19/value", "w");
  fprintf(fp, "1");  //gpio circuit, logic-low yields logic-high on circuit output
  fclose(fp);
  bit_value = BIT_HIGH;
  //cout<<" (H) "<<endl;	//OMIT~~~
}

void gpio_keyboard::bit_low()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio19/value", "w");
  fprintf(fp, "0");  //gpio circuit, logic-high yields logic low-on circuit output
  fclose(fp);
  bit_value = BIT_LOW;
  //cout<<" (L) "<<endl;	//OMIT~~~
}
//-----------------------------------------------------------------------------------------------------------------------------//
void gpio_keyboard::toggle_clock()
{
   if(clk_value == CLK_OFF){
	 clock_on();
   }
   else
   {
     clock_off();
   }
}

void gpio_keyboard::clock_on()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio28/value", "w");
  fprintf(fp, "1");  
  fclose(fp);
  clk_value = CLK_ON;
}

void gpio_keyboard::clock_off()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio28/value", "w");
  fprintf(fp, "0");  
  fclose(fp);
  clk_value = CLK_OFF;
}
//=============================================================================================================================//
int gpio_keyboard::macro_characters(int macro_int)		//remap ascii keycodes to adafruit trinket 1500 usb library keycodes
{
	//do not use centre, generates additional screen each time macro_characters function is called//
	//retrieve seed count from another source -> directly from totalize or batch mode counts
	//figure out which variable corresponds to barcode
  //centre_p = new centre;
  //count_string = QString::number(centre_p->count);
  
  //table_p = new table;	//TEST~~~
  //table_p->old_count;	//TEST~~~
  
  //totalize* totalize_p;	
  //totalize_p = new totalize;
  //count_string = QString::number(totalize_p->old_count);
  //count_string = QString::number(old_count);
  //totalize_p->old_count;
  
  switch(macro_int) {
    case 110 : 
		macro_int = 10;	//corresponding to 'n'
		break;
    case 116 : 
		macro_int = 9;	//corresponding to 't'
		break;
    case 118 : 
		macro_int = 11;	//corresponding to 'v'
		break;
    case 114 : 
		macro_int = 13;	//corresponding to 'r'
		break;
    case 115 : 
		macro_int = 32;	//corresponding to 's'
		break;
    case 85 : 
		macro_int = 38;	//corresponding to 'U'
		break;
    case 68 : 
		macro_int = 40;	//corresponding to 'D'
		break;
    case 76 : 
		macro_int = 37;	//corresponding to 'L'
		break;
    case 82 : 
		macro_int = 39;	//corresponding to 'R'
		break;
    
    /*case 67 : 	//corresponds to 'C' for count
		//macro_int = 110;	//corresponding to count value		//attempt to generate count value from centre files
		QString countstring = QString::number();
		gpio_keyboard_p = new gpio_keyboard;
		//macro_function_string.append("\\X");					//TEST~~~ figure out hex value for CC
		macro_int = 0;
		break;*/
    
    /*case 67 : 	//corresponds to 'C' for count
		//macro_int = 110;	//corresponding to count value		//attempt to generate count value from centre files
		QString countstring = QString::number();
		gpio_keyboard_p = new gpio_keyboard;
		//macro_function_string.append("\\X");					//TEST~~~ figure out hex value for CC
		macro_int = 0;
		break;*/
		
	/*case 83:	//corresponds to 'S' for barcode1
		macro_int = 0;
		break;*/
  }
    //cout<<table_p->old_count;	//tEST~~~
    //table_p = new table;	//TEST~~~
    //~table_p;	//TEST~~~
  
  return macro_int;
}






