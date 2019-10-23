#ifndef tool_choice_hpp
#define tool_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;

class tool_choice : public screen
{
  Q_OBJECT
  
  public:
  tool_choice(centre*set_centre_p);
  
  private slots:
  void crop_edit_clicked();
  void diagnostics_clicked();
  void stop_program_clicked();
  void back_button_clicked();

  private:
  button* crop_edit_button_p;
  button* diagnostics_button_p;
  button* stop_program_button_p;
  button* back_button_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
