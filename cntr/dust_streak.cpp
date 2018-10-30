#include "dust_streak.hpp"
#include <iostream>

using namespace std;

dust_streak::dust_streak()
{
  length = 0;
  confirmed = false;
  started = false;
}
  
void dust_streak::add_line(int pixel_value)
{
  if(pixel_value<detect_value)
  {
    if(started==false)
    {
      started = true;
      length = 0;
    }
    ++length;
    if(length>detect_length && confirmed==false) 
    {
      confirmed = true;
    }
  }
  else
  {
    started = false;
    confirmed = false;
    length = 0;
  }
}


