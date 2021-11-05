#ifndef ss_other_options_hpp
#define ss_other_options_hpp

#include "centre.hpp"
#include <QPushButton>  //TEST~~~ replace all buttons with QPushButtons widget buttons//

class button;
class QGridLayout;
class QGroupBox;
class help_screen;


class ss_other_options : public screen
{
  Q_OBJECT
  
  public:
  ss_other_options(centre*set_centre_p);
  
  public slots:
  void back_button_clicked();
  void print_button_clicked();
  void display_columns_button_clicked();
  void done_button_clicked();
  void help_button_clicked();
  
  private:
  button* back_button_p;
  button* print_button_p;
  button* display_columns_button_p;
  button* done_button_p;
  button* help_button_p;
  QGridLayout* main_layout_p;
  
  help_screen* help_screen_p;
};
#endif
