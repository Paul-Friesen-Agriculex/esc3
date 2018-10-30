#include <stdio.h>
#include <iostream>

#include "envelope_sensor.hpp"

using namespace std;

envelope_sensor::envelope_sensor()
{
}

bool envelope_sensor :: read() //returns true if envelope is present
{
  FILE * fp;
  int i;
  fp = fopen("/sys/class/gpio/gpio174/value", "r");
  if(!fscanf(fp,"%d",&i)) cout<<"fscanf failure\n";
  fclose(fp);
  if (i==1)
  {
    return(false);
  }
  else
  {
    return(true);
  }
}


