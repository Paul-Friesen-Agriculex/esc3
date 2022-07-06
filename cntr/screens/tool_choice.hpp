#ifndef tool_choice_hpp
#define tool_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QRadioButton;
class help_screen;

class tool_choice : public screen
{
  Q_OBJECT
  
  public:
  tool_choice(centre*set_centre_p);
  
  private slots:
  void about_button_clicked();
  void crop_edit_clicked();
  void communications_setup_button_clicked();
  void diagnostics_clicked();
  void stop_program_clicked();
  void back_button_clicked();
  void signal_port_pulse_when_endgate_closes(bool state);

  private:
  button* about_button_p;
  button* crop_edit_button_p;
  button* communications_setup_button_p;
  button* diagnostics_button_p;
  button* stop_program_button_p;
  button* back_button_p;
  QRadioButton* signal_port_pulse_when_endgate_closes_button_p;

  QGridLayout* main_layout_p;
  help_screen* help_screen_p;
//  centre* centre_p;
};
#endif
