#ifndef program_delete_hpp
#define program_delete_hpp

#include <QString>
#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QDir;
class QStringList;

class program_delete : public screen
{
  Q_OBJECT
  
  public:
  program_delete(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~program_delete();
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void more_programs_clicked();
//  void add_program_clicked();
//  void delete_program_clicked();
  void back_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* more_programs_p;
//  button* add_program_p;
//  button* delete_program_p;
  button* back_p;
  
  void generate_program_list();
  void display_programs();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_program_displayed;
  
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  
  QDir* dir_p;
  QStringList* program_list_p;
  QString program_path;
};
#endif
