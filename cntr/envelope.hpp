
#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <QWidget>
#include <QString>
#include <QList>

class QImage;
struct spreadsheet_column;

enum envelope_field_type{Ubuntu_mono, code_39};

struct envelope_field
{
  spreadsheet_column* column_p;
  envelope_field_type type;
  int x;
  int y;
  int h;
};

class envelope : public QObject
{
  Q_OBJECT

  private:
  QList<envelope_field> field_list;
  int selected_field;
  static const float pixels_per_mm = 25;
  void refresh_image();

  //envelope dimensions mm
  int width;
  int height;

  //default values for next field entry
  int x;
  int y;
  int h;
  envelope_field_type type;

  int sample_row;//row number of spreadsheet line to use as sample
  
  public:
  envelope();
  ~envelope();
  QImage* image_p;
  void set_size(int width, int height);//dimensions mm - printed size
  int get_width();
  int get_height();
  
  //position of selected field
  int get_selected_x();
  int get_selected_y();
  int get_selected_h();
  envelope_field_type get_selected_type();
  
  void move_selected_x(int val);
  void move_selected_y(int val);
  void move_selected_h(int val);
  void change_selected_type(envelope_field_type val);
  
  void enter_field(spreadsheet_column* column_p_s);
  int delete_field(QString heading);//return 1 on success, 0 on failure
  void print();
};

#endif

