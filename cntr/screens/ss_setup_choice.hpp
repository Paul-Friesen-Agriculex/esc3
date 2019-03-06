#ifndef ss_setup_choice_hpp
#define ss_setup_choice_hpp

#include <QString>
#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QDir;
class QStringList;

class ss_setup_choice : public screen
{
  Q_OBJECT
  
  public:
  ss_setup_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~ss_setup_choice();
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void more_ss_setups_clicked();
  void add_ss_setup_clicked();
  void delete_ss_setup_clicked();
  void back_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* more_ss_setups_p;
  button* add_ss_setup_p;
  button* delete_ss_setup_p;
  button* back_p;
  
  void display_ss_setups();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_ss_setup_displayed;
  
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  
  QDir* dir_p;
  QStringList* ss_setup_list_p;
  QString ss_setup_path;
};
#endif
