#ifndef macro_screen_hpp
#define macro_screen_hpp

#include <QTableWidget>			  //TEST~~~ building macro creator menu and dispaly GPIO_keyboard
#include <QTableWidgetItem>		//TEST~~~ table items for tablewidget
#include "centre.hpp"

#include <QMessageBox>			//TEST~~~ macro table popup menu
#include <QInputDialog>			//TEST~~~ alternative to dialog window, retrieve user input
#include <QStringList>			//TEST~~~
#include <QTextStream>			//TEST~~~ for streaming out Qstrings
#include <QString>
#include "table.hpp"        //TEST~~~ 
//#include <QDialogButtonBox>		//TEST~~~

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class macro_screen : public screen
{
  Q_OBJECT
  
  public:
  macro_screen(centre*set_centre_p);
  
  public slots:  
  //batch mode - barcodes as characters instead of integers 11_02_2018~~~//
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
  void on_off_button_clicked(int current_row, int current_column);   //TEST~~~

  private:
  button* back_button_p;
  button* ok_button_p;
  button* help_button_p;
  button* disable_all_button_p;
  button* enable_all_button_p;

  QLineEdit *lineEdit;
  bool serialusb_connected;

  QGridLayout* main_layout_p;

  QTableWidget* tableWidget_p;	//TEST~~~ macro window
  QGroupBox *formGroupBox;		  //TEST~~~ multiple input dialog

  QTableWidget* m_pTableWidget;
  QStringList m_TableHeader;
  QTableWidgetItem* tableWidgetItem_p;
  QLabel* tableWidget_label_p;
  QLabel* screen_title_label_p;
  QMessageBox msgBox;
  QString macro_function_string;
  //QString combined_macro_functions; //TEST~~~

  //const int macro_button_width = 210;	//original using QTabWidget
  static const int macro_button_width = 135;	//TEST~~~
  static const int macro_button_height = 45;
  static const int macro_cols = 5;
  static const int macro_rows = 10;
  centre* centre_p;
  
  //QString Alternatives to Integer variables// 11_02_2018~~~//
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
  
  //macro_screen_p -> usb_serial_out(lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str);
  QString lotcode_str;         //Batch variables//
  QString packcode_str; 
  QString batch_count_str;
  QString substitution_str;
  QString dump_count_str;
  
  int current_row;
  int current_column;
};

#endif
  
