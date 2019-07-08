#ifndef DUST_STREAK_HPP
#define DUST_STREAK_HPP


class processor;
class centre;

class dust_streak//each dust_streak is for a single pixel.  detects if obscured by dust on camera window
{
  long long length;
  static const int detect_length = 100;//if a pixel is obscured more lines than this, assume a dust streak
  static const int detect_value = 150;//pixels with less than this intensity considered obscured
  
  public:
  bool started;//true if pixel is below detect_value.  could be due to a seed.
  bool confirmed;//true if pixel has been below detect_value for detect_length lines
  bool in_streak_zone;//this is set by the processor.  it is true for pixels where a streak is confirmed, and also for several pixels on either side.  
  //inflections in streak zone are ignored.
  
  dust_streak();
  void add_line(int pixel_value);
};

#endif
