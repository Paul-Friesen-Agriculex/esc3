#ifndef spreadsheet_choice_hpp
#define spreadsheet_choice_hpp

#include "centre.hpp"
class QTimer;

class spreadsheet_choice : public screen
{
  Q_OBJECT
  
  private:
  QTimer* timer_p;
  
  private slots:
  void run();
  
  public:
  spreadsheet_choice(centre*set_centre_p);
  ~spreadsheet_choice();
};
#endif
