
#include <stdio.h>
#include <iostream>
#include <QTimer>
#include "brother_envelope_feeder.hpp"

#include <stdlib.h>		//OMIT~~~ system time output//

using namespace std;

brother_envelope_feeder::brother_envelope_feeder()
{  
  delay_envelope_taken = new QTimer;
  delay_sensor_triggered = new QTimer;
  
  delay_envelope_taken->setSingleShot(true);
  delay_sensor_triggered->setSingleShot(true);
  
  connect(delay_envelope_taken, SIGNAL(timeout()), this, SLOT(run()));
  connect(delay_sensor_triggered, SIGNAL(timeout()), this, SLOT(run()));
}

brother_envelope_feeder::~brother_envelope_feeder()
{
  motor_off();  
}

void brother_envelope_feeder::run()
{
  envelope_feeder_sensor_state();
  
  if((delay_envelope_taken->isActive()) || (delay_sensor_triggered->isActive()))
  {
	if((delay_envelope_taken->isActive()) && (envelope_feeder_sensor_triggered))	//updates delays until sensor triggers//
	{
	  delay_envelope_taken->start(2000);
	}
	else if((delay_sensor_triggered->isActive()) && !envelope_feeder_sensor_triggered)
	{
	  delay_sensor_triggered->start(1200);	//original 1400ms//
	}
  }
  
  else if(envelope_feeder_sensor_triggered)
  {
    motor_off();
    delay_envelope_taken->start(2000);	//delay to create gap between envelopes taken by printer
//    cout<<endl<<"envelope_taken"<<endl;			//OMIT~~~//
//    int system_int; 							//OMIT~~~//
//    system_int = system("date \"+%M:%S:%N\"");	//OMIT~~~//
  }
  else
  {
    motor_on();
    delay_sensor_triggered->start(1200); //original 1400 ms//	//delay to allow envelope to be pushed further into printer
//    cout<<endl<<"push envelope further"<<endl;	//OMIT~~~//
//    int system_int; 							//OMIT~~~//
//    system_int = system("date \"+%M:%S:%N\"");	//OMIT~~~//
  }
}

void brother_envelope_feeder::envelope_feeder_motor_state()
{
  FILE * fp;
  int i;
  fp = fopen("/sys/class/gpio/gpio173/value", "r");
  if(!fscanf(fp,"%d",&i)) cout<<"fscanf failure\n";
  fclose(fp);
  if (i==1)
  {
    envelope_feeder_motor_triggered = true;
  }
  else
  {
    envelope_feeder_motor_triggered = false;
  }
}

void brother_envelope_feeder::envelope_feeder_sensor_state()
{
  FILE * fp;
  int i;
  fp = fopen("/sys/class/gpio/gpio192/value", "r");
  if(!fscanf(fp,"%d",&i)) cout<<"fscanf failure\n";
  fclose(fp);
  if (i==0)
  {
    envelope_feeder_sensor_triggered = true;
  }
  else
  {
    envelope_feeder_sensor_triggered = false;
  }
}
 
void brother_envelope_feeder::motor_on()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "1");
  fclose(fp);
}

void brother_envelope_feeder::motor_off()
{
  FILE * fp;
  fp = fopen("/sys/class/gpio/gpio173/value", "w");
  fprintf(fp, "0");
  fclose(fp);
}








