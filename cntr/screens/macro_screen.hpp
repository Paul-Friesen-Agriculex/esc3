#ifndef macro_screen_hpp
#define macro_screen_hpp

#include <QTableWidget>			//TEST~~~ building macro creator menu and dispaly GPIO_keyboard
#include <QTableWidgetItem>		//TEST~~~ table items for tablewidget
#include "centre.hpp"

#include "gpio_keyboard.hpp"	//TEST~~~ interfacing gpio_keyboard output, connect properly to "centre.cpp/hpp"

#include <QMessageBox>			//TEST~~~ macro table popup menu
#include <QInputDialog>			//TEST~~~ alternative to dialog window, retrieve user input
#include <QStringList>			//TEST~~~
#include <QTextStream>			//TEST~~~ for streaming out Qstrings
#include <QString>
#include "table.hpp"  //TEST~~~ 
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
  //gpio_keyboard_macros* macros[10];	//TEST~~~
  //struct gpio_keyboard_macros
  
  public slots:
  //void usb_serial_out(QString macro_string);	//original
  void usb_serial_out(int bar1, int bar2, int bar3, int bar4, int totalize_count, int weight);	      //totalize mode
  void usb_serial_out(int lotcode, int packcode, int batch_count, int substitution, int dump_count);	//batch mode

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
  void cout_macros_button_clicked();	//OMIT~~~	//use to simulate count triggers
  void check_serial_connection();		//TEST~~~
  void load_macros(); //TEST~~~
  void load_macros_2(int bar1, int bar2, int bar3, int bar4, int totalize_count, int weight); //TEST~~~ //build from scatch

  private:
  button* back_button_p;
  button* ok_button_p;
  button* help_button_p;
  button* disable_all_button_p;
  button* enable_all_button_p;
  button* cout_macros_button_p;			//OMIT~~~
  button* cout_macros_button_2_p;		//OMIT~~~
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
  const int macro_button_width = 135;	//TEST~~~
  const int macro_button_height = 45;
  const int macro_cols = 5;
  const int macro_rows = 10;
  centre* centre_p;
  gpio_keyboard* gpio_keyboard_p;	//OMIT~~~
  //table* table_p; //TEST~~~
  
  
  //int bar1;     //TEST~~~
  //int bar2;     //TEST~~~
  //int bar3;     //TEST~~~
  //int bar4;     //TEST~~~
  
  int bar_1;             //TEST~~~
  int bar_2;             //TEST~~~
  int bar_3;             //TEST~~~
  int bar_4;             //TEST~~~
  int totalize_count;   //TEST~~~
  int weight;           //TEST~~~
  int lot_num;          //TEST~~~
  int pack_num;         //TEST~~~
  
  //QString
};

#endif
  
