#include <QApplication>
#include "centre.hpp"

//#include <QTimer>

int main(int argc, char ** argv)
{
  /*
  QTimer start_delay;
  start_delay.start();
  while(1)
  {
    if(start_delay.elapsed()>1000) break;
  }
  */
  QApplication a(argc, argv);
  centre c;
  return a.exec();
}
