#ifndef macro_screen_hpp
#define macro_screen_hpp

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QMessageBox>			//help menu popup//
#include <QInputDialog>			//TEST~~~ alternative to dialog window, retrieve user input
//#include <QStringList>			  //TEST~~~
//#include <QTextStream>			  //TEST~~~ for streaming out Qstrings

#include "table.hpp"
#include "centre.hpp"
//#include <QDialogButtonBox>		//TEST~~~

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
  QString text;
  
  public slots:  
  void usb_serial_out(QString bar_str_1, QString bar_str_2, QString bar_str_3, QString bar_str_4, QString totalize_str_count, QString totalize_str_weight);
  void usb_serial_out(QString lotcode_str, QString packcode_str, QString batch_count_str, QString substitution_str, QString dump_count_str);
  
  private slots:
  void back_button_clicked();
  void ok_button_clicked();
  void help_button_clicked();
  void enable_all_clicked();
  void disable_all_clicked();
  void initialize_macro_menu();
  void cellSelected(int nRow, int nCol);
  void load_macro_table();
  void store_macro_table();
  void dialogbox_buttons(int n);
  void check_serial_connection();		  //TEST~~~
  void communications_choice_toggled(bool);

  private:
  button* back_button_p;
  button* ok_button_p;
  button* help_button_p;
  button* disable_all_button_p;
  button* enable_all_button_p;
  help_screen* help_screen_p;
  
  QGroupBox* communications_choice_box_p;
  QVBoxLayout* communications_choice_box_layout_p;
  QRadioButton* communicate_by_keyboard_cable_button_p;
  QRadioButton* communicate_by_tcp_button_p;
  
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
  QString macro_function_string;

  static const int macro_button_width = 135;
  static const int macro_button_height = 35;
  static const int macro_cols = 5;
  static const int macro_rows = 10;
  centre* centre_p;
  
  QString bar_str_1;            //Totalize variables//
  QString bar_str_2;
  QString bar_str_3;
  QString bar_str_4;
  QString totalize_str_count;
  QString totalize_str_weight;
  //============================================//
  QString totalize_count_str;
  QString weight_str;
  //============================================//
  
  QString lotcode_str;         //Batch variables//
  QString packcode_str; 
  QString batch_count_str;
  QString substitution_str;
  QString dump_count_str;
  
  int current_row;
  int current_column;
  
  protected:
  QString* macro_name_string;  //TEST~~~ 11_13_2018//
};

#endif
  
