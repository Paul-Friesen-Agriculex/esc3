#ifndef crop_hpp
#define crop_hpp


struct crop
{
  QString name;
  int sensitivity;
  int gate_setting;
  int area_mean;
  int area_stdev;
  float max_inflection;
  float inflection_count_mean;
  bool calibrated;
  
  int high_feed_speed;
  int low_feed_speed;
  int dump_speed;
};

#endif
