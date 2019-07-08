#ifndef enter_program_hpp
#define enter_program_hpp

#include <QList>
#include "centre.hpp"
#include "batch_mode_driver.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QString;
class QTextEdit;
class QTimer;
class keypad;

class enter_program : public screen
{
  Q_OBJECT
  
  public:
  enter_program(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~enter_program();
  
  private slots:
  void options_clicked();
  void back_clicked();
  void number_entered(int value);
  void help_clicked();
  void done_clicked();
  
  private:
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  QLabel* prompt_p;
  keypad* keypad_p;
  button* options_button_p;
  QTextEdit* edit_p;
  button* back_button_p;
  button* help_button_p;
  button* done_button_p;
  QGridLayout* layout_p;
  
  bm_set* bm_set_p;
  QString* program_string_p;
  void update_program_string();
  char enter_flag;//'p' -> number of packs.  's' -> number of seeds per pack
  int current_packs;
  int current_seeds;
};
#endif
