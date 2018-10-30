#ifndef playback_hpp
#define playback_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QLabel;
class QImage;
class QTimer;
class QSlider;
class QLineEdit;
class display;
class blobdisp;

class playback : public screen
{
  Q_OBJECT
  
  public:
  playback(centre*set_centre_p);
  ~playback();
  
  private slots:
  void autoadvance_clicked();
  void back_clicked();
  void zero_clicked();
  void advance1_clicked();
  void advance10_clicked();
  void advance100_clicked();
  void advance_n();
  void set_n(int n_s);
  void quit_clicked();
  void run();
  void save_image();
  void load_image();
  
  private:
  QLabel* title_p;
  button* back_button_p;
  QLabel* count_message_p;
  button* zero_button_p;
  button* advance1_button_p;
  button* advance10_button_p;
  button* advance100_button_p;
  button* file_button_p;
  button* load_file_button_p;
  button* quit_button_p;
  button* autoadvance_button_p;
  QSlider* autoadvance_slider_p;
  button* save_image_button_p;
  QLineEdit* file_name_p;
  QGroupBox* autoadvance_box_p;
  QGridLayout* autoadvance_layout_p;
  display* display_p;
  blobdisp* blobdisp_p;

  QGridLayout* main_layout_p;
  
  QTimer* qtimer_p;
  QTimer* autoadvance_timer_p;
  bool autoadvance_on;
  int n;//number of lines to advance on each autoadvance pulse
  
  
  signals:
  void send_line(unsigned char* line_start_p);
};
#endif


