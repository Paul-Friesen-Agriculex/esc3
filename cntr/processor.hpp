#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <QObject>
#include <QString>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QImage>
#include <math.h>
#include "crop.hpp"
#include "dust_streak.hpp"

class processor;
class centre;
//class secondary_processor;  //11_21_2018//

struct slice
{
  long long line;
  int start;
  int end;
};

struct outline_point
{
  int x;
  int y;
  float direction;//estimate of direction of curve in radians from horizontal
  float inflection;//a measure of the degree of inflection.  positive values indicate curve is concave.  negative ones indicate convex.  
};

struct line_segment
{
  int x1, y1, x2, y2;//end points
  float m, b;//slope, y intercept
  bool vertical;//true indicates line is vertical.  m and b values not valid.
};

//----------------- list to store rotated raster images -------------------//
struct rotated_raster
{
	int index_num;	//raster index values where seed is present
	
};
//-------------------------------------------------------------------------//

class blob
{
  public:
  static const int direction_distance = 3;//number of positions forward and backward in outline to count to estimate outline direction.
  static const int inflection_distance = 3;//number of positions forward and backward in outline to count to estimate inflection
  static const float inflection_tolerance = 0.1;//min. inflection to count as an inflection during calibration.
  static const float inflection_sensitivity_factor = 1.8;//values above 1 reduce the likelihood of detecting an inflection.
  blob(int start, int end, processor* set_processor_p);
  ~blob();
  bool offer(int slice_start, int slice_end);//true indicates slice accepted
  void combine(int combine_index);
  void form_raster_image();
  void form_outline();
  void form_display_raster();
  void print_display_raster();
  void calculate_characteristics();
  int seeds_in_blob();
  
  processor* processor_p;
  bool remove;//true indicates this blob should be removed
  int area;
  int inflection_count;
  float max_inflection;  
  
  bool took_slice;//true indicates that this blob already took a slice on this line.
  //if a blob gets no slice on a line, it is complete
  
  QList<slice*> slice_list;
  bool* bool_raster_p;
  char* display_raster_p;
  int min_x, max_x;
  long long first_line, last_line;
  int width;
  int height;
  

  QList<outline_point*> outline_point_list;//holds a list of points tracing the outline of the blob.  Starts with the leftmost point of the bottom line.
  int ol_x, ol_y;
  int ol_angle;//while tracing around the blob, this holds the angle of the last move.
  int ol_try_x, ol_try_y;
  void ol_try_start(int start_angle);//try advancing 1 position in this direction.  If on outline, sets trace_x, trace_y, and advance_angle. 
    //tries other angles advancing counterclockwise in 45 degree steps.
  void ol_shift_point(int angle);//sets ol_try_x and ol_try_y 
  int ol_length;//number of entries in outline_point_list
  
  bool calibrating;
  static const int max_seeds = 5;
  float expected_mean_area[max_seeds];//a blob with i seeds is expected to have an area of expected_mean_area[i] on average
  float expected_variance_area[max_seeds];
  int score_area[max_seeds];
  int score_mi[max_seeds];
  int score_ic[max_seeds];
  int score_likelihood[max_seeds];//high numbers of seeds in a blob are unlikely
  
  //functions to check blobs identified as multiple seeds by previous algorithm, which can be fooled by debris
  int inflection_x[100];//x co-ordinates of inflection points
  int inflection_y[100];//y co-ordinates of inflection points
  int inflection_i[100];//indexes of inflection points in outline_point_list
  
  //when line_count in processor reaches finish_line, it will finish this blob by determining number of seeds and deleting.
  //this allows a delay for dust streaks to be detected before the number of seeds is determined.
  long long finish_line;

//==============================================================================================================//  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //TEST Functions for guided overlap algorithm functionality//
  void kmeans_clustering(int cluster_num); //testing k_means_clustering techniques
  
  void silhouette_scoring();      //calculate and scores fitting cluster configuration
  
  void overlap_test(int cluster_num);      //cluster assisted overlap algorithm ~~~ Single Cluster
  void overlap_test_multiple(int cluster_num);      //cluster assisted overlap algorithm ~~~ Multiple Clusters
      //TEST FUNCTION~~~
  
//==============================================================================================================//
  //FUNCTIONS FOR OVERLAP ALGORITHM//
  void store_raw_image_rasters();		    //--- if no calibration data found, stores first 50 seeds to a QList	---//
  void store_rotated_rasters();			    //------------------------------------
  void print_rotated_rasters(); 		    //--- functions same as print display rasters --- //
  void remove_similar_rasters();		    //--- compare and remove similar seeds ---//
  //void raster_comparison();				      //--- compares first 400 rasters to following 50 seeds	---//
  void raster_comparison(int cluster_num);				      //--TEST~~~~~~
  bool write_calibration_data_file();	  //--- writes calibration data to file if none exist	---//
  
  bool write_diagnostic_data_file(float seed_number_overlap_test, float score, float best_score, float camera_seed_size, 
                                  float best_fit_size, float variability_score, float larger_area);	                      //	===Generate Diagnostic Data File===	//	//OMIT~~~
  bool read_calibration_data_file();	  //--- reads calibration to QList for comparing camera images	---//
  //void resize_calibration_data();		  //--- resizes calibration data matrices to rmax_x_store dimensions ---// OMIT~~~
  void print_resized_rasters();			    //--- prints resized rasters in 20x20	---//
  void print_resized_rasters_2(int display_width, int display_height);			//	---display to dynamic display container--//
  void calculate_threshold_values();	  //--- calculates thresholds and stores into file	---//
  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//==============================================================================================================//  
  //variables for centroid approximations generated by kmeans_clustering()//
  int cluster_num;                //K-means clustiner//
  int seed_number_overlap_test; 	//number of overlapped seeds found using overlap algorithm//

  int c1_mean_x, c2_mean_x, c3_mean_x, c4_mean_x;
  int c1_mean_y, c2_mean_y, c3_mean_y, c4_mean_y;
  int c1_x, c2_x, c3_x, c4_x;
  int c1_y, c2_y, c3_y, c4_y;
  float percentage_c1_x, percentage_c2_x, percentage_c3_x, percentage_c4_x;
  float percentage_c1_y, percentage_c2_y, percentage_c3_y, percentage_c4_y;
  
  //int lcx[4]; //OMIT~~~
  //int lcy[4]; //OMIT~~~
  static const int number_overlaps_to_test = 4;
  int centroid_x[number_overlaps_to_test];  //centroid(s) of camera or image file seed
  int centroid_y[number_overlaps_to_test];  //
  
  int centroid_lx[number_overlaps_to_test]; //centroid(s) of library seeds
  int centroid_ly[number_overlaps_to_test]; //
  
  int static_x_shift_l1[number_overlaps_to_test];
  
//==============================================================================================================//

  static const float similar_threshold = 0.10; 	//-----------------------defines threshold for two rasters to be "similar"
  
  //------------- variables used in calculating standard deviation ------------ //
  //float width_mean, height_mean, area_mean;								//-----------------------
  //float std_deviation_width, std_deviation_height, std_deviation_area;	//-----------------------
  
//=====================================================================================================================================================================//  
  //original overlap test variables + functions//
//----------store_raw_image_rasters----------//  
  static const int calibration_seed_number = 50;		//number of seeds to be used for calibration data//     //store_raw_image_rasters()//
  //int largest_dimension;						              //holds the largest seed image width or height in the calibration data set
  //int number_listed_rasters;          //initialize in constructor + omit global declaration in .cpp//
  //QList<int> raw_image_list;		      //contains calibration seed raster images//
  //QList<int> raw_image_zeroes_list;   //contains start+end points of raster images defined by zeroes and seed dimensions//
  //QList<int> raw_image_size_list;     //contains areas of calibration seeds//
  
  
//----------store_rotated_image_rasters----------//  
  float x_scale_factor;							//scaling factor to fit full size images to 20x20 images
  //float y_scale_factor;						//
  static const float scaling_buffer = 0.20;		  //-----------------------% buffer for scaling affect
  
  double half_width;		  //midpoint of seed raster                             //centering image 
  double half_height;
  double start_raster_x;	//start point of raster image in container
  double start_raster_y;  
  double end_raster_x;		//end point of raster image in container
  double end_raster_y;

  static const double pi = 3.1415926535897;
  float theta;
  int raster_container_width;
  int raster_position_in_container;	//possibly omit  

    //----------------- variables used in rotated image rasters ----------------- //
  static const int rmax_x = 20;  //static container dimensions
  static const int rmax_y = 20;
  static const int rmax_x_store = 40;			//-store image rasters into compressed 20x20 matrices
  static const int rmax_y_store = 40;			//
  double shifted_x;			  //Remap seed raster index into 2D
  double shifted_y;		    //changed from double to int to round Y to whole numbers
  double rotated_x; 
  double rotated_y;
  double unshifted_x;
  int unshifted_y;		 
  
  float average_calibration_seed_size;  //remove original declaraction as global variable//

  
  char* display_rotated_raster_p; 	    //----------------- raster_image_container-------------------//
  int index_position; 				          //----------------- rotated raster image storage -------------------/
  //QList<int> index_list;			        //save raster index values for first 50 seeds~~~~~~~~~~~~~~~~~~~~~~~~~~~
  QList<float> full_score_list;		      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //QList<int> camera_seed; 		        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  QList<int> similar_raster_list;       //list of very similar rasters to be removed
//=====================================================================================================================================================================//  
  //moving overlap test to another file and class //11_21_2018//

};


class processor : public QObject
{
  Q_OBJECT
  
  public:
  processor();
  ~processor();
  void increment_count(int increment);
  int took_slice_index;//index in blob_list of blob that took a slice.  
  //If another blob wants same slice, two should combine. -1 indicates empty.
//  static const int dust_streak_line_allowance = 110;//number of lines to wait after blob completion to calculate number of seeds.  This delay is to allow dust streaks to be detected, so that inflections near dust streaks can be ingnored.
  static const int dust_streak_line_allowance = 0;//number of lines to wait after blob completion to calculate number of seeds.  This delay is to allow dust streaks to be detected, so that inflections near dust streaks can be ingnored.
  
  QList<blob*> blob_list;
  long long line_count;
  crop current_crop;
  bool show_blob_bool;//true means show blobs in terminal window
  dust_streak dust_streak_list[2048];
  bool dust_streak_present;//true value indicates dust on window.  should be cleaned.
  float dust_streak_percentage();//returns percentage of width in which inflections will not be counted due to dust streaks
  
  //secondary_processor* secondary_processor_p; //TEST~~~ //11_21_2018//

  
  public slots:
  void set_camera_processing(bool state);
  void playback_line();//advance 1 line in playback
  void get_qimage();
  void set_to_record(bool record_now);
  void set_crop(crop current_crop_set);
  void show_blob_on_terminal(bool show_blob_bool_set);
  void restart_calibration();
  void save_image(QString filename);
  void load_image(QString filename);
  void reset_time_tests();//testing
  
  signals:
  void count_out(int count_incr);
  void send_qimage(QImage qimage);
  void send_calibrated_crop(crop calibrated_crop);
  void end_of_playback();
  void send_cycle_time(int value);
  void send_message(QString message);
  
  private:
  
  void calibrate();
  void new_image(unsigned char* pixel_p);
  void add_line(unsigned char* start_p);

  unsigned char* image_p;
  int image_size;
  int start_slice;//start of next slice of matter
  int end_slice;
  
//  int valid_start;
//  int valid_end;//ignore pixels outside valid range

  
  //testing
  int run_number;
  QTime t;
  int image_processing_time;//milliseconds
  int max_image_processing_time;//milliseconds
  int slack_time;//milliseconds from end of processing to start of next image
  int min_slack_time;//milliseconds from end of processing to start of next image
  bool first_cycle;//will be true when program starts, but false after that.  When true, processing time and slack time not valid.
  int image_cycle_time;//miliseconds for complete image cycle
  
  
  int detect_size;//minimum area in pixels to be detected as a seed
  
  int calibration_area_list[50];
  int calibration_area_list_index;
  float calibration_max_inflection_list[50];
  int calibration_inflection_count_list[50];
  crop calibration_crop;
  
  bool record_this_image;
  bool play_back;//true - play back recorded image.  false - process camera input
  int big_image_size;//in bytes
  int big_image_lines;//total number of lines
  unsigned char* big_image_start_p;//points to start of first line
  unsigned char* big_image_last_line_p;//points to start of last line
  unsigned char* big_image_end_p;//points to one position beyond end of image
  unsigned char* big_image_end_recording_p;//points to start of last line recorded
  unsigned char* big_image_position_p;//points to start of current line for either recording or playing back
  
  unsigned char* rbi_start_p;//points to start of re-ordered big image.  Oldest line is at start, newest one at end
  unsigned char* qimage_top_p;
  unsigned char* qimage_bottom_p;
  static const int lines_to_display = 500;
  static const int lines_above_sense_line = 300;
  int qimage_sense_line;
  int qimage_count_line;
  int qimage_top_line;
  int qimage_bottom_line;
  
//==============================================================================================================//
  bool calibration_data;	  //-----------------------true means calibration data exists in file-------------------//
  int seed_number_overlap;  //TEST~~~ used to retrieve number of expected seeds in blob
//==============================================================================================================//
    
  private slots:
  void run();//processes camera images
  
  public:
  int get_detect_size() {return detect_size;};
  
};

#endif

