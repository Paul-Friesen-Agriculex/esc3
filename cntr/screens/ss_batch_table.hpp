#ifndef ss_batch_table_hpp
#define ss_batch_table_hpp

#include <QStandardItemModel>
#include <QTableView>
#include <QKeyEvent>
#include <QString>
#include <QList>

class centre;
class batch_mode_driver;
struct spreadsheet_column;

class ss_batch_table:public QTableView
{
  Q_OBJECT
  
  private:
  int model_row, model_column;
  int current_row;
  QStandardItemModel* model_p;
  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
//  friend class batch;  //TEST~~~
  QList<spreadsheet_column*> display_column_pointers;

  public:
  ss_batch_table(centre* centre_p_s, batch_mode_driver* batch_mode_driver_p_s);
  ~ss_batch_table();
//  void set_visible_columns(int num);
  
  public slots:
  void set_current_row(int row);
//  void enter_data(int column, QString data);
  void refresh();
//  void enter_seeds(int num);
//  void enter_seed_lot_barcode(QString barcode);
//  void enter_pack_barcode(QString barcode);
//  void save_file(QString file_name);
//  void load_file(QString file_name);
//  void clear();
  
  signals:
  void focus_on_barcode();//signal to batch to return keyboard focus to barcode
  
};
  
#endif
