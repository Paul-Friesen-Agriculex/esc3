#ifndef SECONDARY_PROCESSOR_HPP
#define SECONDARY_PROCESSOR_HPP

#include <QList>
#include <QSet>

//class centre;
//class processor;
class secondary_processor;


class secondary_processor 
{
  processor* processor_p;
  blob* blob_p;

  
  public:
  QList<blob*> blob_list;
  char** display_raster_p;  //TEST~~~
  bool** bool_raster_p;     //TEST~~~
  
  int width;  //raster dimensions//
  int height;
  
  int raster_width;   //TEST~~~//
  int raster_height;  //TEST~~~//
  int raster_area;    //TEST~~~//
  
  //QList<int> overlap_raster_image_index_list;
  
  //private:
  //QList<int> overlap_raster_image_index_list;
  
  
//=============================================================================================================================================//
//=======================================================VARIABLES FROM PROCESSOR - BLOB=======================================================//
//=============================================================================================================================================//
  //int largest_dimension;  //behaves incorrectly, re-declared as global variable in .cpp file//
  int area;
  //int number_listed_rasters;                      //re-declared as global variable in .cpp file//
  //static const int calibration_seed_number = 50;  //re-declared as global//
  int seed_number_overlap_test; 	//number of overlapped seeds found using overlap algorithm//
  
  float x_scale_factor;			//scaling factor to fit full size images to 20x20 images
  float y_scale_factor;     //TEST~~~//12_21_2018//
  
  
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
  char* display_rotated_raster_p;

  static const float similar_threshold = 0.10; 	//-----------------------defines threshold for two rasters to be "similar"
  //static const float similar_threshold = 10.0;  //TEST~~~// calibration threshold needs adjustment

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
  
  int index_position; 				          //----------------- rotated raster image storage -------------------/
  QList<float> full_score_list;		      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  QList<int> similar_raster_list;       //list of very similar rasters to be removed
  //QList<int> camera_seed;
  //QList<int> index_list;
//=============================================================================================================================================//
  //FUNCTIONS FOR RUNNING OVERLAP TEST OR RETRIEVING OVERLAP CALIBRATION DATA//
  
  //processor* processor_p; //11_21_2018//
  public slots:
  //secondary_processor(); //TEST~~~
  //secondary_processor(int width, int height, int area, bool* bool_raster_p, char* display_raster_p); //TEST~~~
  secondary_processor(int width, int height, int area, bool* bool_raster_p); //ORIGINAL~~~//
  
  
//=============================================================================================================================================//
//=======================================================FUNCTIONS FROM PROCESSOR - BLOB=======================================================//
//=============================================================================================================================================//

  //FUNCTIONS FOR OVERLAP ALGORITHM//
  void store_raw_image_rasters(bool* bool_raster_p);		        //--- TEST~~~//
  
  //void store_rotated_rasters();			    //Original~~~//
  void store_rotated_rasters_2();       //TEST~~~//12_21_2018// modified implementation of original compare_rasters function  
  
  void remove_similar_rasters();		        //--- compare and remove similar seeds ---//
  void remove_similar_rasters_2();	//updated remove_similar_rasters~~~~~
  void remove_similar_rasters_3();   //TEST~~~ just for printout

  void raster_comparison(int cluster_num, bool* bool_raster_p);  //--TEST~~~~~~ troubles with persistance and pointer to array defined in main processor
  //void raster_comparison(int cluster_num);                     //--TEST~~~~~~ ideal minimal number of parameters to pass
 
  bool write_calibration_data_file();	      //--- writes calibration data to file if none exist	---//
  
  bool write_diagnostic_data_file(float seed_number_overlap_test, float score, float best_score, float camera_seed_size, 
                                  float best_fit_size, float variability_score, float larger_area);	                      //	===Generate Diagnostic Data File===	//	//OMIT~~~
  bool read_calibration_data_file();	      //--- reads calibration to QList for comparing camera images	---//
  //void resize_calibration_data();		      //--- resizes calibration data matrices to rmax_x_store dimensions ---// OMIT~~~
  //void print_resized_rasters();			      //--- prints resized rasters in 20x20	---//
  //void print_resized_rasters_2(int display_width, int display_height);			//	---display to dynamic display container--//
  void calculate_threshold_values();	    //--- calculates thresholds and stores into file	---//
  
  void print_rotated_rasters();
  void print_resized_rasters();
  
//=============================================================================================================================================//
};

#endif
