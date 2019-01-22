//secondary processor for handling all functions used for overlap testing//
#include "processor.hpp"
#include "secondary_processor.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include <QtMath>

using namespace std;


//=================================================================================//
//------------- variables used in calculating standard deviation ------------ //
float width_mean, height_mean, area_mean;								              //TEST~~~
float std_deviation_width, std_deviation_height, std_deviation_area;	//TEST~~~
float variability_score;												                      //TEST~~~

//----------------- display array -------------------//
//rotated_raster* display_rotated_raster_p = new rotated_raster;
rotated_raster* rotated_raster_p = new rotated_raster;
//-------------------rotated_raster_list-indexes-------------------//
QList<int> index_list;	                  //save raster index values for first 50 seeds
QList<int> index_zero_positions_list;	    //save zero positions denoting end of raster

QList<int> raw_image_list;
QList<int> raw_image_zeroes_list;         //contains original raw image raster areas, widths, and heights
QList<int> raw_image_size_list;
      
QList<int> rotated_image_list;            //01_10_2019 same implementation as storing raw raster images//
QList<int> rotated_image_zeroes_list;     //01_10_2019 store start/end points of rasters in rotated_image_list//
QList<float> rotated_image_dimensions;      //01_11_2019 store dimensions + area of rasters//
QList<int> rotated_image_centroids;   //01_14_2019 list of centroids//

float largest_dimension = 0;              //TEST~~~~	//Should be this
int number_listed_rasters = 0;

float average_calibration_seed_size = 0;	      //TEST~~~
//const int calibration_seed_number = 50;		    //used to define the number of seeds to used for calibration data//
const int calibration_seed_number = 10;         //TEST~~~//

//---------------------------------DIAGNOSTICS---------------------------------//
int average_area, average_width, average_height;      //means
int area_sum = 0;
int width_sum = 0;
int height_sum = 0;

int smallest_area = 100000;                           //range
int smallest_width = 100000; 
int smallest_height = 100000;
int largest_area = 0;
int largest_width = 0;
int largest_height = 0;

float area_variance, width_variance, height_variance;     //calculate rolling variance
float area_std_dev, width_std_dev, height_std_dev;        //calculate standard deviations

//============================================================================================================//
//global lists//
QList<int> global_list_test;                  //TEST~~~//

QList<int> secondary_raw_image_list;          //TEST~~~
QList<int> secondary_raw_image_zeroes_list;	  //TEST~~~
QList<int> secondary_raw_image_size_list;		  //TEST~~~

QList<int> secondary_index_list;
QList<int> secondary_index_zero_positions_list;

//============================================================================================================//
secondary_processor::secondary_processor(int raster_width, int raster_height, int raster_area, bool* bool_raster_p)
{  
  width = raster_width; 
  height = raster_height;
  area = raster_area;
  
  seed_number_overlap_test = 1;   //OMIT~~~ placement holder, may need to retrieve number of seeds detected by inflection test//
  
  if(number_listed_rasters < calibration_seed_number) store_raw_image_rasters(bool_raster_p);
  else raster_comparison(seed_number_overlap_test, bool_raster_p);
}
//=============================================================================================================================================//
//=======================================================FUNCTIONS FROM PROCESSOR - BLOB=======================================================//
//=============================================================================================================================================//
void secondary_processor::store_raw_image_rasters(bool* bool_raster_p)
{
  ++number_listed_rasters;    
  if(height > largest_dimension) largest_dimension = height;      //stores largest dimension size (used for normalized resizing)//
  if(width > largest_dimension) largest_dimension = width;
  
  //-----------------------------------------------Supplementary Diagnostics-----------------------------------------------//
  area_sum = area + area_sum;                         //calculate sum and mean
  width_sum = width + width_sum;                      //
  height_sum = height + height_sum;                   //
  average_area = area_sum/number_listed_rasters;      //
  average_width = width_sum/number_listed_rasters;    //
  average_height = height_sum/number_listed_rasters;  //
  
  if(area < smallest_area) smallest_area = area;          //determines upper and lower ranges
  if(width < smallest_width) smallest_width = width;      //
  if(height < smallest_height) smallest_height = height;  //
  if(area > largest_area) largest_area = area;            //
  if(width > largest_width) largest_width = width;        //
  if(height > largest_height) largest_height = height;    //
  
  area_variance = (qPow((area-average_area),2)+(area_variance*(number_listed_rasters-1)))/number_listed_rasters;          //calculate rolling variance
  width_variance = (qPow((width-average_width),2)+(width_variance*(number_listed_rasters-1)))/number_listed_rasters;      //
  height_variance = (qPow((height-average_height),2)+(height_variance*(number_listed_rasters-1)))/number_listed_rasters;  //
  area_std_dev = qSqrt(area_variance);                                                                                    //
  width_std_dev = qSqrt(width_variance);                                                                                  //
  height_std_dev = qSqrt(height_variance);                                                                                //
  
  QString colour_white_bold = QString("\033[1;37m");  //set font colour to bold white
  QString colour_red_bold = QString("\033[1;31m");    //set font colour to bold red
  QString colour_blue_bold = QString("\033[1;34m");   //set font colour to bold blue
  QString colour_reset = QString("\033[0m");          //reset colour and font settings
  QString area_string = QString::number(area);
  QString width_string = QString::number(width);
  QString height_string = QString::number(height);
  
  if(area < average_area*0.9) area_string = colour_blue_bold + area_string + colour_reset;              //library_seed_diagnostics visual table colour queues
  else if (area > average_area*1.1) area_string = colour_red_bold + area_string + colour_reset;         //
  else area_string = colour_white_bold + area_string + colour_reset;                                    //
  if(width < average_width*0.9) width_string = colour_blue_bold + width_string + colour_reset;          //
  else if(width > average_width*1.1) width_string = colour_red_bold + width_string + colour_reset;      //
  else width_string = colour_white_bold + width_string + colour_reset;                                  //
  if(height < average_height*0.9) height_string = colour_blue_bold + height_string + colour_reset;      //
  else if(height > average_height*1.1) height_string = colour_red_bold + height_string + colour_reset;  //
  else height_string = colour_white_bold + height_string + colour_reset;                                //
  
  cout<<endl<<"Seed library ("<<number_listed_rasters<<"/"<<calibration_seed_number<<")"<<endl;
  cout<<"\033[1;4mLibrary Seed Diagnostics:\033[0m"<<endl<<endl;
  cout<<setw(20)<<"Current:"<<setw(10)<<"Delta:"<<setw(11)<<"σ:"
      <<setw(15)<<"Average:"<<setw(10)<<"Std.Dev.:"<<setw(15)<<"Min.:"<<setw(10)<<"Max.:"<<setw(10)<<"Range:"<<endl;
  cout<<setw(20)<<"========"<<setw(10)<<"========"<<setw(10)<<"========"<<setw(15)<<"========"<<setw(10)
      <<setw(10)<<"========"<<setw(15)<<"========"<<setw(10)<<"========"<<setw(10)<<"========"<<endl;
  cout<<setw(10)<<"area:"<<setw(20)<<area_string.toLatin1().constData()<<setw(10)<<area-average_area<<setw(11)<<(area-average_area)/area_std_dev
      <<setw(15)<<average_area<<setw(10)<<area_std_dev<<setw(15)<<smallest_area<<setw(10)<<largest_area<<setw(10)<<largest_area-smallest_area<<endl;
  cout<<setw(10)<<"width:"<<setw(20)<<width_string.toLatin1().constData()<<setw(10)<<width-average_width<<setw(11)<<(width-average_width)/width_std_dev
      <<setw(15)<<average_width<<setw(10)<<width_std_dev<<setw(15)<<smallest_width<<setw(10)<<largest_width<<setw(10)<<largest_width-smallest_width<<endl;
  cout<<setw(10)<<"height:"<<setw(20)<<height_string.toLatin1().constData()<<setw(10)<<height-average_height<<setw(11)<<(height-average_height)/height_std_dev
      <<setw(15)<<average_height<<setw(10)<<height_std_dev<<setw(15)<<smallest_height<<setw(10)<<largest_height<<setw(10)<<largest_height-smallest_height<<endl;
  cout<<"-------------------------------------------------------------------------------------------------------"<<endl;
  cout<<setw(90)<<"Total Datapoints:"<<setw(10)<<raw_image_list.size()<<endl;
  cout<<setw(90)<<"Largest Dimension:"<<setw(10)<<largest_dimension<<endl;
  cout<<setw(90)<<"Total Library Seeds:"<<setw(10)<<number_listed_rasters<<endl;
  cout<<"-------------------------------------------------------------------------------------------------------"<<endl;
  //------------------------------------------------------------------------------------------------------------------------//  
  
  for(int y=0; y<height; ++y)
  {
    for(int x=0; x<width; ++x) if(bool_raster_p[x+width*y]) raw_image_list.append(x+width*y);   //ORIGINAL~~~//
  }
  
  raw_image_list.append(0);											                  //define endpoint of each raster
  raw_image_zeroes_list.append(raw_image_list.lastIndexOf(0));		//saves zero position to define start+end of segments
  raw_image_zeroes_list.append(width);
  raw_image_zeroes_list.append(height);
  raw_image_size_list.append(area);
    
  if(number_listed_rasters == calibration_seed_number)
  {
    /*cout<<endl<<"raw_image_zeroes_list: "<<endl;              //TEST~~~
    for(int jkl=0; jkl<raw_image_zeroes_list.size(); ++jkl)     //
    {                                                           //
      cout<<raw_image_zeroes_list[jkl]<<", ";                   //
    }                                                           //
    
    cout<<endl<<"raw_image_size_list: "<<endl;                  //
    for(int jkl=0; jkl<raw_image_size_list.size(); ++jkl)       //
    {                                                           //
      cout<<raw_image_zeroes_list[jkl]<<", ";                   //
    }                                                           //*/
    
    //cout<<endl<<"largest_dimension: "<<largest_dimension<<endl;   //TEST~~~//
    //calculate_threshold_values();                                 //TEST~~~//
    store_rotated_rasters_2();                                      //TEST~~~//12_21_2018//
  }
}

void secondary_processor::store_rotated_rasters_2()
{  
  int y_scaling = 0;
  int zero_pos;
  int rotated_raster_index = 0;		  //remapped index for rasters after shifts and rotation  

  QList<int> index_segment;			    //temporary list that contains a single raster
  
  int line_start;					          //define start + end positions of lines for gap fill functions
  int line_end;	
  int prev_start = 0;				        //gap fill - variables for extending shorter lines
  int prev_end = 0;
  int prev2_start = 0;
  int prev2_end = 0;

  bool use_scaling_factor;			    //separate between 0,90,180,270 from 45,135,225,315
  int segment_start = 0;
  int segment_end = 0;
  
  x_scale_factor = 1.0*average_width/average_height;      //raster vertical to horizontal ratio//
  //x_scale_factor = 1;
  y_scale_factor = 1.0;
  
  float larger_dimension;                                   //TEST~~~ 01_10_2019//
  //int larger_dimension;                                       //TEST~~~ 01_10_2019//

//====================Retrieving Raw Raster Image Index Values===================//  
  QSet<int> raw_image_seed;		                              //store and retrieve raw image data
  
  for(int q=0; q<raw_image_zeroes_list.size(); q+=3)        //load rotated raster images into list
  {
    if(q>0) segment_start = raw_image_zeroes_list[q-3]+1;	  //retrieve start + end
    else segment_start = 0;
    segment_end = raw_image_zeroes_list[q];
    
    width = raw_image_zeroes_list[q+1];						          //retrieve width + height
    height = raw_image_zeroes_list[q+2];
    
    if(1.0*width/x_scale_factor > 1.0*height/y_scale_factor) larger_dimension = 1.0*width/x_scale_factor + 1;   //TEST~~~ 01_10_2019//
    else larger_dimension = 1.0*height/y_scale_factor + 1;                                                      //TEST~~~ 01_10_2019//
    
    half_width = width/2;						   //determine center position of raster in bitmap container
    half_height = height/2;
    start_raster_x = -half_width;		   //define boundaries of raster image in enlarged raster
    start_raster_y = -half_height;
    end_raster_x = half_width;
    end_raster_y = half_height;
//-------------------------------------------------------------------------------------------------------------//
	  for(int j=segment_start; j<segment_end; ++j)
	  {
	    raw_image_seed.insert(raw_image_list[j]);
	  }
    
    for (int z=0; z<8; ++z)	  	//apply rotation to seed raster formations
    {
      theta = pi/4;
      theta = z*theta;
      if(z == 0 || z == 2 || z == 4 || z == 6) use_scaling_factor = 1;	//distinction between 0,90,180,270 and 45,135,225,315
      else use_scaling_factor = 0;

      if(use_scaling_factor != 1)	  //form rasters for angles: 45, 135, 225, 315
      {
        for (double y=start_raster_y; y<end_raster_y; ++y)	
        {
          for (double x=start_raster_x; x<end_raster_x; ++x)
          {  
            raster_position_in_container = (x-start_raster_x)+width*(y-start_raster_y);
            if(raw_image_seed.contains(raster_position_in_container))
            {
              //shifted_x = x;  //no compression//
              shifted_y = y;    //
              shifted_x = 1.0*x/x_scale_factor;   //apply horizontal compression//
              //shifted_y = 1.0*y/x_scale_factor; //no applications for vertical compression//
              
	            rotated_x = (shifted_x*qCos(theta)-shifted_y*qSin(theta));
	            rotated_y = (shifted_x*qSin(theta)+shifted_y*qCos(theta));
              unshifted_x = rotated_x + larger_dimension/2;       //TEST~~~//
              unshifted_y = rotated_y + larger_dimension/2;       //TEST~~~//
              rotated_raster_index = unshifted_x+width*unshifted_y;              
              index_segment.append(rotated_raster_index);              
            }
          }
        }
      }
      else 							          //form rasters for angles: 0, 90, 180, 270
      {
  	    for (double y=start_raster_y; y<end_raster_y; ++y)	
        {
          for (double x=start_raster_x; x<end_raster_x; ++x)
          {  
            raster_position_in_container = (x-start_raster_x)+width*(y-start_raster_y);
            if(raw_image_seed.contains(raster_position_in_container))
            {  
              //shifted_x = x;  //unprocessed from camera//
	            shifted_y = y;    //
              shifted_x = 1.0*x/x_scale_factor;     //horizontal compression//
              //shifted_y = 1.0*y/x_scale_factor;   //
                            
	            rotated_x = (shifted_x*qCos(theta)-shifted_y*qSin(theta));
	            rotated_y = (shifted_x*qSin(theta)+shifted_y*qCos(theta));
              unshifted_x = rotated_x + larger_dimension/2;       //TEST~~~//
              unshifted_y = rotated_y + larger_dimension/2;       //TEST~~~//
              rotated_raster_index = unshifted_x+width*unshifted_y;
              index_segment.append(rotated_raster_index);              
            }
          }
        }
      }
      //-------------------------------------------------------------------------------------------------------------------------//
      QSet<int> segment_set = index_segment.toSet();
      index_segment.clear();
      //-------------------------------------------------------------------------------------------------------------------------//
      prev_start = width;		//Gap Fill - variables for extending shorter lines
      prev_end = 0;
      prev2_start = width;
      prev2_end = 0;
      
      //Gap Fill - columns
      {
        for(int y=0; y<width; ++y)
        {
          line_start = width;
          line_end = 0;
          for(int x=-width/2; x<width; ++x)	  //first pass: find start + end positions of column
          {
            if(segment_set.contains(y+width*x))
            {  
              line_end = x;
              if(line_end<line_start) line_start = x;
            }
          }      
          for(int x=-width/2; x<width; ++x)	  //second pass: fill gaps that exist between endpoints of line
          {
            if((!segment_set.contains(y+width*x)) && x>line_start && x<line_end) segment_set.insert(y+width*x); //TEST~~
          }
        }
      }
      //-------------------------------------------------------------------------------------------------------------------------//
      
      if(z==0) average_calibration_seed_size += segment_set.size();	//TEST~~~for calculating diagnostics in raster_comparison function
      index_segment = segment_set.toList();
      segment_set.clear();
      qSort(index_segment.begin(), index_segment.end());
      index_segment.append(0);
		
      for(int i=0; i<index_segment.size(); ++i)
      {
        index_list.append(index_segment[i]); 
      }
      zero_pos = index_list.lastIndexOf(0);
      index_zero_positions_list.append(zero_pos);
          
      //-------------------------------------------------------------------------------------------------------------------------//   //TEST~~~// index value subtraction
      //suppose only need a vertical shift//
      
      for (int q=larger_dimension; q>=0; --q)   //TEST~~~//
      {
        for (int r=0; r<larger_dimension; ++r)
        {
          if(index_segment.contains(r + width*q))
          {
            rotated_image_list.append(r+width*q);    //TEST~~~ 01_16_2019b
          }
        }
      }
      
    //-------------------------------------------------------------------------------------------------------------------------//
      rotated_image_list.append(0);
      rotated_image_zeroes_list.append(rotated_image_list.lastIndexOf(0));
      
      rotated_image_dimensions.append(index_segment.size());
      rotated_image_dimensions.append(1.0*width/x_scale_factor);
      rotated_image_dimensions.append(1.0*height/y_scale_factor);
      //rotated_image_dimensions.append(larger_dimension);  //TEST~~~//
    //==================================================================================================================// //Centroid calculations for visualization
      
      //calculate centroid instead of using width and height midpoints - subject to rounding errors//
      /*//int rotated_segment_centroid_x = 0;   //original~~~//
      //int rotated_segment_centroid_y = 0;   //original~~~//
      float rotated_segment_centroid_x = 0;   //rounding error//
      float rotated_segment_centroid_y = 0;   //rounding error//
    
      int rotated_segment_size;
    
      rotated_segment_size = index_segment.size();
    
      for(int q=larger_dimension; q>=0; --q)
      {
        for(int r=0; r<larger_dimension; ++r)
        {
          if(index_segment.contains(r+width*q))
          {
            rotated_segment_centroid_x = rotated_segment_centroid_x + r;
            rotated_segment_centroid_y = rotated_segment_centroid_y + q;
          }
        }
      }
      cout<<"centroid coordinates: ("<<rotated_segment_centroid_x/rotated_segment_size<<","<<rotated_segment_centroid_y/rotated_segment_size<<")"<<endl;  //OMIT~~~//
      //rotated_segment_centroid_x = round(rotated_segment_centroid_x/rotated_segment_size);  //TEST~~~
      //rotated_segment_centroid_y = round(rotated_segment_centroid_y/rotated_segment_size);  //TEST~~~
      rotated_segment_centroid_x = rotated_segment_centroid_x/rotated_segment_size;   //precision limited//
      rotated_segment_centroid_y = rotated_segment_centroid_y/rotated_segment_size;   //precision limited//
      cout<<"rounded centroid coordinates: ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;                                    //OMIT~~~//*/
    
    //==================================================================================================================// 
      
      //visualization - printout to terminal
      /*int raster_area_count = 0;                    //TEST~~~//
      for (int q=larger_dimension + 2; q>=0; --q)   //TEST~~~//
      {
        for (int r=0; r<larger_dimension + 2; ++r)
        {
          //if((r == int(larger_dimension/2)) && q == height/2) cout<<"M ";
          if((q == rotated_segment_centroid_y) && (r == rotated_segment_centroid_x)) cout<<"X ";
          else if((r+width*q) == 0) cout<<"x ";
          else if(index_segment.contains(r + width*q))
          {
            cout<<"o ";
            ++raster_area_count;
          }
          else cout<<". ";
        }
        cout<<endl;
      }
      cout<<endl;
    
      cout<<"From working printout~"<<endl;                 //OMIT~~~//
      cout<<"larger_dimension: "<<larger_dimension<<endl;   //OMIT~~~//
      cout<<"width: "<<width<<endl;                         //OMIT~~~//
      cout<<"x_scale_factor: "<<x_scale_factor<<endl;       //OMIT~~~//
      
      rotated_image_centroids.append(rotated_segment_centroid_x);
      rotated_image_centroids.append(rotated_segment_centroid_y);*/

    //==================================================================================================================//  //visualization printout//
      index_segment.clear();
    }
    raw_image_seed.clear();
  }
  average_calibration_seed_size = average_calibration_seed_size/calibration_seed_number;	    //TEST~~~
  //cout<<"avg calibration seed size: "<<average_calibration_seed_size<<endl;                 //OMIT~~~//
  //remove_similar_rasters();		//compare calibration data library and removes similar rasters
  //remove_similar_rasters_2();   //TEST~~~ implementation of rotated raster images
  remove_similar_rasters_3();   //TEST~~~ 01_21_2019
}
//==========================================================================================================================================================//
void secondary_processor::remove_similar_rasters()	//compare and remove similar seeds in Qlist, link top 5 most similiar rasters to remaining
{
  cout<<"REMOVE SIMILAR"<<endl; //OMIT~~~//
  //==================================================================================================//  //TEST~~~// retrieving rotated raster data from new QLists
  //reprint rotated raster image with centroid// test to overlap rasters for similar comparison//
  
  for(int jkl=0; jkl<rotated_image_zeroes_list.size(); ++jkl)
  {
    int test_start;
    int test_end;
    int test_width;
    int test_height;
    QSet<int> test_segment_print;
    int larger_dimension;
    int raster_selected;  //floor rounded int for selecting from raw_image_zeroes_list//
  
    if(jkl != 0) test_start = rotated_image_zeroes_list.at(jkl-1);
    else test_start = 0;
    test_end = rotated_image_zeroes_list.at(jkl);
    
    raster_selected = jkl/8;
    cout<<"Raster_selected: "<<raster_selected<<endl;         //OMIT~~~//
    raster_selected = 3*raster_selected;
    cout<<"Raster_selected: "<<raster_selected<<endl;         //OMIT~~~//
    test_width = raw_image_zeroes_list.at(raster_selected+1);              //unprocessed original dimensions
    test_height = raw_image_zeroes_list.at(raster_selected+2);
    cout<<endl<<"TEST_WIDTH: "<<test_width<<"\traster selected: "<<raster_selected+1<<endl;     //OMIT~~~
    cout<<endl<<"TEST_HEIGHT: "<<test_height<<"\traster_selected: "<<raster_selected+2<<endl;   //OMIT~~~
    
    if(rotated_image_dimensions.at(jkl*3+1) > rotated_image_dimensions.at(jkl*3+2)) larger_dimension = rotated_image_dimensions.at(jkl*3+1);  //TEST~~~//
    else larger_dimension = rotated_image_dimensions.at(jkl*3+2);                                                                             //TEST~~~//
    
    for(int x=test_start+1; x<test_end-1; ++x)                  //retrieve and store
    {
      test_segment_print.insert(rotated_image_list.at(x));
    }
    
    /*for(int x=larger_dimension+2; x>=0; --x)      //print test segment//
    {
      for(int y=0; y<larger_dimension+2; ++y)
      {
        if(test_segment_print.contains(x+test_width*y)) cout<<"o ";
        else cout<<". ";
      }
      cout<<endl;
    }*/
  
    //-------------------------------------------------------------------// //TEST~~~//
    //centroid calculation//
    //int rotated_segment_centroid_x = 0;
    //int rotated_segment_centroid_y = 0;
    float rotated_segment_centroid_x = 0;
    float rotated_segment_centroid_y = 0;
    int rotated_segment_size;
    
    rotated_segment_size = test_segment_print.size();
    
    for(int q=larger_dimension+2; q>=0; --q)
    {
      for(int r=0; r<larger_dimension+2; ++r)
      {
        if(test_segment_print.contains(r+test_width*q))
        {
          rotated_segment_centroid_x = rotated_segment_centroid_x + r;
          rotated_segment_centroid_y = rotated_segment_centroid_y + q;
        }
      }
    }
    
    cout<<"centroid sum: ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;    //OMIT~~~//
    cout<<"centroid coordinates (new): ("<<float(rotated_segment_centroid_x/rotated_segment_size)       //OMIT~~~//
        <<","<<float(rotated_segment_centroid_y/rotated_segment_size)<<")"<<endl;                       //OMIT~~~//
    cout<<"centroid coordinates (rounded): ("<<round(rotated_segment_centroid_x/rotated_segment_size)   //OMIT~~~//
    <<","<<round(rotated_segment_centroid_y/rotated_segment_size)<<")"<<endl;                           //OMIT~~~//
        
    //cout<<"index_segment_size: "<<rotated_segment_size<<endl;     //rounding error//
    
    //rotated_segment_centroid_x = rotated_segment_centroid_x/rotated_segment_size; //original~~~
    //rotated_segment_centroid_y = rotated_segment_centroid_y/rotated_segment_size; //original~~~
    rotated_segment_centroid_x = round(rotated_segment_centroid_x/rotated_segment_size); //TEST~~~//
    rotated_segment_centroid_y = round(rotated_segment_centroid_y/rotated_segment_size); //TEST~~~//
    cout<<"centroid coordinates (old): ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;
    
    for(int x=larger_dimension+2; x>=0; --x)      //print test segment//
    {
      for(int y=0; y<larger_dimension+2; ++y)
      {
        if((x == rotated_segment_centroid_x) && (y == rotated_segment_centroid_y)) cout<<"X ";
        else if(test_segment_print.contains(x+test_width*y)) cout<<"o ";
        else cout<<". ";
      }
      cout<<endl;
    }
    
    cout<<"re-centred raster about origin"<<endl;
    //cout<<"centroid: ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;
    for(int x=larger_dimension/2+1; x>=-larger_dimension/2-1; --x)
    {
      for(int y=-larger_dimension/2-1; y<larger_dimension/2+1; ++y)
      {
        if(x+test_width*y == 0) cout<<"M ";
        else if(test_segment_print.contains((x+rotated_segment_centroid_x) + test_width*(y+rotated_segment_centroid_y))) cout<<"o ";    //relative to centroid//
        else cout<<". ";
      }
      cout<<endl;
    }
    
    //-------------------------------------------------------------------// //print out raster index values//
    /*cout<<endl<<"test_segment: "<<endl;
    
    QList<int> test_segment_print_list = test_segment_print.toList();
    qSort(test_segment_print_list);
    
    for(int asd=0; asd<test_segment_print_list.size(); ++asd)
    {
      cout<<test_segment_print_list.at(asd)<<" ";
    }
    cout<<endl<<endl;*/
    //-------------------------------------------------------------------//
  
    //DIAGNOSTICS//
    cout<<"w: "<<test_width<<"\tH:" <<test_height<<endl;      //OMIT~~~
    cout<<"segment_size: "<<test_segment_print.size()<<endl;  //OMIT~~~
    cout<<"raw_dimensions: \nW: "<<rotated_image_dimensions.at(jkl*3+1)<<"\tH: "<<rotated_image_dimensions.at(jkl*3+2)<<endl;
    cout<<"larger_dimensions: "<<larger_dimension<<endl;      //OMIT~~~
    //DIAGNOSTICS//    
    
    test_segment_print.clear();
  
    //int loop_counter;
    //cin>>loop_counter;
  }
  //==================================================================================================//
  
  int segment_start = 0;
  int segment_end = 0;
  QList<int> index_segment_1;		//temporary list that contains a single raster
  QList<int> index_segment_2;

  int seed_area_1;
  int seed_area_2;
  int score = 0;        //denotes number of differences between two rasters
  int i = 0;            //increment segment 1 (raster 1)
  int j = 0;            //increment segment 2 (raster 2)
  
  float area_percent;             //Area of overlap over the larger raster area
  float larger_area;              //Stores the size of the larger raster
  int link_num = 5;               //number of similar rasters to link
  float score_array[link_num];    //temporary container to carry most similar raster links
  int num_array[link_num];        //
  
  display_rotated_raster_p = new char[rmax_x*rmax_y];   //TEST~~ used to visualize raster comparisons
  
//--------------------Link similar rasters--------------------------//
  QList<int> segment_num;
  QList<float> segment_similarity;
//-------------------redesigning similar comparison-----------------//
  int rasters_in_list;  
  int raster_position = 0;
//------------------------load segment 1----------------------------//
  //for(int b=0; b<index_zero_positions_list.size()-1; ++b)   //ORIGINAL~~~//
  for(int b=0; b<rotated_image_zeroes_list.size()-1; ++b)     //TEST~~~//
  {
    for(i=0; i<link_num; ++i) score_array[i] = 1;   //score_array
    full_score_list.append(b);                                   
    //segment_end = index_zero_positions_list[b];  //ORIGINAL~~~//
    segment_end = rotated_image_zeroes_list[b];    //TEST~~~//
    
    for(int i = segment_start; i<segment_end; ++i)
    {
      //index_segment_1.append(index_list[i]);          //ORIGINAL~~~//
      index_segment_1.append(rotated_image_list[i]);    //TEST~~~//
    }
    seed_area_1 = index_segment_1.size(); 
//------------------------load segment 2----------------------------// 
    //for(int a=b+1; a<index_zero_positions_list.size(); ++a)   //ORIGINAL~~~//
    for(int a=b+1; a<rotated_image_zeroes_list.size(); ++a)     //TEST~~~//
    {
      //segment_start = index_zero_positions_list[a]+1;   //ORIGINAL~~~//
      //segment_end = index_zero_positions_list[a+1];     //
      segment_start = rotated_image_zeroes_list[a]+1;     //TEST~~~//
      segment_end = rotated_image_zeroes_list[a+1];       //TEST~~~//
  
      for(j=segment_start; j<segment_end; ++j)
      {
        //index_segment_2.append(index_list[j]);          //ORIGINAL~~~//
        index_segment_2.append(rotated_image_list[j]);    //TEST~~~//
      }
      seed_area_2 = index_segment_2.size();
      
//========================================================================================================================================//
//--------------------------------------------------remapping images before calculations--------------------------------------------------//
      //index_segment_1, index_segment_2  (QLists)
      //seed_area_1, seed_area_2  (list size)
      //rotated_image_dimensions
      //index_segment_1;
      //b - raster 1 selected, a - raster 2 selected
      
      int seed_width_1, seed_width_2, larger_width;
      int seed_height_1, seed_height_2, larger_height;
      
      seed_width_1 = rotated_image_dimensions.at(b*3 + 1);
      seed_height_1 = rotated_image_dimensions.at(b*3 + 2);
      seed_width_2 = rotated_image_dimensions.at(a*3 + 1);
      seed_height_2 = rotated_image_dimensions.at(a*3 + 2);
      
      if(seed_width_1 > seed_width_2) larger_width = seed_width_1;
      else larger_width = seed_width_2;
      if(seed_height_1 > seed_height_2) larger_height = seed_height_1;
      else larger_height = seed_height_2;
      
//========================================================================================================================================//      
//---------------------calculate scores method----------------------//
      int i = 0;
      int j = 0;
      
      while((i<seed_area_1) || (j<seed_area_2))
      {
        if(index_segment_1.at(i) == index_segment_2.at(j))      //no condition for when seeds end on same index value (stuck on encounter)
        {
          if(i<seed_area_1) ++i;
          if(j<seed_area_2) ++j;
        }
        else if(index_segment_1.at(i) > index_segment_2.at(j))
        {
          ++score;
          if(j<seed_area_2) ++j;
          else ++i;
        }
        else
        {
          ++score;
          if(i<seed_area_1) ++i;
          else ++j;
        }
      }
      i = 0;
      j = 0;    
//-------------------------Store Scores and Similar Raster Images------------------------//
      if(seed_area_1 > seed_area_2) larger_area = seed_area_1;      //determines larger seed area of the two and calculates % of score/area
      else larger_area = seed_area_2;
      area_percent = score/larger_area;  
      full_score_list.append(area_percent);
      //cout<<"AREA %: "<<area_percent<<"\tSCORE: "<<score<<endl;   //OMIT~~~//
      //cout<<"S1: "<<seed_area_1<<"\tS2: "<<seed_area_2<<endl;     //OMIT~~~//
      score = 0;
      
      if((area_percent < similar_threshold) && !similar_raster_list.contains(a))  //threshold defined in header
      {
        similar_raster_list.append(a);
      }
      index_segment_2.clear();
    } //segment 2 for loop
    index_segment_1.clear();
    segment_start = index_zero_positions_list[b]+1;     //iterates segment 1 loop
  } //segment 1 for loop

//--------------------------Store Best Scores in Array---------------------------//
  rasters_in_list = index_zero_positions_list.size();
  raster_position = 0;
  
  for(int g=0; g<rasters_in_list; ++g)
  {  
    while(similar_raster_list.contains(g)) ++g; 
    raster_position = g;
    segment_start = (rasters_in_list*(rasters_in_list+1))/2 - (rasters_in_list-raster_position)*((rasters_in_list-raster_position)+1)/2;
    segment_end = (rasters_in_list*(rasters_in_list+1))/2 - (rasters_in_list-raster_position-1)*((rasters_in_list-raster_position))/2;
    for(int i=0; i<link_num; ++i) score_array[i] = 1;   	//score_array  
          
    for(int h=segment_start+1; h<segment_end; ++h)
    {
      while(similar_raster_list.contains(h)) ++h;
      while(full_score_list.at(h)<similar_threshold) ++h;				//TEST~~~
      area_percent = full_score_list.at(h);
      
      if(area_percent < score_array[0])
      {
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
  	    num_array[3] = num_array[2];        
        score_array[2] = score_array[1];
  	    num_array[2] = num_array[1];
        score_array[1] = score_array[0];
        num_array[1] = num_array[0];
        score_array[0] = area_percent;
        num_array[0] = h-segment_start+g;
      }    
      else if(area_percent < score_array[1])
      {
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
  	    num_array[3] = num_array[2];        
        score_array[2] = score_array[1];
  	    num_array[2] = num_array[1];
        score_array[1] = area_percent;
        num_array[1] = h-segment_start+g;
      }
      else if(area_percent < score_array[2])     
      {        
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
        num_array[3] = num_array[2];
        score_array[2] = area_percent;
        num_array[2] = h-segment_start+g;
      }
      else if(area_percent < score_array[3])
      {
        score_array[4] = score_array[3];
        num_array[4] = num_array[3];
        score_array[3] = area_percent;
        num_array[3] = h-segment_start+g;
      }
      else if(area_percent < score_array[4])
      {  
        score_array[4] = area_percent;
        num_array[4] = h-segment_start+g;
      }  
    }
    segment_num.append(g);		    
    segment_similarity.append(g);
    for(int q=0; q<link_num; ++q)
    {
      segment_num.append(num_array[q]);
      segment_similarity.append(score_array[q]);
    }
  }
//---------------------------------reverse comparison--------------------------------//
  for(int i = segment_num.size()-(link_num+1); i>=0; i-=(link_num+1))
  {
    for(int j=0; j<i; j+=(link_num+1))
    {
      for(int n=1; n<(link_num+1); ++n)
      {
        if(segment_num.at(j+n)==segment_num.at(i))
        {
          if(segment_similarity.at(j+n) < segment_similarity.at(i+1))         //Replace if statements with for loops later  (or check remove and pushback functions)
          {      
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(i+3)));
            segment_num.replace((i+3),(segment_num.at(i+2)));
            segment_num.replace((i+2),(segment_num.at(i+1)));
            segment_num.replace((i+1),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(i+3)));
            segment_similarity.replace((i+3),(segment_similarity.at(i+2)));
            segment_similarity.replace((i+2),(segment_similarity.at(i+1)));
            segment_similarity.replace((i+1),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+2))
          {
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(i+3)));
            segment_num.replace((i+3),(segment_num.at(i+2)));
            segment_num.replace((i+2),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(i+3)));
            segment_similarity.replace((i+3),(segment_similarity.at(i+2)));
            segment_similarity.replace((i+2),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+3))
          {
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(i+3)));
            segment_num.replace((i+3),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(i+3)));
            segment_similarity.replace((i+3),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+4))
          {
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+5))
          {
            segment_num.replace((i+5),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(j+n)));
          }
        }
      }
    }
  }
//--------------------------------Remove Similar Rasters from List----------------------------//
  qSort(similar_raster_list.begin(), similar_raster_list.end());
  for(int h=similar_raster_list.size()-1; h>=0; --h)	//similar_raster_list
  {
    segment_start = index_zero_positions_list[(similar_raster_list[h])-1]+1;
    segment_end = index_zero_positions_list[(similar_raster_list[h])];
    for(int f=segment_end; f>segment_start; --f)
    {
      index_list.removeAt(f);	  //removes similar_raster zero position index from list
    }
  }
  int previous_zero_position = 0;
  index_zero_positions_list.clear();
  for(int q=0; q<index_list.size(); ++q)
  {
    previous_zero_position = index_list.indexOf(0,q-1);
    if(index_list.indexOf(0,q) > previous_zero_position)
    {
      index_zero_positions_list.append(index_list.lastIndexOf(0,q));
    }
  }
//--------------------------Diagnostics-------------------------//
  cout<<endl<<"Removing: "<<similar_raster_list.size()<<"/"<<calibration_seed_number*8<<endl;     //OMIT~~~//
  cout<<"Similarity Threshold: "<<100-100*similar_threshold<<"%"<<endl;                            //OMIT~~~//
//--------------------------------------------------------------//
  /*cout<<endl<<"segment_num: "<<endl;            
  for(int g=0; g<segment_num.size(); ++g)
  {
    cout<<segment_num[g]<<" ";
  }
  cout<<endl<<"segment_similarity: "<<endl;
  for(int g=0; g<segment_similarity.size(); ++g)
  {
    cout<<segment_similarity[g]<<" ";
  }*/
  cout<<endl<<"similar_raster_list: "<<"("<<similar_raster_list.size()<<" @ "<<similar_threshold<<")"<<endl;
  for(int g=0; g<similar_raster_list.size(); ++g)
  {
    cout<<similar_raster_list[g]<<" ";
  }
  cout<<endl<<"zero_positions_list: ("<<index_zero_positions_list.size()<<")"<<endl;
  for(int g=0; g<index_zero_positions_list.size(); ++g)
  {
    cout<<index_zero_positions_list[g]<<" ";
  }
//---------------------------------------------------------------//
  cout<<endl<<"----------------------------Calibration Data Ready-----------------------------"<<endl;
}
//==================================================================================================================================================//
void secondary_processor::remove_similar_rasters_3()	//compare and remove similar seeds in Qlist, link top 5 most similiar rasters to remaining
{
  cout<<"REMOVE SIMILAR 3"<<endl; //OMIT~~~//
  //==================================================================================================//  //TEST~~~// retrieving rotated raster data from new QLists
  //reprint rotated raster image with centroid// test to overlap rasters for similar comparison//
  QSet<int> index_segment_1;
  QSet<int> index_segment_2;
  
  int seed_width_1, seed_width_2, larger_width;
  int seed_height_1, seed_height_2, larger_height;
  int larger_dimension_1, larger_dimension_2, larger_dimension;
  
  int seed_start_1, seed_start_2;
  int seed_end_1, seed_end_2;
  int raster_selected_1, raster_selected_2;
  
  int segment_start_1, segment_start_2;
  int segment_end_1, segment_end_2;
  
  float rotated_seg_1_centroid_x, rotated_seg_2_centroid_x;   //for centroid calculations//
  float rotated_seg_1_centroid_y, rotated_seg_2_centroid_y;   //
  int rotated_seg_1_size, rotated_seg_2_size;                 //
  //-----------------------------------------------------------------------------------------//
  int larger_rotated_seg;
  
  int score = 0;
  
  float area_percent;
  int link_num = 5;   //might make more sense to store in header//
  float score_array[link_num];
  int num_array[link_num];

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Outer Loop~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //implement imbedded for loops like original remove_similar_rasters//
  for(int b=0; b<rotated_image_zeroes_list.size()-1; ++b)
  {
    raster_selected_1 = b/8;                                          //retrieve raster variables//
    raster_selected_1 = 3*raster_selected_1;                          //
    seed_width_1 = raw_image_zeroes_list.at(raster_selected_1 + 1);   //
    seed_height_1 = raw_image_zeroes_list.at(raster_selected_1 + 2);  //
    
    if(b != 0) segment_start_1 = rotated_image_zeroes_list[b-1]+1;  //define start + end of rasters
    else segment_start_1 = 0;                                       //
    segment_end_1 = rotated_image_zeroes_list[b]-1;                 //
    
    if(rotated_image_dimensions.at(b*3+1) > rotated_image_dimensions.at(b*3+2)) //determine printout container dimensions
    {                                                                           //
      larger_dimension_1 = rotated_image_dimensions.at(b*3+1);                  //
    }                                                                           //
    else larger_dimension_1 = rotated_image_dimensions.at(b*3+2);               //
    
    for(int i=segment_start_1; i<segment_end_1; ++i)    //retrieve raster into QSet
    {                                                   //
      index_segment_1.insert(rotated_image_list.at(i)); //
    }                                                   //
    
    rotated_seg_1_size = index_segment_1.size();                                    //centroid calculations
    for(int q=larger_dimension_1+2; q>=0; --q)                                      //
    {                                                                               //
      for(int r=0; r<larger_dimension_1+2; ++r)                                     //
      {                                                                             //
        if(index_segment_1.contains(r+seed_width_1*q))                              //
        {                                                                           //
          rotated_seg_1_centroid_x = rotated_seg_1_centroid_x + r;                  //
          rotated_seg_1_centroid_y = rotated_seg_1_centroid_y + q;                  //
        }                                                                           //
      }                                                                             //
    }                                                                               //
    rotated_seg_1_centroid_x = round(rotated_seg_1_centroid_x/rotated_seg_1_size);  //
    rotated_seg_1_centroid_y = round(rotated_seg_1_centroid_y/rotated_seg_1_size);  //
    
    //function for shifting raster image centroid to 0,0//
    QSet<int> seg_1_temp_set;                               //shift raster image centroid to 0.0//
    seg_1_temp_set.clear();                                 //
    for(int x=larger_dimension_1+2; x>=0; --x)              //
    {                                                       //
      for(int y=0; y<larger_dimension_1+2; ++y)             //
      {                                                     //
        if(index_segment_1.contains(x+seed_width_1*y))      //
        {                                                   //
          seg_1_temp_set.insert((x+seed_width_1*y)-(rotated_seg_1_centroid_x+seed_width_1*rotated_seg_1_centroid_y));
        }                                                   //
      }                                                     //
    }                                                       //
    
    //print retrieved rotated raster with centroid position//
    /*for(int x=larger_dimension_1+2; x>=0; --x)                                              //TEST~~~ print test segment//
    {                                                                                       //
      for(int y=0; y<larger_dimension_1+2; ++y)                                             //
      {                                                                                     //
        if((x == rotated_seg_1_centroid_x) && (y == rotated_seg_1_centroid_y)) cout<<"X ";  //
        else if(index_segment_1.contains(x+seed_width_1*y)) cout<<"o ";                     //
        else cout<<". ";                                                                    //
      }                                                                                     //
      cout<<endl;                                                                           //
    }                                                                                       //*/
    
    //print shifted rotated raster image//
    /*for(int x=larger_dimension_1/2+1; x>=-larger_dimension_1/2-1; --x)    //TEST~~~ print test segment//
    {                                                                     //
      for(int y=-larger_dimension_1/2-1; y<larger_dimension_1/2+1; ++y)   //
      {                                                                   //
        if((x == 0) && (y == 0)) cout<<"X ";                              //
        else if(seg_1_temp_set.contains(x+seed_width_1*y)) cout<<"o ";    //
        else cout<<". ";                                                  //
      }                                                                   //
      cout<<endl;                                                         //
    }                                                                     //*/
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Inner Loop~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    for(int a=b+1; a<rotated_image_zeroes_list.size(); ++a)
    {
      raster_selected_2 = a/8;                                          //retrieve raster variables//
      raster_selected_2 = 3*raster_selected_2;                          //
      seed_width_2 = raw_image_zeroes_list.at(raster_selected_2 + 1);   //
      seed_height_2 = raw_image_zeroes_list.at(raster_selected_2 + 2);  //
      
      segment_start_2 = rotated_image_zeroes_list[a-1]+1; //define start + end of rasters
      segment_end_2 = rotated_image_zeroes_list[a]-1;     //
      
      if(rotated_image_dimensions.at(a*3+1) > rotated_image_dimensions.at(a*3+2)) //determine printout container dimensions
      {                                                                           //
        larger_dimension_2 = rotated_image_dimensions.at(a*3+1);                  //
      }                                                                           //
      else larger_dimension_2 = rotated_image_dimensions.at(a*3+2);               // 
      
      for(int i=segment_start_2; i<segment_end_2; ++i)    //retrieve raster into QSet
      {                                                   //
        index_segment_2.insert(rotated_image_list.at(i)); //
      }                                                   //
      
      rotated_seg_2_size = index_segment_2.size();                                    //centroid calculations
      for(int q=larger_dimension_2+2; q>=0; --q)                                      //
      {                                                                               //
        for(int r=0; r<larger_dimension_2+2; ++r)                                     //
        {                                                                             //
          if(index_segment_2.contains(r+seed_width_2*q))                              //
          {                                                                           //
            rotated_seg_2_centroid_x = rotated_seg_2_centroid_x + r;                  //
            rotated_seg_2_centroid_y = rotated_seg_2_centroid_y + q;                  //
          }                                                                           //
        }                                                                             //
      }                                                                               //
      rotated_seg_2_centroid_x = round(rotated_seg_2_centroid_x/rotated_seg_2_size);  //
      rotated_seg_2_centroid_y = round(rotated_seg_2_centroid_y/rotated_seg_2_size);  //
      
      //function for shifting raster image centroid to 0,0//
      QSet<int> seg_2_temp_set;                               //shift raster image centroid to 0.0//
      seg_2_temp_set.clear();                                 //
      for(int x=larger_dimension_2+2; x>=0; --x)              //
      {                                                       //
        for(int y=0; y<larger_dimension_2+2; ++y)             //
        {                                                     //
          if(index_segment_2.contains(x+seed_width_2*y))      //
          {                                                   //
            seg_2_temp_set.insert((x+seed_width_2*y)-(rotated_seg_2_centroid_x+seed_width_2*rotated_seg_2_centroid_y));
          }                                                   //
        }                                                     //
      }                                                       //
      
      //print retrieved rotated raster with centroid position//
      /*for(int x=larger_dimension_2+2; x>=0; --x)                                              //TEST~~~ print test segment//
      {                                                                                       //
        for(int y=0; y<larger_dimension_2+2; ++y)                                             //
        {                                                                                     //
          if((x == rotated_seg_2_centroid_x) && (y == rotated_seg_2_centroid_y)) cout<<"X ";  //
          else if(index_segment_2.contains(x+seed_width_2*y)) cout<<"o ";                     //
          else cout<<". ";                                                                    //
        }                                                                                     //
        cout<<endl;                                                                           //
      }                                                                                       //*/
      
      //print shifted rotated raster image//
      /*for(int x=larger_dimension_2/2+1; x>=-larger_dimension_2/2-1; --x)    //TEST~~~ print test segment//
      {                                                                     //
        for(int y=-larger_dimension_2/2-1; y<larger_dimension_2/2+1; ++y)   //
        {                                                                   //
          if((x == 0) && (y == 0)) cout<<"X ";                              //
          else if(seg_2_temp_set.contains(x+seed_width_2*y)) cout<<"o ";    //
          else cout<<". ";                                                  //
        }                                                                   //
        cout<<endl;                                                         //
      }                                                                     //*/
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//      
      //superimpose raster segment 1 and segment 2//
      if(larger_dimension_1 > larger_dimension_2) larger_dimension = larger_dimension_1;
      else larger_dimension = larger_dimension_2;
      
      score = 0;
      
      for(int x=larger_dimension/2+1; x>=-larger_dimension/2-1; --x)
      {
        for(int y=-larger_dimension/2-1; y<larger_dimension/2+1; ++y)
        {
          if(seg_1_temp_set.contains(x+seed_width_1*y) && seg_2_temp_set.contains(x+seed_width_2*y)) {}
          else if(seg_1_temp_set.contains(x+seed_width_1*y)) ++score;
          else if(seg_2_temp_set.contains(x+seed_width_2*y)) ++score;
        }
      }
      
    //-------------------------Store Scores and Similar Raster Images------------------------//
      if(rotated_seg_1_size > rotated_seg_2_size) larger_rotated_seg = rotated_seg_1_size;
      else larger_rotated_seg = rotated_seg_1_size;
      area_percent = 1.0*score/larger_rotated_seg;
      full_score_list.append(area_percent);
      
      //cout<<endl<<endl<<"area_percent = "<<area_percent<<endl<<"score: "<<score   //OMIT~~~//
          //<<"\t\tlarger_rotated_seg = "<<larger_rotated_seg<<endl;                //OMIT~~~//
      
      if((area_percent < similar_threshold) && !similar_raster_list.contains(a))
      {
        similar_raster_list.append(a);
      }
      
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Diagnostics~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      /*cout<<"b = "<<b<<"\ta = "<<a<<endl;                     //OMIT~~~
      cout<<"raster_selected_1 = "<<raster_selected_1<<endl   //
          <<"raster_selected_2 = "<<raster_selected_2<<endl;  //12
      
      cout<<endl<<"zeroes_list: "<<endl;                          //OMIT~~~//
      for(int asd=0; asd<rotated_image_zeroes_list.size(); ++asd) //
      {                                                           //
        cout<<rotated_image_zeroes_list.at(asd)<<" ";             //
      }                                                           //*/
      //cout<<endl<<endl;                                           //
      //cout<<"seg_start_1: "<<segment_start_1<<"\t\tseg_end_1: "<<segment_end_1<<endl  //OMIT~~~//
          //<<"seg_start_2: "<<segment_start_2<<"\t\tseg_end_2: "<<segment_end_2<<endl; //
  
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
      index_segment_2.clear();
    }
    index_segment_1.clear();
  }
  
  qSort(similar_raster_list); //unnecessary//
  cout<<endl<<endl<<"similar_raster_list ("<<similar_raster_list.size()<<")"<<endl;    //OMIT~~~
  for(int j=0; j<similar_raster_list.size(); ++j)   //OMIT~~~
  {
    cout<<similar_raster_list.at(j)<<" ";
  }
  
  int loop_counter;   //OMIT~~~//
  cin>>loop_counter;  //OMIT~~~//
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  int segment_start;
  int segment_end;
  
  int rasters_in_list;
  int raster_position = 0;

  rasters_in_list = rotated_image_zeroes_list.size();
  
  /*cout<<endl<<"index zeroes_list: "<<endl;                    //OMIT~~~//
  for(int asd=0; asd<index_zero_positions_list.size(); ++asd) //
  {                                                           //
    cout<<index_zero_positions_list.at(asd)<<" ";             //
  }                                                           //
  cout<<endl<<"rotated zeroes_list: "<<endl;                  //
  for(int asd=0; asd<rotated_image_zeroes_list.size(); ++asd) //
  {                                                           //
    cout<<rotated_image_zeroes_list.at(asd)<<" ";             //
  }                                                           //*/
  
  cout<<endl<<endl;                                                 //OMIT~~~//
  cout<<"sim.rasters: "<<similar_raster_list.size()<<endl;          //
  cout<<"ind.zero_pos.: "<<index_zero_positions_list.size()<<endl;  //
  cout<<"rot.im.zeroes: "<<rotated_image_zeroes_list.size()<<endl;  //
  
  for(int g=0; g<rasters_in_list; ++g)    //refer to 01_22_2019 notes on math and function of this section//
  {
    while(similar_raster_list.contains(g)) ++g;
    raster_position = g;
    
    //score range in descending series// (n*(n+1)/2)
    segment_start = (rasters_in_list*(rasters_in_list+1))/2 - (rasters_in_list-raster_position)*((rasters_in_list-raster_position)+1)/2;
    segment_end = (rasters_in_list*(rasters_in_list+1))/2 - (rasters_in_list-raster_position-1)*((rasters_in_list-raster_position))/2;
    
    for(int i=0; i<link_num; ++i) score_array[i] = 1; //initialize score_array  
    
    for(int h=segment_start+1; h<segment_end; ++h)
    {
      while(similar_raster_list.contains(h)) ++h;
      while(full_score_list.at(h)<similar_threshold) ++h;
      area_percent = full_score_list.at(h);
      
      if(area_percent < score_array[0])
      {
        score_array[4] = score_array[3];
        num_array[4] = num_array[3];
        score_array[3] = score_array[2];
        num_array[3] = num_array[2];
        score_array[2] = score_array[1];
        num_array[2] = num_array[1];
        score_array[1] = score_array[0];
        num_array[1] = num_array[0];
        score_array[0] = area_percent;
        num_array[0] = h-segment_start+g; //should this value be the index to some QList?//
      }
      else if(area_percent < score_array[1])
      {
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
  	    num_array[3] = num_array[2];        
        score_array[2] = score_array[1];
  	    num_array[2] = num_array[1];
        score_array[1] = area_percent;
        num_array[1] = h-segment_start+g;
      }
      else if(area_percent < score_array[2])     
      {        
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
        num_array[3] = num_array[2];
        score_array[2] = area_percent;
        num_array[2] = h-segment_start+g;
      }
      else if(area_percent < score_array[3])
      {
        score_array[4] = score_array[3];
        num_array[4] = num_array[3];
        score_array[3] = area_percent;
        num_array[3] = h-segment_start+g;
      }
      else if(area_percent < score_array[4])
      {  
        score_array[4] = area_percent;
        num_array[4] = h-segment_start+g;
      }
    }
    //segment_num.append(g);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DIAGNOSTICS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    cout<<"start: "<<segment_start<<"\t\tend: "<<segment_end<<endl;
    cout<<"G: "<<g<<endl;
    cin>>loop_counter;  //omit~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  }
  
  //==================================================================================================//
}
//==================================================================================================================================================//
void secondary_processor::remove_similar_rasters_2()	//compare and remove similar seeds in Qlist, link top 5 most similiar rasters to remaining
{
  for(int jkl=0; jkl<rotated_image_zeroes_list.size(); ++jkl)
  {
    cout<<"Removing: "<<jkl<<endl;  //OMIT~~~
    
    int test_start;
    int test_end;
    int test_width;
    int test_height;
    QSet<int> test_segment_print;
    int larger_dimension;
    int raster_selected;  //floor rounded int for selecting from raw_image_zeroes_list//
  
    if(jkl != 0) test_start = rotated_image_zeroes_list.at(jkl-1);
    else test_start = 0;
    test_end = rotated_image_zeroes_list.at(jkl);
    
    raster_selected = jkl/8;
    raster_selected = 3*raster_selected;
    test_width = raw_image_zeroes_list.at(raster_selected+1);              //unprocessed original dimensions
    test_height = raw_image_zeroes_list.at(raster_selected+2);
    //cout<<endl<<"TEST_WIDTH: "<<test_width<<"\traster selected: "<<raster_selected+1<<endl;     //OMIT~~~
    //cout<<endl<<"TEST_HEIGHT: "<<test_height<<"\traster_selected: "<<raster_selected+2<<endl;   //OMIT~~~
    
    if(rotated_image_dimensions.at(jkl*3+1) > rotated_image_dimensions.at(jkl*3+2)) larger_dimension = rotated_image_dimensions.at(jkl*3+1);  //TEST~~~//
    else larger_dimension = rotated_image_dimensions.at(jkl*3+2);                                                                             //TEST~~~//
    
    for(int x=test_start+1; x<test_end-1; ++x)                  //retrieve and store
    {
      test_segment_print.insert(rotated_image_list.at(x));
    }
    
    //for(int x=larger_dimension+2; x>=0; --x)      //print test segment//
    //{
        //for(int y=0; y<larger_dimension+2; ++y)
      //{
        //if(test_segment_print.contains(x+test_width*y)) cout<<"o ";
        //else cout<<". ";
      //}
      //cout<<endl;
    //}
  
    //-------------------------------------------------------------------// //TEST~~~//
    //centroid calculation//
    float rotated_segment_centroid_x = 0;
    float rotated_segment_centroid_y = 0;
    int rotated_segment_size;
    
    rotated_segment_size = test_segment_print.size();
    
    for(int q=larger_dimension+2; q>=0; --q)
    {
      for(int r=0; r<larger_dimension+2; ++r)
      {
        if(test_segment_print.contains(r+test_width*q))
        {
          rotated_segment_centroid_x = rotated_segment_centroid_x + r;
          rotated_segment_centroid_y = rotated_segment_centroid_y + q;
        }
      }
    }
    
    cout<<"centroid sum: ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;    //OMIT~~~//
    cout<<"centroid coordinates (new): ("<<float(rotated_segment_centroid_x/rotated_segment_size)       //OMIT~~~//
        <<","<<float(rotated_segment_centroid_y/rotated_segment_size)<<")"<<endl;                       //OMIT~~~//
    cout<<"centroid coordinates (rounded): ("<<round(rotated_segment_centroid_x/rotated_segment_size)   //OMIT~~~//
        <<","<<round(rotated_segment_centroid_y/rotated_segment_size)<<")"<<endl;                       //OMIT~~~//
        
    rotated_segment_centroid_x = round(rotated_segment_centroid_x/rotated_segment_size); //TEST~~~//
    rotated_segment_centroid_y = round(rotated_segment_centroid_y/rotated_segment_size); //TEST~~~//
    cout<<"centroid coordinates (old): ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;
    
    for(int x=larger_dimension+2; x>=0; --x)      //print test segment//
    {
      for(int y=0; y<larger_dimension+2; ++y)
      {
        if((x == rotated_segment_centroid_x) && (y == rotated_segment_centroid_y)) cout<<"X ";
        else if(test_segment_print.contains(x+test_width*y)) cout<<"o ";
        else cout<<". ";
      }
      cout<<endl;
    }
    
    cout<<"re-centred raster about origin"<<endl;
    //cout<<"centroid: ("<<rotated_segment_centroid_x<<","<<rotated_segment_centroid_y<<")"<<endl;
    for(int x=larger_dimension/2+1; x>=-larger_dimension/2-1; --x)
    {
      for(int y=-larger_dimension/2-1; y<larger_dimension/2+1; ++y)
      {
        if(x+test_width*y == 0) cout<<"M ";
        else if(test_segment_print.contains((x+rotated_segment_centroid_x) + test_width*(y+rotated_segment_centroid_y))) cout<<"o ";    //relative to centroid//
        else cout<<". ";
      }
      cout<<endl;
    }
    
    //-------------------------------------------------------------------// //print out raster index values//
    //cout<<endl<<"test_segment: "<<endl;
    
    //QList<int> test_segment_print_list = test_segment_print.toList();
    //qSort(test_segment_print_list);
    
    //for(int asd=0; asd<test_segment_print_list.size(); ++asd)
    //{
      //cout<<test_segment_print_list.at(asd)<<" ";
    //}
    //cout<<endl<<endl;
    //-------------------------------------------------------------------//
  
    //DIAGNOSTICS//
    cout<<"w: "<<test_width<<"\tH:" <<test_height<<endl;      //OMIT~~~
    cout<<"segment_size: "<<test_segment_print.size()<<endl;  //OMIT~~~
    cout<<"raw_dimensions: \nW: "<<rotated_image_dimensions.at(jkl*3+1)<<"\tH: "<<rotated_image_dimensions.at(jkl*3+2)<<endl;
    cout<<"larger_dimensions: "<<larger_dimension<<endl;      //OMIT~~~
    //DIAGNOSTICS//    
    
    test_segment_print.clear();
  
    //int loop_counter;
    //cin>>loop_counter;
  }
  
  //==================================================================================================//
  cout<<"REMOVE SIMILAR_2"<<endl; //OMIT~~~//
  //==================================================================================================//
  
  int segment_start = 0;        //new initialization
  int segment_end = 0;          //
  QList<int> index_segment_1;		//temporary list that contains a single raster  ***change to QSet, faster .contains() performance***
  QList<int> index_segment_2;   //

  int seed_area_1;
  int seed_area_2;
  int score = 0;        //denotes number of differences between two rasters
  int i = 0;            //increment segment 1 (raster 1)
  int j = 0;            //increment segment 2 (raster 2)
  
  float area_percent;             //Area of overlap over the larger raster area
  float larger_area;              //Stores the size of the larger raster
  int link_num = 5;               //number of similar rasters to link
  float score_array[link_num];    //temporary container to carry most similar raster links
  int num_array[link_num];        //
  
  //==================================================================================================//  new initializations
  //re-defined from index_segment_2 loop//
  int seed_width_1, seed_width_2, larger_width;     //define as function wide variable//
  int seed_height_1, seed_height_2, larger_height;  //
  
  int larger_dimension_1;
  int larger_dimension_2;
  
  int seed_start_1 = 0;   //define as function wide variable//
  int seed_end_1 = 0;     //
  int seed_start_2 = 0;   //
  int seed_end_2 = 0;     //
  
  int raster_selected_1;
  int raster_selected_2;
  //raster_selected_2= a/8;
  //raster_selected_2 = 3*raster_selected_2;

  //==================================================================================================//
      
//--------------------Link similar rasters--------------------------//
  QList<int> segment_num;
  QList<float> segment_similarity;
//-------------------redesigning similar comparison-----------------//
  int rasters_in_list;  
  int raster_position = 0;
//------------------------load segment 1----------------------------//
  for(int b=-1; b<rotated_image_zeroes_list.size()-1; ++b)
  {
    if(b == -1) segment_start = 0;   //TEST~~~//
   
    raster_selected_1= b/8;                     //TEST~~~
    //raster_selected_1= (b+1)/8;                     //TEST~~~
    raster_selected_1 = 3*raster_selected_1;    //TEST~~~
    
    for(i=0; i<link_num; ++i) score_array[i] = 1;   //score_array
    full_score_list.append(b);                                   
    //segment_end = rotated_image_zeroes_list[b+1];   //ORIGINAL~~~//
    segment_end = rotated_image_zeroes_list[b+1]-1;   //
    
    for(int i = segment_start; i<segment_end; ++i)    //ORIGINAL~~~//
    //for(int i = segment_start; i<segment_end-1; ++i)  //one less to not include 0,0//
    {
      index_segment_1.append(rotated_image_list[i]);
    }
    seed_area_1 = index_segment_1.size(); 
    
    cout<<"start_b: "<<segment_start<<"\tend_b: "<<segment_end<<endl;   //OMIT~~~
    
//========================================================================================================================================//  01_17_2019 overlap visualization test//
//------------------------------------------------retrieve and print rotated raster images------------------------------------------------//

      int seed_width_1, seed_width_2, larger_width;     //define as function wide variable//
      int seed_height_1, seed_height_2, larger_height;  //
      int larger_dimension_2;
      int seed_start_2 = 0;   //define as function wide variable//
      int seed_end_2 = 0;     //
      
      //used to define raster number//
      //int raster_selected_2 = b/8;
      //raster_selected_2 = 3*raster_selected_2;
      
      //used to define raster position in QList//
      if(b != -1) seed_start_1 = rotated_image_zeroes_list.at(b);
      else seed_start_1 = 0;
      seed_end_1 = rotated_image_zeroes_list.at(b+1);
      
      //define raster index dimensions//
      seed_width_1 = raw_image_zeroes_list.at(raster_selected_1+1);
      seed_height_1 = raw_image_zeroes_list.at(raster_selected_1+2);
      
      //define container dimensions//
      if(rotated_image_dimensions.at(raster_selected_1+1) > rotated_image_dimensions.at(raster_selected_1+2))
      {
        larger_dimension_1 = rotated_image_dimensions.at(raster_selected_1+1);
      }
      else larger_dimension_1 = rotated_image_dimensions.at(raster_selected_1+2);
      
      //retrieve and store index values//
      for(int x=seed_start_1+1; x<seed_end_1-1; ++x)
      {
        index_segment_1.append(rotated_image_list.at(x));
      }
    
      //print test segment 1//
      for(int x=larger_dimension_1+2; x>=0; --x)
      {
        for(int y=0; y<larger_dimension_1+2; ++y)
        {
          if(index_segment_1.contains(x+seed_width_1*y)) cout<<"o ";
          else cout<<". ";
        }
        cout<<endl;
      }
      //reset temporary QList container//
      index_segment_1.clear();
      int loop_counter;         //OMIT~~~//
      cin>>loop_counter;        //OMIT~~~//

//----------------------------------------------------------------------------------------------------------------------------------------//
//========================================================================================================================================//
    
//------------------------load segment 2----------------------------// 
    for(int a=b+1; a<rotated_image_zeroes_list.size(); ++a)
    {
      segment_start = rotated_image_zeroes_list[a]+1;
      //segment_end = rotated_image_zeroes_list[a+1];
      segment_end = rotated_image_zeroes_list[a+1]-1;   //one less to not include 0,0//
  
      for(j=segment_start; j<segment_end; ++j)
      {
        index_segment_2.append(rotated_image_list[j]);
      }
      seed_area_2 = index_segment_2.size();
      
      //~~~~~~~~~~~~~~~TEST~~~~~~~~~~~~~~~//
      cout<<"\tROTATED_IMAGE_ZEROES_LIST: "<<endl;                      //OMIT~~~//
      for(int asd = 0; asd<rotated_image_zeroes_list.size(); ++asd)     //
      {                                                                 //
        cout<<rotated_image_zeroes_list.at(asd)<<" ";                   //
      }                                                                 //
      cout<<endl<<endl;                                                 //
      cout<<"A: "<<a<<"\tB: "<<b<<endl;                                 //
      cout<<"start_1: "<<(rotated_image_zeroes_list[a]+1)<<"\tend_1: "  //
          <<rotated_image_zeroes_list[a+1]<<endl<<endl;                 //
      cout<<"B: "<<b<<endl; //                                          //
      cout<<"start_2: "<<(rotated_image_zeroes_list[b]+1)<<"\tend_1: "  //
          <<rotated_image_zeroes_list[b+1]<<endl<<endl;                 //
      
//========================================================================================================================================//

//========================================================================================================================================//  01_17_2019 overlap visualization test//
/*//------------------------------------------------retrieve and print rotated raster images------------------------------------------------//

      //int seed_width_1, seed_width_2, larger_width;     //define as function wide variable//
      //int seed_height_1, seed_height_2, larger_height;  //
      //int larger_dimension_2;
      //int seed_start_2 = 0;   //define as function wide variable//
      //int seed_end_2 = 0;     //
      
      //used to define raster number//
      int raster_selected_2 = a/8;
      raster_selected_2 = 3*raster_selected_2;
      
      //used to define raster position in QList//
      if(a != 0) seed_start_2 = rotated_image_zeroes_list.at(a-1);
      else seed_start_2 = 0;
      seed_end_2 = rotated_image_zeroes_list.at(a);
      
      //define raster index dimensions//
      //seed_width_1 = raw_image_zeroes_list.at(raster_selected_2+1);
      //seed_height_1 = raw_image_zeroes_list.at(raster_selected_2+2);
      seed_width_2 = raw_image_zeroes_list.at(raster_selected_2+1);
      seed_height_2 = raw_image_zeroes_list.at(raster_selected_2+2);
      
      //define container dimensions//
      if(rotated_image_dimensions.at(raster_selected_2+1) > rotated_image_dimensions.at(raster_selected_2+2))
      {
        larger_dimension_2 = rotated_image_dimensions.at(raster_selected_2+1);
      }
      else larger_dimension_2 = rotated_image_dimensions.at(raster_selected_2+2);
      
      //retrieve and store index values//
      for(int x=seed_start_2+1; x<seed_end_2-1; ++x)
      {
        //index_segment_1.append(rotated_image_list.at(x));
        index_segment_2.append(rotated_image_list.at(x));
      }
    
      //print test segment 2//
      for(int x=larger_dimension_2+2; x>=0; --x)
      {
        for(int y=0; y<larger_dimension_2+2; ++y)
        {
          //if(index_segment_1.contains(x+seed_width_1*y)) cout<<"o ";
          if(index_segment_2.contains(x+seed_width_2*y)) cout<<"o ";
          else cout<<". ";
        }
        cout<<endl;
      }
      //reset temporary QList container//
      //index_segment_1.clear();
      index_segment_2.clear();
      int loop_counter;         //OMIT~~~//
      cin>>loop_counter;        //OMIT~~~//

//----------------------------------------------------------------------------------------------------------------------------------------//*/
//========================================================================================================================================//      
//---------------------calculate scores method----------------------//
      int i = 0;
      int j = 0;
      
      while((i<seed_area_1) || (j<seed_area_2))
      {
        if(index_segment_1.at(i) == index_segment_2.at(j))      //no condition for when seeds end on same index value (stuck on encounter)
        {
          if(i<seed_area_1) ++i;
          if(j<seed_area_2) ++j;
        }
        else if(index_segment_1.at(i) > index_segment_2.at(j))
        {
          ++score;
          if(j<seed_area_2) ++j;
          else ++i;
        }
        else
        {
          ++score;
          if(i<seed_area_1) ++i;
          else ++j;
        }
      }
      i = 0;
      j = 0;    
//-------------------------Store Scores and Similar Raster Images------------------------//
      if(seed_area_1 > seed_area_2) larger_area = seed_area_1;      //determines larger seed area of the two and calculates % of score/area
      else larger_area = seed_area_2;
      area_percent = score/larger_area;  
      full_score_list.append(area_percent);
      //cout<<"AREA %: "<<area_percent<<"\tSCORE: "<<score<<endl;   //OMIT~~~//
      //cout<<"S1: "<<seed_area_1<<"\tS2: "<<seed_area_2<<endl;     //OMIT~~~//
      score = 0;
      
      if((area_percent < similar_threshold) && !similar_raster_list.contains(a))  //threshold defined in header
      {
        similar_raster_list.append(a);
      }
      index_segment_2.clear();
    } //segment 2 for loop
    index_segment_1.clear();
    segment_start = index_zero_positions_list[b]+1;     //iterates segment 1 loop
  } //segment 1 for loop

//--------------------------Store Best Scores in Array---------------------------//
  rasters_in_list = index_zero_positions_list.size();
  raster_position = 0;
  
  for(int g=0; g<rasters_in_list; ++g)
  {  
    while(similar_raster_list.contains(g)) ++g; //cycles through seqeuntial rasters deemed too similar
    raster_position = g;
    segment_start = (rasters_in_list*(rasters_in_list+1))/2 - (rasters_in_list-raster_position)*((rasters_in_list-raster_position)+1)/2;  //the +1 before /2 are used for rounding
    segment_end = (rasters_in_list*(rasters_in_list+1))/2 - (rasters_in_list-raster_position-1)*((rasters_in_list-raster_position))/2;
    for(int i=0; i<link_num; ++i) score_array[i] = 1;   	//score_array  
          
    for(int h=segment_start+1; h<segment_end; ++h)
    {
      while(similar_raster_list.contains(h)) ++h;
      while(full_score_list.at(h)<similar_threshold) ++h;				//TEST~~~
      area_percent = full_score_list.at(h);
      
      if(area_percent < score_array[0])
      {
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
  	    num_array[3] = num_array[2];        
        score_array[2] = score_array[1];
  	    num_array[2] = num_array[1];
        score_array[1] = score_array[0];
        num_array[1] = num_array[0];
        score_array[0] = area_percent;
        num_array[0] = h-segment_start+g;
      }    
      else if(area_percent < score_array[1])
      {
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
  	    num_array[3] = num_array[2];        
        score_array[2] = score_array[1];
  	    num_array[2] = num_array[1];
        score_array[1] = area_percent;
        num_array[1] = h-segment_start+g;
      }
      else if(area_percent < score_array[2])     
      {        
        score_array[4] = score_array[3];
  	    num_array[4] = num_array[3];
        score_array[3] = score_array[2];
        num_array[3] = num_array[2];
        score_array[2] = area_percent;
        num_array[2] = h-segment_start+g;
      }
      else if(area_percent < score_array[3])
      {
        score_array[4] = score_array[3];
        num_array[4] = num_array[3];
        score_array[3] = area_percent;
        num_array[3] = h-segment_start+g;
      }
      else if(area_percent < score_array[4])
      {  
        score_array[4] = area_percent;
        num_array[4] = h-segment_start+g;
      }  
    }
    segment_num.append(g);		    
    segment_similarity.append(g);
    for(int q=0; q<link_num; ++q)
    {
      segment_num.append(num_array[q]);
      segment_similarity.append(score_array[q]);
    }
  }
//---------------------------------reverse comparison--------------------------------//
  for(int i = segment_num.size()-(link_num+1); i>=0; i-=(link_num+1))
  {
    for(int j=0; j<i; j+=(link_num+1))
    {
      for(int n=1; n<(link_num+1); ++n)
      {
        if(segment_num.at(j+n)==segment_num.at(i))
        {
          if(segment_similarity.at(j+n) < segment_similarity.at(i+1))         //Replace if statements with for loops later  (or check remove and pushback functions)
          {      
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(i+3)));
            segment_num.replace((i+3),(segment_num.at(i+2)));
            segment_num.replace((i+2),(segment_num.at(i+1)));
            segment_num.replace((i+1),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(i+3)));
            segment_similarity.replace((i+3),(segment_similarity.at(i+2)));
            segment_similarity.replace((i+2),(segment_similarity.at(i+1)));
            segment_similarity.replace((i+1),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+2))
          {
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(i+3)));
            segment_num.replace((i+3),(segment_num.at(i+2)));
            segment_num.replace((i+2),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(i+3)));
            segment_similarity.replace((i+3),(segment_similarity.at(i+2)));
            segment_similarity.replace((i+2),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+3))
          {
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(i+3)));
            segment_num.replace((i+3),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(i+3)));
            segment_similarity.replace((i+3),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+4))
          {
            segment_num.replace((i+5),(segment_num.at(i+4)));
            segment_num.replace((i+4),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(i+4)));
            segment_similarity.replace((i+4),(segment_similarity.at(j+n)));
          }
          else if(segment_similarity.at(j+n) < segment_similarity.at(i+5))
          {
            segment_num.replace((i+5),(segment_num.at(j)));
            segment_similarity.replace((i+5),(segment_similarity.at(j+n)));
          }
        }
      }
    }
  }
//--------------------------------Remove Similar Rasters from List----------------------------//
  qSort(similar_raster_list.begin(), similar_raster_list.end());
  for(int h=similar_raster_list.size()-1; h>=0; --h)	//similar_raster_list
  {
    segment_start = index_zero_positions_list[(similar_raster_list[h])-1]+1;
    segment_end = index_zero_positions_list[(similar_raster_list[h])];
    for(int f=segment_end; f>segment_start; --f)
    {
      index_list.removeAt(f);	  //removes similar_raster zero position index from list
    }
  }
  int previous_zero_position = 0;
  index_zero_positions_list.clear();
  for(int q=0; q<index_list.size(); ++q)
  {
    previous_zero_position = index_list.indexOf(0,q-1);
    if(index_list.indexOf(0,q) > previous_zero_position)
    {
      index_zero_positions_list.append(index_list.lastIndexOf(0,q));
    }
  }
//--------------------------Diagnostics-------------------------//
  cout<<endl<<"Removing: "<<similar_raster_list.size()<<"/"<<calibration_seed_number*8<<endl;     //OMIT~~~//
  cout<<"Similarity Threshold: "<<100-100*similar_threshold<<"%"<<endl;                            //OMIT~~~//
//--------------------------------------------------------------//
  /*cout<<endl<<"segment_num: "<<endl;            
  for(int g=0; g<segment_num.size(); ++g)
  {
    cout<<segment_num[g]<<" ";
  }
  cout<<endl<<"segment_similarity: "<<endl;
  for(int g=0; g<segment_similarity.size(); ++g)
  {
    cout<<segment_similarity[g]<<" ";
  }*/
  cout<<endl<<"similar_raster_list: "<<"("<<similar_raster_list.size()<<" @ "<<similar_threshold<<")"<<endl;
  for(int g=0; g<similar_raster_list.size(); ++g)
  {
    cout<<similar_raster_list[g]<<" ";
  }
  cout<<endl<<"zero_positions_list: ("<<index_zero_positions_list.size()<<")"<<endl;
  for(int g=0; g<index_zero_positions_list.size(); ++g)
  {
    cout<<index_zero_positions_list[g]<<" ";
  }
//---------------------------------------------------------------//
  cout<<endl<<"----------------------------Calibration Data Ready-----------------------------"<<endl;
}
//==================================================================================================================================================//

void secondary_processor::raster_comparison(int cluster_num, bool* bool_raster_p)  //function for comparing raster library images to live/camera images
//void secondary_processor::raster_comparison(int cluster_num)                     //try to re-define all variables from this function in main processor to have variables persist//
{
  cout<<"============================================================================================================================"<<endl; //OMIT~~~//
  cout<<"Raster Comparison"<<endl;  //OMIT~~~//
  /*//-------------------------------------------------------------------------------------------------------------------------//
  cout<<"similar_raster dimensions: "<<raw_image_zeroes_list.size()<<endl;  //OMIT~~~//
  
  for(int jkl=0; jkl<raw_image_zeroes_list.size(); ++jkl)   //OMIT~~~//
  {
    cout<<raw_image_zeroes_list.at(jkl)<<", ";
  }
  
  //-------------------------------------------------------------------------------------------------------------------------//*/
  
  int camera_seed_size;
  int calibration_seed_size;
  int combined_raster_size;

  float larger_area = 0;
  float area_percent;
  int score = 0;
  int i,j;               		    //iterators for random_seed_set, camera_seed_set, and calibrated_seed_set

  int segment_start = 0;      	//variables for extracting raster index values
  int segment_end = 0;
  QList<int> calibration_seed;
  QList<int> camera_seed;
  
  float best_score = 1;
  int best_raster = 0;          	  //raster # with best fit to raster from camera
  int best_raster_size = 0;
  float y_scaling;
  int yfactor = 1;   //TEST?~~~//
  
  QSet<int> calibrated_seed_set;
  QSet<int> random_seed_set;
  QSet<int> random_seed_set_2;
  QSet<int> random_seed_set_3;
  QSet<int> similar_raster_set = similar_raster_list.toSet();
//-----------------------------------------------------------------------------------------------------------//
  int rotated_raster_index = 0;		//remapped index for rasters after shifts and rotation
  int line_start;					        //define start + end positions of lines for gap fill functions
  int line_end;	
//-----------------------------------------------------------------------------------------------------------//
//===========================================================================================================//
  //approximate centroids of overlapped seeds in blob - obtained from k-means clustering algorithm//
  
  //currently centroids are found for full size images, must re-order functions so k-means clustering analyzes resized raster image blobs//
  
  
  //cout<<"Expected Seed #: "<<cluster_num<<endl;                       //OMIT~~~
  //if(cluster_num >= 1) cout<<"c1_x: "<<c1_x<<"  c1_y: "<<c1_y<<endl;  //OMIT~~~
  //if(cluster_num >= 2) cout<<"c2_x: "<<c2_x<<"  c2_y: "<<c2_y<<endl;  //OMIT~~~
  //if(cluster_num >= 3) cout<<"c3_x: "<<c3_x<<"  c3_y: "<<c3_y<<endl;  //OMIT~~~
  //if(cluster_num >= 4) cout<<"c4_x: "<<c4_x<<"  c4_y: "<<c4_y<<endl;  //OMIT~~~
  
  //display_raster_p[c2_mean_x+width*c2_mean_y]='~';      //centroid
  
  //temporary: calculate percentage of centroid relative to centre//
  //float percentage_c1_x, percentage_c2_x, percentage_c3_x, percentage_c4_x;
  //float percentage_c1_y, percentage_c2_y, percentage_c3_y, percentage_c4_y;
  
  //percentage_c1_x = 1.0*c1_x/(width);
  //percentage_c2_x = 1.0*c2_x/(width);
  //percentage_c3_x = 1.0*c3_x/(width);
  //percentage_c4_x = 1.0*c4_x/(width);
  //percentage_c1_y = 1.0*c1_y/(height);
  //percentage_c2_y = 1.0*c2_y/(height);
  //percentage_c3_y = 1.0*c3_y/(height);
  //percentage_c4_y = 1.0*c4_y/(height);
  
  //if(cluster_num >= 1) cout<<"c1x_%: "<<percentage_c1_x<<"  c1y_%: "<<percentage_c1_y<<endl;  //OMIT~~~
  //if(cluster_num >= 2) cout<<"c2x_%: "<<percentage_c2_x<<"  c2y_%: "<<percentage_c2_y<<endl;  //OMIT~~~
  //if(cluster_num >= 3) cout<<"c3x_%: "<<percentage_c3_x<<"  c3y_%: "<<percentage_c3_y<<endl;  //OMIT~~~
  //if(cluster_num >= 4) cout<<"c4x_%: "<<percentage_c4_x<<"  c4y_%: "<<percentage_c4_y<<endl;  //OMIT~~~
  
//===========================================================================================================//
//-----------------------------------------------------------------------------------------------------------//  
  
  //x_scale_factor = (1.0+scaling_buffer)*largest_dimension/rmax_x;	//TEST~~~
  x_scale_factor = 1.0*average_width/average_height;
  
  //width = width/x_scale_factor;     //TEST~~~//
  //height = height/y_scale_factor;   //TEST~~~//       
  
  half_width = width/2;						    //determine center position of raster in bitmap container
  half_height = height/2;
  start_raster_x = -half_width;	      //define boundaries of raster image in enlarged raster
  start_raster_y = -half_height;
  end_raster_x = half_width;
  end_raster_y = half_height;
//-----------------------------------------------------------------------------------------------------------//
  for (double y=start_raster_y; y<end_raster_y; ++y)	//performs transformation on seeds from camera (or from playback)
  {
    for (double x=start_raster_x; x<end_raster_x; ++x)
    {  
      raster_position_in_container = (x-start_raster_x)+width*(y-start_raster_y);
      if(bool_raster_p[raster_position_in_container])
      {
        //shifted_x = x;  //not compressed//
        shifted_y = y;
        shifted_x = x/x_scale_factor;    //compressed// //TEST~~~//
        //shifted_y = y/y_scale_factor;    //
        
        //unshifted_x = shifted_x;
        //unshifted_y = shifted_y;
        unshifted_x = shifted_x + width/2;
        unshifted_y = shifted_y + height/2;
        
        rotated_raster_index = unshifted_x+width*unshifted_y;
        camera_seed.append(rotated_raster_index);
      }
    }
  }
  //-----------------------------------------------------------------------------------------------------------// //TEST~~~//
  //determine centroids of live raster and calibration raster image// test//01_08_2019//
  /*int camera_seed_centroid = 0;
  
  for(int x=0; x<camera_seed.size(); ++x)
  {
    camera_seed_centroid = camera_seed_centroid + camera_seed.at(x);
  }
  camera_seed_centroid = camera_seed_centroid/camera_seed.size();
  
  cout<<"camera_seed_centroid: "<<camera_seed_centroid<<endl;*/
  //centroid is slightly inaccurate//
  
  
  //int library_seed_centroid = 0;
  //-----------------------------------------------------------------------------------------------------------// //TEST~~~//
  //TEST=================================================================~~~//visualization - printout to terminal
      /*int raster_area_count = 0;      //OMIT~~~//
      
      for (int q=height; q>=0; --q)   //TEST~~~//
      {
        for(int r=(width/2-width/x_scale_factor/2-1); r<(width/2 + width/x_scale_factor/2+1); ++r)       //TEST~~~//
        {
          if(camera_seed.contains(r + width*q))
          {
            cout<<"o ";
            ++raster_area_count;
          }
          else cout<<". ";
          
          //if((r + width*q) == camera_seed_centroid) cout<<"X "; //OMIT~~~//
        }
        cout<<endl;
      }*/
  //========================================================================//
//------------------------------------------------------------------------------------------------------------------------------------------//
  QSet<int> camera_seed_set = camera_seed.toSet();

  //for(int y=0; y<rmax_y; ++y) //original~~~//
  for(int y=0; y<width; ++y)
  {
    line_start = width;
    line_end = 0;
    //for(int x=0; x<rmax_x; ++x)	//first pass through line to find start + end positions of column
    for(int x=0; x<width; ++x)	//test~~~//
    {
      if(camera_seed_set.contains(y+width*x))
      {
        line_end = x;
        if(line_end<line_start) line_start = x;
      }
    }      
    //for(int x=0; x<rmax_x; ++x)	//fills gaps that exist between endpoints of line
    for(int x=0; x<width; ++x)	//test~~~//
    {
      //if((!camera_seed_set.contains(y+rmax_x*x)) && x>line_start && x<line_end) camera_seed_set.insert(y+rmax_x*x); //original~~~
      if((!camera_seed_set.contains(y+width*x)) && x>line_start && x<line_end) camera_seed_set.insert(y+width*x); //TEST~~~
    }
  }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//  
  //QSet<int> camera_seed_set = camera_seed.toSet();  	//might be able to omit this section entirely
  qSort(camera_seed.begin(), camera_seed.end());		    //sort single raster in ascending order
  camera_seed.clear();
  camera_seed = camera_seed_set.toList();     			    //TEST~~
  qSort(camera_seed.begin(), camera_seed.end());		    //sort single raster in ascending order
  camera_seed_size = camera_seed.size();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  for(int q=0; q<index_zero_positions_list.size(); ++q)  	//load rotated raster images into list
  {
    if(!calibration_seed.empty()) calibration_seed.clear();
    while(similar_raster_set.contains(q))
    {
      ++q;
      segment_start = index_zero_positions_list[q-1]+1;
    }
    segment_end = index_zero_positions_list[q];
    
    for(j=segment_start; j<segment_end; ++j)
    {
      calibration_seed.append(index_list[j]);
    }
    calibration_seed_size = calibration_seed.size();
      
    score = 0;    
    i = 0;
    j = 0;
    
    
  //===========================================================================================================// //TEST~~~//
  //determine centroids of live raster and calibration raster image// test//01_08_2019//
    /*int camera_seed_centroid = 0;
  
    for(int x=0; x<camera_seed.size(); ++x)
    {
      camera_seed_centroid = camera_seed_centroid + camera_seed.at(x);
    }
    camera_seed_centroid = camera_seed_centroid/camera_seed.size();
  
    cout<<"camera_seed_centroid: "<<camera_seed_centroid<<endl;
  
  
    int calibration_seed_centroid = 0;
    
    for(int x=0; x<calibration_seed.size(); ++x)
    {
      calibration_seed_centroid = calibration_seed_centroid + calibration_seed.at(x);
    }
    calibration_seed_centroid = calibration_seed_centroid/calibration_seed.size();
    
    cout<<"calibration_seed_centroid: "<<calibration_seed_centroid<<endl;*/
  //===========================================================================================================// //TEST~~~//
    
    //===========================================================================================================//
    //seeds are not centered and will generate slightly inaccurate overlap score//
    /*for (int q=width; q>=-width/2; --q)
    {
      for (int r=-width/2 - 1; r<width/2 + 1; ++r)       //TEST~~~//
      {
        if(camera_seed.contains(r + width*q))
        {
          cout<<"o ";
        }
        else if(calibration_seed.contains(r+width*q))
        {
          cout<<"a ";
        }
        else cout<<". ";
      }
      cout<<endl;
    }
    int loop_counter;
    cin>>loop_counter;*/
    //===========================================================================================================//
    
    while((i<camera_seed_size) || (j<calibration_seed_size))	//calculates score for single raster comparison
    {
      if(camera_seed.at(i) == calibration_seed.at(j))      		//***no condition for when seeds end on same index value (stuck on encounter)
      {
        if(i<camera_seed_size) ++i;
        if(j<calibration_seed_size) ++j;
      }
      else if(camera_seed.at(i) > calibration_seed.at(j))
      {
        ++score;
        if(j<calibration_seed_size) ++j;
        else ++i;
      }
      else
      {
        ++score;
        if(i<camera_seed_size) ++i;
        else ++j;
      }
    }
    if(camera_seed_size > calibration_seed_size) larger_area = camera_seed_size;
    else larger_area = calibration_seed_size;
    area_percent = score/larger_area;
    if(area_percent < best_score) 
    {
      best_score = area_percent;
      best_raster = q;
      best_raster_size = calibration_seed_size;
    }
    segment_start = segment_end+1;
  }  
  
  x_scale_factor = (1.0+scaling_buffer)*largest_dimension/rmax_x;                         //TEST~~~
  
  cout<<"\033[1;31m";                                                                     //OMIT~~~//
  cout<<"SB: "<<scaling_buffer<<"\tLD: "<<largest_dimension<<"\tRmax: "<<rmax_x<<endl;    //OMIT~~~//
  cout<<"\033[0m";                                                                        //OMIT~~~//
  
//--------------------------------------------------Diagnostics-------------------------------------------------//
  cout<<"best_score: "<<best_score<<"   best_raster: "<<best_raster<<"	score: "<<best_score*larger_area<<endl;
  cout<<"camera_seed_size: "<< camera_seed_size<<"  best_fit_size: "<<best_raster_size<<endl;   
  cout<<"W: "<<width<<"	H: "<<height<<endl;
  cout<<"adjusted_W: "<<width/x_scale_factor<<"\tadjusted_H: "<<height*(yfactor*1.2)/x_scale_factor<<endl;
  
  cout<<"TEST x_scale: "<<x_scale_factor<<"\t y_factor: "<<yfactor<<endl<<endl;   //OMIT~~~~//
//--------------------------------------------------------------------------------------------------------------//
  
  display_rotated_raster_p = new char[rmax_x*rmax_y];     

  if(best_raster > 0) segment_start = index_zero_positions_list[best_raster-1]+1;	//retrieve and load best raster
  else segment_start = 0;
  segment_end = index_zero_positions_list[best_raster];
  calibrated_seed_set.clear();

  for(int j=segment_start; j<segment_end; ++j)  //store best_fitting raster to QSet
  {
    calibrated_seed_set.insert(index_list[j]);
  }
  
  for(int y=0; y<rmax_y; ++y)					//generate empty raster image
  {
    for(int x=0; x<rmax_x; ++x)
    {
      display_rotated_raster_p[x+rmax_x*y]=' ';
    }
  }   	
  for(int y=0; y<rmax_y; ++y)
  {
    for(int x=0; x<rmax_x; ++x)
    {
      if(camera_seed_set.contains(x+rmax_x*y) && calibrated_seed_set.contains(x+rmax_x*y))		//generates a display_raster from stored index values
      {
        display_rotated_raster_p[x+rmax_x*y]='x';
      }
      else if (camera_seed_set.contains(x+rmax_x*y))
      {
        display_rotated_raster_p[x+rmax_x*y]='o';
      }
      else if (calibrated_seed_set.contains(x+rmax_x*y))
      {
        display_rotated_raster_p[x+rmax_x*y]='|';
      }
      else display_rotated_raster_p[x+rmax_x*y]='.';
    }
  }
  
  //int test_var = rmax_x*percentage_c1_x + rmax_x * (rmax_y*percentage_c1_y);
  //TEST~~~
  //if(cluster_num >= 1) display_rotated_raster_p[1*rmax_x*percentage_c1_x+1*rmax_x*(percentage_c1_y*rmax_y)]='1';
  //if(cluster_num >= 2) display_rotated_raster_p[1*rmax_x*percentage_c2_x+rmax_x*(percentage_c2_y*rmax_y)]='2';
  //if(cluster_num >= 3) display_rotated_raster_p[1*rmax_x*percentage_c3_x+rmax_x*(percentage_c3_y*rmax_y)]='3';
  //if(cluster_num >= 4) display_rotated_raster_p[1*rmax_x*percentage_c4_x+rmax_x*(percentage_c4_y*rmax_y)]='4';
  
  //if(cluster_num >= 1) display_rotated_raster_p[percentage_c1_x+1*rmax_x*(percentage_c1_y*rmax_y)]='1';  
  //if(cluster_num >= 1) display_rotated_raster_p[test_var*rmax_x*percentage_c1_x] ='1'];  
  //if(cluster_num >= 1) display_rotated_raster_p[test_var] ='1';
  //if(cluster_num >= 1) display_rotated_raster_p[test_var] ='1';
  
  //print_rotated_rasters();	//TEST~~~
//===========================================================================================================//
//---------------------------------------------random function test------------------------------------------//
//===========================================================================================================//
  int rand_x = 0;				    //raster library translations for comparing to static camera images
  int rand_y = 0;
  int rand_x_2 = 0;
  int rand_y_2 = 0;
  int rand_x_3 = 0;
  int rand_y_3 = 0;
  int rand_x_4 = 0;
  int rand_y_4 = 0;
  
  int shift_tot = 0;			  //store total translation amount for positions of improved overlap
  int shift_tot_2 = 0;
  int shift_tot_3 = 0;
  int shift_tot_4 = 0;
  
  float shift_score = 1;
  int rand_raster;          //TEST~~ random secondary raster when camera image is large and has a best score over similarity threshold
  int rand_raster_2;
  int rand_raster_3;
  
  int num_seeds_tried = 1;
  int loop_iterations = 0;
  int redundancy_score = 0;	//TEST~~~ used to define overlap of calibration seeds
  
  srand(time(NULL));    		//seed rand function for generating random sequences each runtime
  //seed_number_overlap_test = 1;
     
  float target_score = 0.09 + variability_score;	//TEST~~~ DYNAMIC	07_18a
    
  float previous_best_score;			    //variables used to define best score achieved for previous and current number of seeds used to test overlap
  float current_best_score;				    //TEST~~~
  previous_best_score = best_score;		//TEST~~~	store the score from single static image comparisons
  current_best_score = 1;				      //TEST~~~	initialize current best score
//-----------------------------------------------------------------------------------------------------------------------------//
  int row_start = (rmax_y_store-rmax_y)/2;			//define raster boundaries within container
  int row_end = (rmax_y_store+rmax_y)/2;
  int column_start = (rmax_x_store-rmax_x)/2;
  int column_end = (rmax_x_store+rmax_x)/2;

  rand_raster = rand() % index_zero_positions_list.size();		//initialize random raster for first iteration
  rand_raster_2 = rand() % index_zero_positions_list.size();
  rand_raster_3 = rand() % index_zero_positions_list.size();
//-----------------------------------------------------------------------------------------------------------------------------//
  display_rotated_raster_p = new char[rmax_x_store*rmax_y_store];
  
  int display_width, display_height;									          //TEST~~~
  display_width = width/x_scale_factor+4;								        //TEST~~~
  display_height = (height+4)*yfactor*1.2/x_scale_factor;				//TEST~~~
  //display_width = width/x_scale_factor+10;								    //TEST~~~  
  //display_height = (height+10)*yfactor*1.2/x_scale_factor;		//TEST~~~  
  //design cases for when the overlap is larger than the predefined display container size (overcome segfault)
  
  if(display_width > rmax_x_store && display_width > display_height)
  {
    display_rotated_raster_p = new char[display_width*display_width];
  }
  else if(display_height > rmax_y_store && display_height > display_width) 
  {
    display_rotated_raster_p = new char[display_height*display_height];
  }
//===================================================================================================================================//
//---------------------------------------------------------All Random Shift Test-----------------------------------------------------//
//===================================================================================================================================//
  //Note: threshold values defined are chosen through manually documenting characteristics of overlapping raster images
	//	    values should be automatically defined through a dynamic calibration phase prior to use
  //while((best_score*larger_area/average_calibration_seed_size > 0.25) && (best_score > target_score) && (area>area_mean-1.0*std_deviation_area))		//TEST~~~ 07_17c
  //while((best_score*larger_area/average_calibration_seed_size > 0.25) && (best_score > target_score) && (area>area_mean+2.0*std_deviation_area))		//TEST~~~ 07_23a	canola
  //while((best_score*larger_area/average_calibration_seed_size > 0.25) && (best_score > target_score) && (area>area_mean+2.0*std_deviation_area))		//TEST~~~ modify function to accomodate canola
  
  average_calibration_seed_size = average_area/calibration_seed_number; //TEST alternative to average_calibration_seed_size//
  //average_calibration_seed_size = average_area; //12_10_2018 TEST~~~//
  //average_calibration_seed_size = average_area/(x_scale_factor); //12_10_2018 TEST~~~// FIX DENOMINATOR//
  
  
  cout<<"single target: "<<target_score<<"	";	//OMIT~~~
  cout<<"best: "<<best_score<<"	larger: "<<larger_area<<"	average_size: "<<average_calibration_seed_size<<"	X= "<<best_score*larger_area/average_calibration_seed_size<<endl;			          //OMIT~~~
  //cout<<"SD scale: "<<(5-60.0*variability_score)<<"	area:"<<area<<"	c.area:"<<(area_mean+((4-40.0*variability_score)*std_deviation_area))<<" ("<<(4-40.0*variability_score)<<")"<<endl;		//OMIT~~~
  cout<<"area:"<<area<<"	c.area:"<<(area_mean+(((((1-10.0*variability_score)-0.35)*10)-0.35)*std_deviation_area))<<" ("<<((((1-10.0*variability_score)-0.35)*10)-0.35)<<")"<<endl;		      //OMIT~~~
  
  cout<<endl<<endl;																									                                              //OMIT~~~
  //cout<<"cond1: best_score-"<<best_score<<" larger_area-"<<larger_area<<" average_calibration_seed_size-"<<average_calibration_seed_size<<endl; //OMIT~~~//
  
  cout<<"<cond1> "<<best_score*larger_area/average_calibration_seed_size<<" - 0.25"									              //OMIT~~~
	  //<<"		adjusted: ("<<10*variability_score*best_score*larger_area/average_calibration_seed_size				        //OMIT~~~
	  //<<"		adjusted: ("<<(1/(30*variability_score))*best_score*larger_area/average_calibration_seed_size-0.1	    //OMIT~~~
      <<"		adjusted: ("<<(1/(20*variability_score))*best_score*larger_area/average_calibration_seed_size-0.1	    //OMIT~~~
      <<" - "<<(0.25+2*variability_score)<<")"<<endl;																                              //OMIT~~~
  cout<<"<cond2> "<<best_score<<" - "<<target_score<<endl;															                          //OMIT~~~
  cout<<"<cond3> "<<area<<" - "<<area_mean+(((((1-10.0*variability_score)-0.35)*10))*std_deviation_area)			    //OMIT~~~
      <<"		# of SD ("<<(area-area_mean)/std_deviation_area<<")"<<endl;											                      //OMIT~~~
  cout<<endl<<"<cond*1> "<<(1/(20*variability_score))*best_score*larger_area/average_calibration_seed_size<<" - "
      <<0.125*(1/(10*variability_score))<<endl;		//OMIT~~~ 	07_30_2018b
	  
//----------------------------------------------------------------------------------------------------------------------------//
  cout<<"\033[1;31m";                                                                                                                                     //OMIT~~~//
  cout<<"cond1"<<endl;                                                                                                                                    //OMIT~~~//
  cout<<"Best_score: "<<best_score<<"\tLarger_area: "<<larger_area<<"\taverage_calibration_seed_size: "<<average_calibration_seed_size<<endl;             //OMIT~~~//
  
  cout<<"cond2"<<endl;                                        
  cout<<"best_score: "<<best_score<<"\ttarget_score: "<<target_score<<endl;
  
  cout<<"cond3"<<endl;
  cout<<"area: "<<area<<"\tarea_mean: "<<area_mean<<"\tvariability_score: "<<variability_score<<"\tstd_deviation_area: "<<std_deviation_area<<endl;
  
  cout<<"variability_score: "<<variability_score<<endl;  //OMIT~~~//
  cout<<"std_deviation: "<<std_deviation_area<<"\tarea_mean: "<<area_mean<<endl;  //OMIT~~~//
  //variability_score = (std_deviation_area/area_mean + std_deviation_height/height_mean + std_deviation_width/width_mean)/10;

  
  cout<<"\033[0m";                                                                                                                                        //OMIT~~~//
//----------------------------------------------------------------------------------------------------------------------------//
    
  
  while((1/(20*variability_score))*best_score*larger_area/average_calibration_seed_size > 0.125*(1/(20*variability_score)) && 
       (best_score > target_score) && 
       //(best_score > (qFabs(camera_seed_size-best_raster_size)/larger_area)	//theoretical best vs best_score
       (area > area_mean+2.0*std_deviation_area))		//TEST~~~ modify function to accomodate canola
  {
    if(seed_number_overlap_test >= 1)	//rasters: calibration
    {
      rand_x = rand() % 5 + (-2);
      rand_y = rmax_x*(rand() % 5 + (-2));
	  
      if(best_raster > 0) segment_start = index_zero_positions_list[best_raster-1]+1;
      else segment_start = 0;
      segment_end = index_zero_positions_list[best_raster];
      if(!calibrated_seed_set.empty()) calibrated_seed_set.clear();
      for(int j=segment_start; j<segment_end; ++j)
      {
        calibrated_seed_set.insert(index_list[j]+rand_x+rand_y+shift_tot);
      }
    }
//------------------------------------------------------------------------------------------------------------------//
    if(seed_number_overlap_test >= 2)	//rasters: calibration + 1random
    {
      rand_raster = rand() % index_zero_positions_list.size();							          //random raster
      rand_x_2 = rand() % 5 + (-2);														                        //random shift
      rand_y_2 = rmax_x*(rand() % 5 + (-2));
	  
      if(rand_raster > 0) segment_start = index_zero_positions_list[rand_raster-1]+1;	//define start and end points of raster selected
      else segment_start = 0;
      segment_end = index_zero_positions_list[rand_raster];
      if(!random_seed_set.empty()) random_seed_set.clear();								            //retrieve and load raster into QSet
      for(int j=segment_start; j<segment_end; ++j)
      {
        random_seed_set.insert(index_list[j]+rand_x_2+rand_y_2+shift_tot_2);
      }
    }
//------------------------------------------------------------------------------------------------------------------//
    if(seed_number_overlap_test >= 3)	//rasters: calibration + 2random
    {
      rand_raster_2 = rand() % index_zero_positions_list.size();
      rand_x_3 = rand() % 5 + (-2);
      rand_y_3 = rmax_x*(rand() % 5 + (-2));
	
      if(rand_raster_2 > 0) segment_start = index_zero_positions_list[rand_raster_2-1]+1;
      else segment_start = 0;
      segment_end = index_zero_positions_list[rand_raster_2];
      if(!random_seed_set_2.empty()) random_seed_set_2.clear();
      for(int j=segment_start; j<segment_end; ++j)
      {
        random_seed_set_2.insert(index_list[j]+rand_x_3+rand_y_3+shift_tot_3);
      }
    }
//------------------------------------------------------------------------------------------------------------------//
    if(seed_number_overlap_test == 4)	//rasters: calibration + 3random
    {
      rand_raster_3 = rand() % index_zero_positions_list.size();
      rand_x_4 = rand() % 5 + (-2);
      rand_y_4 = rmax_x*(rand() % 5 + (-2));
      
      if(rand_raster_3 > 0) segment_start = index_zero_positions_list[rand_raster_3-1]+1;
      else segment_start = 0;
      segment_end = index_zero_positions_list[rand_raster_3];
      if(!random_seed_set_3.empty()) random_seed_set_3.clear();
      for(int j=segment_start; j<segment_end; ++j)
      {
        random_seed_set_3.insert(index_list[j]+rand_x_4+rand_y_4+shift_tot_4);
      }
    }
//-----------------------------------------------------------------------------------------------------------------------------//	//may be able to omit these initializations
    row_start = (rmax_y_store-rmax_y)/2;	  //initializations
    row_end = (rmax_y_store+rmax_y)/2;
    column_start = (rmax_x_store-rmax_x)/2;
    column_end = (rmax_x_store+rmax_x)/2;
    score = 0;
    redundancy_score = 0;	                  //test~~~ used to define overlap of calibration seeds
//-----------------------------------------------------------------------------------------------------------------------------//  
    for(int y=0; y<rmax_y_store; ++y)		    //determine score
    {
      for(int x=0; x<rmax_x_store; ++x)
      {
        display_rotated_raster_p[x+rmax_x_store*y]=' ';	  //TEST~~~
      }
    }
    for(int y=column_start; y<column_end; ++y)				    //TEST~~~
    {
      for(int x=row_start; x<row_end; ++x)					      //TEST~~~
      {
        display_rotated_raster_p[x+rmax_x_store*y]='.';		//TEST~~~
      }
    } 
//=============================================================================================================================//  	
    for(int y=1; y<=rmax_y_store; ++y)
    {
      for(int x=1; x<=rmax_x_store; ++x)
      {
	  //---------------------------------------------------------------------------------------------------------------------------------------------------------------//
        if((x < (rmax_x_store/2 + 1.1*width/x_scale_factor/2)) && (x > (rmax_x_store/2 - 1.1*width/x_scale_factor/2)))	//TEST~~~	remove duplicate images
        {
		//modify to add width, height works however
      //---------------------------------------------------------------------------------------------------------------------------------------------------------------//  	
          if(calibrated_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) && random_seed_set.contains((x-column_start)+rmax_x*(y-row_start))) ++redundancy_score;
          if(calibrated_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) && random_seed_set_2.contains((x-column_start)+rmax_x*(y-row_start))) ++redundancy_score;
          if(calibrated_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) && random_seed_set_3.contains((x-column_start)+rmax_x*(y-row_start))) ++redundancy_score;
          if(random_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) && random_seed_set_2.contains((x-column_start)+rmax_x*(y-row_start))) ++redundancy_score;
          if(random_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) && random_seed_set_3.contains((x-column_start)+rmax_x*(y-row_start))) ++redundancy_score;
          if(random_seed_set_2.contains((x-column_start)+rmax_x*(y-row_start)) && random_seed_set_3.contains((x-column_start)+rmax_x*(y-row_start))) ++redundancy_score;
      //---------------------------------------------------------------------------------------------------------------------------------------------------------------//  		
          if(camera_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) && (calibrated_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) || 
            random_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) || random_seed_set_2.contains((x-column_start)+rmax_x*(y-row_start)) ||
            random_seed_set_3.contains((x-column_start)+rmax_x*(y-row_start))))
          {
            display_rotated_raster_p[x+rmax_x_store*y]='x';
          }
          else if(camera_seed_set.contains((x-column_start)+rmax_x*(y-row_start)))
          {
            display_rotated_raster_p[x+rmax_x_store*y]='|';
            ++score;
          }
          else if(calibrated_seed_set.contains((x-column_start)+rmax_x*(y-row_start)))
          {
            display_rotated_raster_p[x+rmax_x_store*y]='c';
            ++score;
          }
          else if(random_seed_set.contains((x-column_start)+rmax_x*(y-row_start)))
          {
            display_rotated_raster_p[x+rmax_x_store*y]='1';
            ++score;
          }
          else if(random_seed_set_2.contains((x-column_start)+rmax_x*(y-row_start)))
          {
            display_rotated_raster_p[x+rmax_x_store*y]='2';
            ++score;
          }
          else if(random_seed_set_3.contains((x-column_start)+rmax_x*(y-row_start)))
          {
            display_rotated_raster_p[x+rmax_x_store*y]='3';
            ++score;
          }
        }
      }
	  }
//=============================================================================================================================//  	
    combined_raster_size = 0;				  //Calculate double counted area of test seeds
    for(int y=0; y<rmax_y_store; ++y)
    {
      for(int x=0; x<rmax_x_store; ++x)
      {
        if((calibrated_seed_set.contains((x-column_start)+rmax_x*(y-row_start))) ||
            random_seed_set.contains((x-column_start)+rmax_x*(y-row_start)) ||
            random_seed_set_2.contains((x-column_start)+rmax_x*(y-row_start)) ||
            random_seed_set_3.contains((x-column_start)+rmax_x*(y-row_start)))
        {
          ++combined_raster_size;
        }
      }
    }
    if(camera_seed_size > combined_raster_size) larger_area = camera_seed_size;	//calculate similarity value and score
    else larger_area = combined_raster_size;
    //area_percent = (variability_score*redundancy_score+1.0*score)/larger_area;	        //ORIGINAL~~~
    area_percent = (10*variability_score*redundancy_score+1.0*score)/larger_area;	        //TEST~~~	07_27b
    
    if(area_percent < shift_score)
    {
      shift_score = area_percent;
      shift_tot = rand_x+rand_y+shift_tot;
      shift_tot_2 = rand_x_2+rand_y_2+shift_tot_2;
      shift_tot_3 = rand_x_3+rand_y_3+shift_tot_3;
      shift_tot_4 = rand_x_4+rand_y_4+shift_tot_4;
    }
    if(shift_score < best_score) best_score = shift_score;
    if(area_percent < current_best_score)
    {
      current_best_score = shift_score;	//ORIGINAL~~~
      //current_best_score = area_percent;	//TEST~~~
    }
//------------------------------------------------------------------------------------------------------------------//    
    ++loop_iterations;			//INCREMENT NUMBER OF RASTERS USED
    
    if(num_seeds_tried >= 40 && loop_iterations == 50)			//condition for quadruple raster	//TEST~~~
    {
      seed_number_overlap_test = 4;
      //target_score = 0.24 + variability_score;
      target_score = 0.21 + variability_score;	//TEST~~~
      loop_iterations = 0;
      ++num_seeds_tried;
      //if(num_seeds_tried > 100) best_score = 0.11 + variability_score;	//ORIGINAL
      if(num_seeds_tried > 60) best_score = 0;	//TEST~~~
      rand_raster = rand() % index_zero_positions_list.size();
      rand_raster_2 = rand() % index_zero_positions_list.size();
      rand_raster_3 = rand() % index_zero_positions_list.size();
	  
      cout<<"area_percent: "<<area_percent<<"	best_score: "<<best_score<<"	target_score: "<<target_score<<endl;	//OMIT~~~
      if(num_seeds_tried <= 60) best_score = 1;	//TEST~~~
    }
    //else if(num_seeds_tried >= 25 && loop_iterations == 50)	//condition for triple raster
    //else if(num_seeds_tried >= 15 && loop_iterations == 50)	//condition for triple raster		//Latest~~~
    else if(num_seeds_tried >= 20 && loop_iterations == 50)	//condition for triple raster		//TEST~~~
    {
      seed_number_overlap_test = 3;
      //target_score = 0.16 + variability_score;
      target_score = 0.18 + variability_score;	//TEST~~~
      loop_iterations = 0;
      ++num_seeds_tried;
      rand_raster = rand() % index_zero_positions_list.size();
      rand_raster_2 = rand() % index_zero_positions_list.size();
	  
      cout<<"area_percent: "<<area_percent<<"	best_score: "<<best_score<<"	target_score: "<<target_score<<endl;	//OMIT~~~
      best_score = 1;	//TEST~~~
    }
    else if(loop_iterations >= 50 && num_seeds_tried >= 1)	//condition for double raster
    {
      seed_number_overlap_test = 2;
      //target_score = 0.13 + variability_score;
      target_score = 0.15 + variability_score;	//TEST~~~
      loop_iterations = 0;
      ++num_seeds_tried;
      rand_raster = rand() % index_zero_positions_list.size();
	  
      cout<<"area_percent: "<<area_percent<<"	best_score: "<<best_score<<"	target_score: "<<target_score<<endl;	//OMIT~~~ 
      best_score = 1;	//TEST~~~
    }
//================================reverting function for oversensitivity on single rasters==========================//
	/*if(num_seeds_tried == 2 && loop_iterations == 0) cout<<endl<<"NUM2 LOOP0"<<endl;		//OMIT~~~
	if(num_seeds_tried == 21 && loop_iterations == 0) cout<<endl<<"NUM21 LOOP0"<<endl;		//OMIT~~~
	if(num_seeds_tried == 41 && loop_iterations == 0) cout<<endl<<"NUM41 LOOP0"<<endl;		//OMIT~~~
	
	if((num_seeds_tried == 41) || (num_seeds_tried == 21) || (num_seeds_tried == 2))	//TEST~~~
	{
	  if(num_seeds_tried == 41 && loop_iterations == 0)		//TEST~~~
	  {
		cout<<endl<<"ASDASD 41"<<endl;	//OMIT~~~
	    if(previous_best_score < current_best_score)
	    {
		  best_score = 0;
		  if(seed_number_overlap_test > 1) --seed_number_overlap_test;
		  cout<<"overlap: "<<seed_number_overlap_test<<endl;
          cin>>loop_counter;										//OMIT~~~
	    }
	    else
	    {
	      previous_best_score = current_best_score;
	      current_best_score = 1;
	    }
	  }
	  else if(num_seeds_tried == 21 && loop_iterations == 0)	//TEST~~~
	  {
		cout<<endl<<"ASDASD 21"<<endl;	//OMIT~~~
	    if(previous_best_score < current_best_score)
	    {
		  best_score = 0;
		  if(seed_number_overlap_test > 1) --seed_number_overlap_test;
		  cout<<"overlap: "<<seed_number_overlap_test<<endl;
		  cin>>loop_counter;										//OMIT~~~
	    }	
	    else
	    {
	      previous_best_score = current_best_score;
	      current_best_score = 1;
	    }
	  }
	  else if(num_seeds_tried == 2 && loop_iterations == 0)		//TEST~~~
	  {
		cout<<endl<<"ASDASD 2"<<endl;	//OMIT~~~
	    if(previous_best_score < current_best_score)
	    {
		  best_score = 0;
		  if(seed_number_overlap_test > 1) --seed_number_overlap_test;
		  cout<<"overlap: "<<seed_number_overlap_test<<endl;
		  cin>>loop_counter;										//OMIT~~~
	    }
	    else
	    {
	      previous_best_score = current_best_score;
	      current_best_score = 1;
	    }
	  }
	}*/
//==================================================================================================================//
    if(best_score < target_score)
    {
      print_resized_rasters();	//Visualization for resized overlap comparison images
      cout<<"============================================================"<<endl;
      cout<<"best similarity: "<<best_score<<"		best score: "<<best_score*larger_area<<endl;
      cout<<"Current Similarity: "<<area_percent<<"	Score: "<<score<<endl;
      cout<<"Shift_score: "<<shift_score<<endl;
      cout<<"# of seeds in blob: "<<seed_number_overlap_test<<endl;
      cout<<endl;																				//OMIT~~~
      cout<<"camera_seed_size: "<<camera_seed_size<<"		H: "<<height<<"	W: "<<width<<endl;	//OMIT~~~
      cout<<"best similarity: "<<best_score<<"		best score: "<<score<<endl;					      //OMIT~~~
      
      //int loop_counter;     //OMIT~~~//
      //cin >> loop_counter;  //OMIT~~~//
    }
  }
//==================================================Diagnostics============================================================================//
  //print_resized_rasters();	//Visualization for resized overlap comparison images
  
  cout<<"seed_number_overlap_test: "<<seed_number_overlap_test<<"	best_score: "<<best_score<<endl;				      //OMIT~~~
  cout<<"prev.: "<<previous_best_score<<"	curr.: "<<current_best_score<<"	Target_Score: "<<target_score<<endl;	//OMIT~~~
  cout<<"============================================================"<<endl;
//=========================================================================================================================================//
}
bool secondary_processor::read_calibration_data_file()
{
  char calibrated_crop_name[30];
  char user_crop_request[30];
  char dummy[61];					      //TEST~~~ for the purpose of concatenating string names (access user defined file name)
  int crop_name_compare = 0;		//TEST~~~ //>0 means strings are different
  int file_to_calibration;
  int loop_counter;					    //OMIT~~~
  bool find_loop = 1;
  bool return_bool; //TEST~~~

//-----------------------------------------------------------------------------------//	TEST~~~
  for(int i=0; i<30; ++i)						          //initialize empty array
  {
    user_crop_request[i]='a';
  }
  cout<<endl<<"Input crop file name: "<<endl;	//request user input
  cin>>user_crop_request;
//-----------------------------------------------------------------------------------//	TEST~~~
  dummy[0] = '\0';	//initialize
  strcat(dummy,"datasets/");
  strcat(dummy, user_crop_request);
//-----------------------------------------------------------------------------------//	TEST~~~
  ifstream raster_h("datasets/crop_diagnostic_data");	//open crop_diagnostic_data file
  
  if(raster_h)
  {
    while(!raster_h.eof() && find_loop == 1)	        //should remove .eof() from loop
    {	  
      for(int i=0; i<30; ++i)					                //initialize empty array
      {
        calibrated_crop_name[i]='a';
      }
      raster_h>>calibrated_crop_name;
      raster_h>>largest_dimension;
      raster_h>>average_calibration_seed_size;
      raster_h>>height_mean;
      raster_h>>width_mean;
      raster_h>>area_mean;
      raster_h>>std_deviation_height;
      raster_h>>std_deviation_width;
      raster_h>>std_deviation_area;
      
      crop_name_compare = 0;
      for(int j=0; j<30; ++j)
      {
        if(user_crop_request[j] != calibrated_crop_name[j]) ++crop_name_compare;
      }
      if(crop_name_compare == 0)
      {
        variability_score = (std_deviation_area/area_mean + std_deviation_height/height_mean + std_deviation_width/width_mean)/10;
        //variability_score = 0.065;	//OMIT~~~ (Test for using with canola)
        cout<<endl<<"...diagnostic data retrieved... "<<"("<<variability_score<<")"<<endl;
        find_loop = 0;
        raster_h.clear();
        raster_h.close();
      }
    }
  }
  if(find_loop == 1) cout<<endl<<"...MISSING: raster diagnostic data... "<<"	("<<user_crop_request<<")"<<endl<<endl;
  
  ifstream raster_f(dummy);
  if(raster_f)
  {
    //raster_f>>calibrated_crop_name;
    index_list.clear();
    while(!raster_f.eof())
    {
      raster_f>>file_to_calibration;
      index_list.append(file_to_calibration);
    }
    raster_f.clear();
    raster_f.close();
    cout<<"...retrieved image rasters... "<<"		("<<index_list.size()<<")"<<endl;
  }
//--------------------------------------------------------------------------------------------------------------//
  int previous_zero_position = 0;
  
  for(int q=0; q<index_list.size(); ++q)
  {
    previous_zero_position = index_list.indexOf(0,q-1);
    if(index_list.indexOf(0,q) > previous_zero_position)
    {
      index_zero_positions_list.append(index_list.lastIndexOf(0,q));
    }
  }
  if(raster_f) cout<<"...total rasters retrieved ... "<<"		("<<index_zero_positions_list.size()<<")"<<endl;
//--------------------------------------------------------------------------------------------------------------//
  if(raster_f && raster_h && find_loop==0)
  {
    cout<<endl<<"Retrieved calibration data for: "<<calibrated_crop_name<<endl;
    cout<<endl<<calibrated_crop_name<<endl<<largest_dimension<<" "<<average_calibration_seed_size<<endl<<height_mean<<" "		  //OMIT~~~
        <<width_mean<<" "<<area_mean<<endl<<std_deviation_height<<" "<<std_deviation_width<<" "<<std_deviation_area<<endl;		//OMIT~~~
    cin>>loop_counter;																											//OMIT~~~
    //return 1;
    return_bool = 1;
  }
  if(!raster_f || !raster_h || find_loop==1)
  {
    cout<<"Warning: missing or corrupted calibration data files."<<endl;
    cin>>loop_counter;	//OMIT~~~
    //return 0;
    return_bool = 0;
  }
  return return_bool;
}

bool secondary_processor::write_calibration_data_file() 
{
  char calibrated_crop_name[30];
  cout<<endl<<"No existing calibration data file found. Creating new file."<<endl
            <<"Enter crop name for calibration data file: "<<endl;
  cin>>calibrated_crop_name;
  cout<<endl<<"Writing calibration data for: "<<calibrated_crop_name<<endl;

  char dummy[61];
  dummy[0] = '\0';	//initialize
  strcat(dummy,"datasets/");
  strcat(dummy, calibrated_crop_name);
//----------------------------------------------------------------------------------------------------------------------------------------------// 
  {
    std::ofstream raster_f(dummy,std::ofstream::out);	//test~~ create a new save file, index_list  
    for(int g=0; g<index_list.size(); ++g)
    {
      raster_f<<index_list[g]<<" ";
    }
    raster_f.close();
    raster_f.clear();
  }
  cout<<"...Creating image library for	"<<calibrated_crop_name<<" ..."<<endl;
//----------------------------------------------------------------------------------------------------------------------------------------------// 
  {
    std::ofstream raster_f("datasets/crop_diagnostic_data",(std::ofstream::out|std::ofstream::app));	//create a new save file or appends if crop_diagnostic_data exists
    raster_f<<endl<<calibrated_crop_name<<endl
        <<largest_dimension<<" "<<average_calibration_seed_size<<endl
        <<width_mean<<" "<<height_mean<<" "<<area_mean<<endl
        <<std_deviation_width<<" "<<std_deviation_height<<" "<<std_deviation_area<<endl;

    raster_f.close();
    raster_f.clear();  
  }
  cout<<"...diagnostic data appended to file..."<<endl;
//----------------------------------------------------------------------------------------------------------------------------------------------// 
  return 1;
}

void secondary_processor::calculate_threshold_values()
{
  int num_rasters;
  width_sum = 0;    //defined in global, calculated in store_raw_image_rasters()
  height_sum = 0;
  area_sum = 0;
  float variance_width = 0;
  float variance_height = 0;
  float variance_area = 0;  
  num_rasters  = raw_image_size_list.size();
  
//------------------------------------------------sum calculations-------------------------------------------//
  for(int q=0; q<raw_image_zeroes_list.size(); q+=3)
  {
    width_sum = raw_image_zeroes_list[q+1] + width_sum;
    height_sum = raw_image_zeroes_list[q+2] + height_sum;
  }
  for(int r=0; r<raw_image_size_list.size(); ++r)
  {
    area_sum = raw_image_size_list[r] + area_sum;	  
  }
  width_mean = width_sum/num_rasters;
  height_mean = height_sum/num_rasters;
  area_mean = area_sum/num_rasters;
//---------------------------------------------variance calculations-----------------------------------------//
  for(int i=0; i<raw_image_zeroes_list.size(); i+=3)
  {
    variance_width = qPow((raw_image_zeroes_list[i+1] - width_mean), 2) + variance_width;
    variance_height = qPow((raw_image_zeroes_list[i+2] - height_mean), 2) + variance_height;
  }
  variance_width = variance_width/num_rasters;
  variance_height = variance_height/num_rasters;
  
  for(int r=0; r<raw_image_size_list.size(); ++r)
  {
    variance_area = qPow((raw_image_size_list[r] - area_mean), 2) + variance_area;
  }
  variance_area = variance_area/num_rasters;
//----------------------------------------standard deviatione calculations-----------------------------------//
  std_deviation_width = qSqrt(variance_width);
  std_deviation_height = qSqrt(variance_height);
  std_deviation_area = qSqrt(variance_area);
//--------------------------------------------variability calculation----------------------------------------//
  variability_score = (std_deviation_area/area_mean + std_deviation_height/height_mean + std_deviation_width/width_mean)/10;
  cout<<"variability_score: "<<variability_score<<endl;	//OMIT~~~
  
  cout<<"\033[1;31m";                                                                                                         //OMIT~~~//
  cout<<"std_dev_area: "<<std_deviation_area<<"\tarea_mean: "<<area_mean<<"\tstd_dev_height: "<<std_deviation_height<<endl    //OMIT~~~//
      <<"height_mean: "<<height_mean<<"\tstd_dev_width: "<<std_deviation_width<<"\twidth_mean: "<<width_mean<<endl;           //OMIT~~~//
  cout<<"\033[0m";                                                                                                            //OMIT~~~//
}

//===========================================================================================================================================//
void secondary_processor::print_rotated_rasters()
{
  for (int y=rmax_y-1; y>=0; --y)
  {
    for (int x=0; x<rmax_x; ++x)
    {
      //cout<<display_rotated_raster_p[x + rmax_x*y]<<" ";
    }
    cout<<endl;
  }
}

void secondary_processor::print_resized_rasters()
{
  for (int y=rmax_y_store-1; y>=0; --y)
  {
    for (int x=0; x<rmax_x_store; ++x)
    {
      cout<<display_rotated_raster_p[x + rmax_x_store*y]<<" ";
    }
    cout<<endl;
  }
}
//===========================================================================================================================================//
