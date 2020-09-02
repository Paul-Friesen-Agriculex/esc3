#include <QApplication>
#include "centre.hpp"
#include <iostream>

using namespace std;

centre* centre_p;

int main(int argc, char ** argv)
{
  QApplication a(argc, argv);
  centre_p = new centre;
  centre_p->application_p = &a;
  int ret_val = a.exec();
  cout<<"ret_val = "<<ret_val<<endl;
  return ret_val;
}
