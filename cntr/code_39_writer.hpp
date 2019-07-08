#ifndef CODE_39_WRITER_HPP
#define CODE_39_WRITER_HPP


class QString;
class QPainter;

class code_39_writer
{
  private:
  
  QPainter* target_p;
  int height;
  int narrow_width;//of narrow bar
  int wide_width;
  float width_factor;
  int start_x;
  int start_y;
  int x;
  int y;
  void draw_bars(char w1, char w2, char w3, char w4, char w5, char w6, char w7, char w8, char w9);
  
  public:
  code_39_writer(QPainter* target_p_s);
  void set_size(int height_s, int narrow_width_s);
  void write(int x_s, int y_s, QString* text_p);
};
  
  


#endif
