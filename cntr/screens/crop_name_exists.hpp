#ifndef crop_name_exists_hpp
#define crop_name_exists_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QVBoxLayout;
class QLabel;

class crop_name_exists : public screen
{
  Q_OBJECT
  
  public:
  crop_name_exists(centre*set_centre_p);
  
  private slots:
	  void ok_button_clicked();
    void start_screen_clicked();
  
  private:
	  QLabel* message_p;
	  button* ok_button_p;
    button* start_screen_p;
	  QVBoxLayout* layout_p;
};
#endif
