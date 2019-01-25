#ifndef crop_hpp
#define crop_hpp


struct crop
{
  QString name;
  int sensitivity;
  int gate_setting;
  int area_mean;
  int area_stdev;
  float max_inflection_1;//measured with arms 1% of outline length
  float max_inflection_3;//measured with arms 3% of outline length
  float max_inflection_9;//measured with arms 9% of outline length
  bool calibrated;
  
  int high_feed_speed;
  int low_feed_speed;
  int dump_speed;
};

#endif
