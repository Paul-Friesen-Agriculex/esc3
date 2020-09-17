#ifndef macro_screen_hpp
#define macro_screen_hpp

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QMessageBox>			//help menu popup//
#include <QInputDialog>			//TEST~~~ alternative to dialog window, retrieve user input

#include "table.hpp"
#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;
class help_screen;

class macro_screen : public screen
{
  Q_OBJECT
  
  public:
  macro_screen(centre*set_centre_p);
  ~macro_screen();
  QString text;
  
  private slots:
  void back_button_clicked();
  void ok_button_clicked();
  void help_button_clicked();
  void initialize_macro_menu();
  void cellSelected(int nRow, int nCol);
  void load_macro_table();
  void toggle_macro(int macro_number);

  private:
  button* back_button_p;
  button* ok_button_p;
  button* help_button_p;
  help_screen* help_screen_p;
  QLabel* instruction_label_p;
  QLineEdit *lineEdit;
  bool serialusb_connected;

  QGridLayout* main_layout_p;

  QTableWidget* tableWidget_p;
  QGroupBox *formGroupBox;

  QTableWidget* m_pTableWidget;
  QStringList m_TableHeader;
  QTableWidgetItem* tableWidgetItem_p;
  QLabel* tableWidget_label_p;
  QLabel* screen_title_label_p;
  QLabel* description_p;
  QMessageBox msgBox;
  static const int macro_cols = 5;
  static const int macro_rows = 10;
  centre* centre_p;

  int current_row;
  int current_column;
  
  protected:
  QString* macro_name_string;
};

#endif
  
