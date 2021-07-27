#ifndef ss_setup_entry_hpp
#define ss_setup_entry_hpp

#include <QString>
#include "centre.hpp"
#include "batch_mode_driver.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QDir;
class QStringList;

class ss_setup_entry : public screen
{
  Q_OBJECT
  
  public:
  ss_setup_entry(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~ss_setup_entry();
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void next_headings_clicked();
  void previous_headings_clicked();
  void none_clicked();
  void back_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* next_headings_p;
  button* previous_headings_p;
  button* none_p;
  button* back_p;
  
  void display_headings();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_heading_displayed;
  
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  
  QDir* dir_p;
  
  char mode;//m - material id.  r - required count.  a - actual count.  p - print time.  f - fill time.  d - dump count.  s - substitution.
  
  void enter_choice(int column);
  
  
  QString ss_setup_path;
};  


#endif
