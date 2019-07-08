#ifndef batch_table_hpp
#define batch_table_hpp

#include <QStandardItemModel>
#include <QTableView>
#include <QKeyEvent>
#include <QString>

class centre;
class batch_mode_driver;

class batch_table:public QTableView
{
  Q_OBJECT
  
  private:
  int model_row, model_column;
  int filled_rows;
  QStandardItemModel* model_p;
  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  friend class batch;  //TEST~~~

  public:
  batch_table(centre* centre_p_s, batch_mode_driver* batch_mode_driver_p_s);
  ~batch_table();
  void set_visible_columns(int num);
//  void prepare_for_seed_lot_barcode();
//  void prepare_for_pack_barcode();
  
  public slots:
  void enter_seeds(int num);
  void enter_seed_lot_barcode(QString barcode);
  void enter_pack_barcode(QString barcode);
  void save_file(QString file_name);
  void load_file(QString file_name);
  void clear();
  
  signals:
  void focus_on_barcode();//signal to batch to return keyboard focus to barcode
  
//  void barcode_entered(QString value);
  /*
  protected:
  void keyPressEvent(QKeyEvent* event);
  void mouseReleaseEvent(QMouseEvent* e);
  */
};
  
#endif
