#ifndef table_view_hpp
#define table_view_hpp

#include <QStandardItemModel>
#include <QTableView>
#include <QKeyEvent>
#include <QString>

class centre;

class table:public QTableView
{
  Q_OBJECT
  
  private:
  int model_row, model_column;
  QStandardItemModel* model_p;
  centre* centre_p;
  friend class totalize;  //TEST~~~

  public:
  table(centre* centre_p_s);
  void set_visible_columns(int num);
  
  public slots:
  void enter_seeds(int num);
  void enter_weight(int num);
  void save_file(QString file_name);
  void load_file(QString file_name);
  void clear();
  
  protected:
  void keyPressEvent(QKeyEvent* event);
  void mouseReleaseEvent(QMouseEvent* e);
};
  
#endif
