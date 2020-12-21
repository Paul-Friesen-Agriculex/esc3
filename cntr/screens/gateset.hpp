#ifndef gateset_hpp
#define gateset_hpp

#include "centre.hpp"

class button;
class QSpinBox;
class QGridLayout;
class QLabel;

class gateset : public screen
{
  Q_OBJECT
  
  public:
  gateset(centre*set_centre_p);
  
  private slots:
	  void ok_button_clicked();
//    void enter_value(int value);
  
  private:
	  QLabel* message_p;
    QSpinBox* spinbox_p;
	  button* ok_button_p;
	  QGridLayout* layout_p;
};
#endif
