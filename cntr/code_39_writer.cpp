#include <QString>
#include <QPainter>

#include "code_39_writer.hpp"


code_39_writer::code_39_writer(QPainter* target_p_s)
{
  target_p = target_p_s;
  target_p -> setBrush(QBrush(QColor(0,0,0)));
  width_factor = 2;
  height = 1000;
  narrow_width = 100;//of narrow bar
  wide_width = width_factor*narrow_width;
  start_x = 0;
  start_y = 0;
}

void code_39_writer::draw_bars(char w1, char w2, char w3, char w4, char w5, char w6, char w7, char w8, char w9)
{
  if(w1 == 'w')
  {
    target_p -> drawRect(x, y, wide_width, height);
    x += wide_width;
  }
  else
  {
    target_p -> drawRect(x, y, narrow_width, height);
    x += narrow_width;
  }
  if(w2 == 'w')
  {
    x += wide_width;
  }
  else
  {
    x += narrow_width;
  }
  if(w3 == 'w')
  {
    target_p -> drawRect(x, y, wide_width, height);
    x += wide_width;
  }
  else
  {
    target_p -> drawRect(x, y, narrow_width, height);
    x += narrow_width;
  }
  if(w4 == 'w')
  {
    x += wide_width;
  }
  else
  {
    x += narrow_width;
  }
  if(w5 == 'w')
  {
    target_p -> drawRect(x, y, wide_width, height);
    x += wide_width;
  }
  else
  {
    target_p -> drawRect(x, y, narrow_width, height);
    x += narrow_width;
  }
  if(w6 == 'w')
  {
    x += wide_width;
  }
  else
  {
    x += narrow_width;
  }
  if(w7 == 'w')
  {
    target_p -> drawRect(x, y, wide_width, height);
    x += wide_width;
  }
  else
  {
    target_p -> drawRect(x, y, narrow_width, height);
    x += narrow_width;
  }
  if(w8 == 'w')
  {
    x += wide_width;
  }
  else
  {
    x += narrow_width;
  }
  if(w9 == 'w')
  {
    target_p -> drawRect(x, y, wide_width, height);
    x += wide_width;
  }
  else
  {
    target_p -> drawRect(x, y, narrow_width, height);
    x += narrow_width;
  }
  x += narrow_width;
}
  
void code_39_writer::set_size(int height_s, int narrow_width_s)
{
  height = height_s;
  narrow_width = narrow_width_s;
  wide_width = narrow_width*width_factor;
}
  
void code_39_writer::write(int x_s, int y_s, QString* text_p)
{
  start_x = x = x_s;
  start_y = y = y_s;
  draw_bars('n','w','n','n','w','n','w','n','n');
  for (int i=0; i<text_p->size(); ++i)
  {
    QChar c = text_p -> at(i);
    if(c=='A')draw_bars('w','n','n','n','n','w','n','n','w');
    else if(c=='B')draw_bars('n','n','w','n','n','w','n','n','w');
    else if(c=='C')draw_bars('w','n','w','n','n','w','n','n','n');
    else if(c=='D')draw_bars('n','n','n','n','w','w','n','n','w');
    else if(c=='E')draw_bars('w','n','n','n','w','w','n','n','n');
    else if(c=='F')draw_bars('n','n','w','n','w','w','n','n','n');
    else if(c=='G')draw_bars('n','n','n','n','n','w','w','n','w');
    else if(c=='H')draw_bars('w','n','n','n','n','w','w','n','n');
    else if(c=='I')draw_bars('n','n','w','n','n','w','w','n','n');
    else if(c=='J')draw_bars('n','n','n','n','w','w','w','n','n');
    else if(c=='K')draw_bars('w','n','n','n','n','n','n','w','w');
    else if(c=='L')draw_bars('n','n','w','n','n','n','n','w','w');
    else if(c=='M')draw_bars('w','n','w','n','n','n','n','w','n');
    else if(c=='N')draw_bars('n','n','n','n','w','n','n','w','w');
    else if(c=='O')draw_bars('w','n','n','n','w','n','n','w','n');
    else if(c=='P')draw_bars('n','n','w','n','w','n','n','w','n');
    else if(c=='Q')draw_bars('n','n','n','n','n','n','w','w','w');
    else if(c=='R')draw_bars('w','n','n','n','n','n','w','w','n');
    else if(c=='S')draw_bars('n','n','w','n','n','n','w','w','n');
    else if(c=='T')draw_bars('n','n','n','n','w','n','w','w','n');
    else if(c=='U')draw_bars('w','w','n','n','n','n','n','n','w');
    else if(c=='V')draw_bars('n','w','w','n','n','n','n','n','w');
    else if(c=='W')draw_bars('w','w','w','n','n','n','n','n','n');
    else if(c=='X')draw_bars('n','w','n','n','w','n','n','n','w');
    else if(c=='Y')draw_bars('w','w','n','n','w','n','n','n','n');
    else if(c=='Z')draw_bars('n','w','w','n','w','n','n','n','n');
    else if(c=='0')draw_bars('n','n','n','w','w','n','w','n','n');
    else if(c=='1')draw_bars('w','n','n','w','n','n','n','n','w');
    else if(c=='2')draw_bars('n','n','w','w','n','n','n','n','w');
    else if(c=='3')draw_bars('w','n','w','w','n','n','n','n','n');
    else if(c=='4')draw_bars('n','n','n','w','w','n','n','n','w');
    else if(c=='5')draw_bars('w','n','n','w','w','n','n','n','n');
    else if(c=='6')draw_bars('n','n','w','w','w','n','n','n','n');
    else if(c=='7')draw_bars('n','n','n','w','n','n','w','n','w');
    else if(c=='8')draw_bars('w','n','n','w','n','n','w','n','n');
    else if(c=='9')draw_bars('n','n','w','w','n','n','w','n','n');
    else if(c==' ')draw_bars('n','w','w','n','n','n','w','n','n');
    else if(c=='-')draw_bars('n','w','n','n','n','n','w','n','w');
    else if(c=='$')draw_bars('n','w','n','w','n','w','n','n','n');
    else if(c=='%')draw_bars('n','n','n','w','n','w','n','w','n');
    else if(c=='.')draw_bars('w','w','n','n','n','n','w','n','n');
    else if(c=='/')draw_bars('n','w','n','w','n','n','n','w','n');
    else if(c=='+')draw_bars('n','w','n','n','n','w','n','w','n');
    else draw_bars('n','w','n','n','n','n','w','n','w');//uncodeable.  print '-'
  }
  draw_bars('n','w','n','n','w','n','w','n','n');  
}
  
    

