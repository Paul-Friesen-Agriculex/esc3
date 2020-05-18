#include "processor.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <QString>
#include <QPainter>
#include "centre.hpp"
#include <sstream>
#include <time.h>
#include "processor_calcs.hpp"
#include </usr/include/SK91USB3.h>

//#include "secondary_processor.hpp"  //TEST~~~ 11_21_2018//

#include <ctime>          //for srand function
#include <QSet>     	    //Quicker lookup speed than QList
#include <QList>          //
#include <QtMath>         //Trig., Sqrt., Pow functions
#include <QtAlgorithms>	  //for list qSort function
#include <stdio.h>	      //for using eof in filestream

using namespace std;

extern const int image_lines;   //defined in centre.cpp
extern const int line_length;   //defined in centre.cpp
extern const int images_to_record;
int valid_start;
int valid_end;//ignore pixels outside valid range

//=================================================================================//
//------------- variables used in calculating standard deviation ------------ //
float width_mean, height_mean, area_mean;								              //TEST~~~
float std_deviation_width, std_deviation_height, std_deviation_area;	//TEST~~~
float variability_score;												                      //TEST~~~

//int seed_number_overlap; //TEST~~~

//----------------- display array -------------------//
//rotated_raster* display_rotated_raster_p = new rotated_raster;
rotated_raster* rotated_raster_p = new rotated_raster;
//-------------------rotated_raster_list-indexes-------------------//
QList<int> index_list;	//save raster index values for first 50 seeds
QList<int> index_zero_positions_list;	//save zero positions denoting end of raster

QList<int> raw_image_list;			  //TEST~~~
QList<int> raw_image_zeroes_list;	//TEST~~~
QList<int> raw_image_size_list;		//TEST~~~

//------------------- initialize starting position values -------------------//
int number_listed_rasters = 0;

float largest_dimension = 0;              //TEST~~~~	//Should be this
//float average_calibration_seed_size = 0;	//TEST~~~
static const int calibration_seed_number = 50;		//------------------------------- used to define the number of seeds to used for calibration data

//=================================================================================//

processor::processor()
{
//  valid_start = 130;
//  valid_end = 1920;//ignore pixels outside valid range
  
  int result;
  if(SK_LOADDLL() != SK_RESULT_OK)
  {
	  std::cout << "SK_LOADDLL failure" << std::endl;
	  exit(1);
  }
  
  result = SK_INITCAMERA(0);
  if (result != SK_RESULT_OK)
  {
	  std::cout <<"SK_INIT_CAMERA failure "<<result<< std::endl;
	  exit(2);
  }
  if (SK_GETBITDEPTH(0) != 8) SK_SETBITDEPTH(0,8);
  
  if (SK_GETLINEFREQUENCY(0) < 4.5)
  {
	  SK_SETLINEFREQUENCY(0, 4.6);
  }
  
  image_size = line_length * image_lines;
  image_p = 0;
  
  cout<<"before shadecorr\n";
  SK_SHADCORR(0, false);
  SK_SETSCM(0, NULL, 8, line_length, image_lines, 10, 80);
//  SK_SETSCM(0, NULL, 8, line_length, 8, 10, 80);
  cout<<"after shadecorr\n";
  
  line_count = 0;
  took_slice_index = -1;//indicates no blob has taken slice
  show_blob_bool = false;
  for(calibration_area_list_index = 0; calibration_area_list_index<50; ++calibration_area_list_index)
  {
    calibration_area_list[calibration_area_list_index] = 0;
    calibration_max_inflection_1_list[calibration_area_list_index] = -1000;
    calibration_max_inflection_3_list[calibration_area_list_index] = -1000;
    calibration_max_inflection_9_list[calibration_area_list_index] = -1000;
  }
  calibration_area_list_index = 0;
  
  big_image_size = images_to_record * image_lines * line_length;
  big_image_lines = images_to_record * image_lines;
  big_image_start_p = new unsigned char[big_image_size];
  rbi_start_p = new unsigned char[big_image_size];
  big_image_last_line_p = big_image_start_p + big_image_size - line_length;//points to beginning of last line
  big_image_end_p = big_image_start_p + big_image_size;//points to one position beyond end of image
  big_image_position_p = big_image_end_recording_p = big_image_start_p;
  for(long i=0; i<big_image_size; ++i)
  {
    *big_image_position_p = 0;
    ++big_image_position_p;
  }
  big_image_position_p = big_image_start_p;
  record_this_image = false;
  play_back = false;
  
  //testing
  t.start();
  run_number = 0;
  max_image_processing_time = 0;
  min_slack_time = 100000;
  first_cycle = true;//processing time and cycle time invalid until set false
  
  
  if(SK_GRABCONTINUOUS(0, image_lines, 5000, false, 512, 0, SK_FREERUN) != SK_RESULT_OK)
  {
    std::cout << "SK_GRAB_CONTINUOUS failure" << std::endl;
    exit(1);
  }
  
  
  
  //We need to determine the part of the camera's field that is useable, and store values in valid_start and valid_end.
  //We take one image from the camera and determine average readings for all pixels.
  //This is used to set valid_start and valid_end such that dark ends of the field are ignored.
  while(1)
  {
    result = SK_GETIMAGE(0, &image_p, 1000);
    if (result == SK_RESULT_IMAGE_COMPLETED)
    {
      break;
    }
    else
    {
      cout<<"SK_GETIMAGE did not complete\n";
      if (result != SK_RESULT_OPERATION_PENDING)
      {
        std::cout << "SK_GETIMAGE failure "<<result<< std::endl;
      }
    }
  }
  int average_pixel_values[line_length];
  unsigned char* position_p = image_p;
  for (int i=0; i<line_length; ++i) average_pixel_values[i] = 0;
  for (int j=0; j<image_lines; ++j)
  {
    for (int i=0; i<line_length; ++i)
    {
      average_pixel_values[i] += *position_p;
      ++ position_p;
    }
  }
  for (int i=0; i<line_length; ++i)
  {
	average_pixel_values[i] /= image_lines;
	
//	cout<<"average_pixel_values["<<i<<"] = "<<average_pixel_values[i]<<endl;
	
  }
  int dark_start_end = 0;//this will become the end of the dark area at the start of the line
  for(int i=0; i<500; ++i)
  {
	if(average_pixel_values[i] < 150) dark_start_end = i;
  }
  int dark_end_start = line_length;//this will become the start of the dark area at the end of the line
  for(int i=line_length; i>(line_length-500); --i)
  {
	if(average_pixel_values[i] < 150) dark_end_start = i;
  }
  valid_start = dark_start_end + 10;
  valid_end = dark_end_start - 10;
  
  cout<<"valid_start = "<<valid_start<<endl;
  cout<<"valid_end = "<<valid_end<<endl;
		  

  
  
  
  
  
  

}

processor::~processor()
{
  SK_STOPGRABBING(0);
  SK_CLOSECAMERA(0);
  SK_UNLOADDLL();
  
  delete [] big_image_start_p;
  
  delete [] rbi_start_p;  
  
  cout<<"end ~processor\n";
}

void processor::increment_count(int increment)
{
  emit count_out(increment);
}

float processor::dust_streak_percentage()//returns percentage of width in which inflections will not be counted due to dust streaks
{
  int in_streak_count = 0;
  for(int i=valid_start; i<valid_end; ++i)
  {
    if (dust_streak_list[i].in_streak_zone)
    {
      ++in_streak_count;
    }
  }
  return float(in_streak_count) / float(valid_end-valid_start) * 100.0;
}

void processor::set_camera_processing(bool state)
{
  if(state == true)
  {
    play_back = false;
    run(); //will enter loop which lasts until play_back set to true
  }
  else
  {
    play_back = true; //will halt loop
    //create re-ordered big image.
    unsigned char* rbi_position_p = rbi_start_p;
    //big_image_position_p is at start of oldest recorded line (left there by new_image).  
    for(int i=0; i<big_image_size; ++i)
    {
      *rbi_position_p = *big_image_position_p;
      ++big_image_position_p;
      if(big_image_position_p >= big_image_end_p) big_image_position_p = big_image_start_p;
      ++rbi_position_p;
    }
    qimage_sense_line = 0;
  }
}

void processor::playback_line()//advance 1 line in playback
{
  if(qimage_sense_line<big_image_lines)
  {
    unsigned char* rbi_position_p;
    rbi_position_p = rbi_start_p + qimage_sense_line*line_length;
    if( (rbi_position_p<rbi_start_p) || (rbi_position_p>rbi_start_p+(big_image_size-line_length)) )
    {
      cout<<"processor::playback_line.  rbi_position_p out of bounds\n";
      exit(1);
    }
    add_line(rbi_position_p);
    ++qimage_sense_line;
  }
  else
  {
    end_of_playback();
  }
}

void processor::get_qimage()
{
  QImage qimage(line_length, lines_to_display, QImage::Format_RGB32);
  qimage_top_line = qimage_sense_line + lines_above_sense_line;
  for(int i=0; i<lines_to_display; ++i)
  {
    int current_line = qimage_top_line-i;
    for(int j=0; j<line_length; ++j)
    {
      unsigned char brt;
      if(current_line<0 || current_line>=big_image_lines) brt = 255;
      else
      {
        unsigned char* pixel_p = rbi_start_p + (current_line*line_length) + j;
        brt = *pixel_p;
      }
      qimage.setPixel(j, i, qRgb(brt,brt,brt));
    }
  }
  QPainter painter(&qimage);
  QPen pen;
  pen.setColor(QColor(255,0,0));
  pen.setWidth(5);
  painter.setPen(pen);
  qimage_count_line = lines_above_sense_line+dust_streak_line_allowance;
  painter.drawLine(0, lines_above_sense_line, line_length, lines_above_sense_line);
  painter.drawLine(0, qimage_count_line, line_length, qimage_count_line);
  painter.drawLine(valid_start, 0, valid_start, lines_to_display);
  painter.drawLine(valid_end, 0, valid_end, lines_to_display);
  
  emit send_qimage(qimage);
}
  
void processor::set_to_record(bool record_now)//true - turn on.  false - turn off
{
  if(record_now) 
  {
    record_this_image = true;
  }
  else
  {
    record_this_image = false;
  }
}

void processor::set_crop(crop current_crop_set)//this slot runs every time centre::run() runs to keep processor crop current with centre.
{
  current_crop = current_crop_set;


  if(current_crop.calibrated == false)//when calibrating, we wish to register all inflections
  {
    current_crop.max_inflection_1 = -1000;
    current_crop.max_inflection_3 = -1000;
    current_crop.max_inflection_9 = -1000;
    current_crop.sensitivity = 1;//minimum value -> most sensitive
  }



  detect_size = 3 * pow(current_crop.sensitivity, 2);
  
//  cout<<"end processor::set_crop\n";
  
}

void processor::show_blob_on_terminal(bool show_blob_bool_set)
{
  show_blob_bool = show_blob_bool_set;
}

void processor::restart_calibration()//re-starts data collection for calibration
{
  
  cout<<"processor::restart_calibration\n";
  
  calibration_crop.calibrated = false;
  for(calibration_area_list_index = 0; calibration_area_list_index<50; ++calibration_area_list_index)
  {
    calibration_area_list[calibration_area_list_index] = 0;
    calibration_max_inflection_1_list[calibration_area_list_index] = -1000;
    calibration_max_inflection_3_list[calibration_area_list_index] = -1000;
    calibration_max_inflection_9_list[calibration_area_list_index] = -1000;
  }
  calibration_area_list_index = 0;
  show_blob_bool = true;
//  calibration_crop = centre_p->crops[0];
  
//  emit send_calibrated_crop(calibration_crop);//will set crops[0] in centre.
  
//  cout<<"current_crop.calibrated="<<current_crop.calibrated<<endl;
}

void processor::save_image(QString file_name)
{
  unsigned char* current_pixel_p = rbi_start_p;
  QByteArray bytearray;
  bytearray = file_name.toLatin1();
  const char* const_char_p = (const char*)bytearray;
  ofstream ofile(const_char_p);
  for (int line = 0; line < (image_lines*images_to_record); ++line)
  {
    for(int i=0;i<line_length;++i)
    {
      ofile << (*current_pixel_p);
      ++current_pixel_p;
    }
  }
  ofile.close();
}

void processor::load_image(QString file_name)
{
  unsigned char* rbi_position_p;
  QByteArray bytearray;
  bytearray = (file_name).toLatin1();
  const char* const_char_p = (const char*)bytearray;

  cout<<"loading "<<const_char_p<<endl;
  
  ifstream infile(const_char_p);
  
  if(!infile.is_open())
  {
    cout<<"file not open\n";
  }
  
  rbi_position_p = rbi_start_p;
  unsigned char pixel;
  
  for (int line_num=0; line_num<(image_lines*images_to_record); ++line_num)
  {
    for (int pix_num=0; pix_num<line_length; ++pix_num)
    {
      infile >>noskipws>> pixel;
      *rbi_position_p = pixel;
      ++rbi_position_p;
    }
  }
  infile.close();
}

void processor::reset_time_tests()
{
  cout<<"processor::reset_time_tests()\n";
  max_image_processing_time = 0;
  min_slack_time = 100000;
}

void processor::calibrate()//does the calibration after data has been collected
{
  calibration_crop.name = current_crop.name;
  
//  cout<<"processor::calibrate.  current_crop.name "<<(current_crop.name).toStdString()<<endl;
  
  int area_sum = 0;
  for(int i=0; i<50; ++i)
  {
    area_sum += calibration_area_list[i];
  }
  int area_average = area_sum/50; //This is an initial estimate, but the data could contain a few small
    //blobs representing dust & debris.  Use this to set sensitivity,
    //but eliminate any small blobs for other calculations.
  calibration_crop.sensitivity = pow(area_average/15, 0.5);
//  detect_size = 5 * pow(calibration_crop.sensitivity, 2);
  detect_size = 3 * pow(calibration_crop.sensitivity, 2);
  int blob_count = 0; //to count blobs with area > detect_size
  area_sum = 0;//calculate average again, leaving out small blobs
  for(int i=0; i<50; ++i)
  {
    if(calibration_area_list[i] > detect_size)
    {
      ++ blob_count;
      area_sum += calibration_area_list[i];
    }
  }
  
  if(blob_count < 30)
  {
    cout<<"warning processor::calibrate.  Low blob_count = "<<blob_count<<"\n";
    if(blob_count<5) return;
  }
  
  area_average = area_sum/blob_count;//more accurate measure
  
//  cout<<"calibration blob_count="<<blob_count<<endl;
  
  int area_squared_deviations_sum = 0;
  float max_inflection_1 = -1000;
  float max_inflection_3 = -1000;
  float max_inflection_9 = -1000;
  for(int i=0; i<50; ++i)
  {
    if(calibration_area_list[i] > detect_size)
    {
      area_squared_deviations_sum += pow(calibration_area_list[i]-area_average, 2);
      if(max_inflection_1 < calibration_max_inflection_1_list[i]) max_inflection_1 = calibration_max_inflection_1_list[i];
      if(max_inflection_3 < calibration_max_inflection_3_list[i]) max_inflection_3 = calibration_max_inflection_3_list[i];
      if(max_inflection_9 < calibration_max_inflection_9_list[i]) max_inflection_9 = calibration_max_inflection_9_list[i];
    }
  }
  calibration_crop.gate_setting = calibration_crop.sensitivity;
  calibration_crop.area_mean = area_average;
  calibration_crop.area_stdev = pow(area_squared_deviations_sum/(blob_count-1), 0.5);
  calibration_crop.max_inflection_1 = max_inflection_1;
  calibration_crop.max_inflection_3 = max_inflection_3;
  calibration_crop.max_inflection_9 = max_inflection_9;
  
//  cout<<"end of processor::calibrate.  calibration_crop.name = "<<calibration_crop.name.toStdString()<<".  calibration_crop.calibrated = "<<calibration_crop.calibrated<<endl;
}

void processor::new_image(unsigned char* pixel_p)
{
  slack_time = t.restart();
  unsigned char* small_image_position_p = pixel_p;
  for (int line = 0; line < image_lines; ++line)
  {
    add_line(small_image_position_p);
    small_image_position_p += line_length;
  }
  
  image_processing_time = t.restart();
  if(first_cycle==false)//slack_time and image_processing_time are now valid
  {
    if(slack_time<min_slack_time) min_slack_time = slack_time;
    if(image_processing_time>max_image_processing_time) max_image_processing_time = image_processing_time;
  }
  image_cycle_time = slack_time + image_processing_time;
  emit send_cycle_time(image_cycle_time);
  ++run_number;
  if(run_number >= 5)
  {
    run_number = 0;
    QString str = QString("Max image processing %1 ms.  Min slack %2 ms.")
    .arg(max_image_processing_time).arg(min_slack_time);
    emit send_message(str);
    //cout<<(str.toStdString())<<endl;
    first_cycle = false;
  }

  if (record_this_image)
  {
    unsigned char* small_image_position_p = pixel_p;
    for(int i=0; i<image_size; ++i)
    {
      *big_image_position_p = *small_image_position_p;
      ++big_image_position_p;
      if(big_image_position_p >= big_image_end_p)
      {
        big_image_position_p = big_image_start_p;
      }
      ++small_image_position_p;
    }
  }
}

void processor::add_line(unsigned char* start_p)
{
  ++ line_count;
  int i;
  int blob_index;
  int pixel_value;
  unsigned char* position_p = start_p;
  bool in_blob = false;
  for(blob_index=0; blob_index<blob_list.size(); ++blob_index)
  {
    blob_list[blob_index]->took_slice = false;
  }
  for(i=0; i<line_length; ++i)
  {
    if(i<valid_start || i>valid_end) pixel_value = 200;//blank out
    else pixel_value = int(*position_p);
    if(in_blob == false)
    {
      if(pixel_value < 100)
      {
        start_slice = i;
        in_blob = true;
      }
    }
    else //in blob.  Look for end of blob
    {
      if(pixel_value >= 100)
      {
        end_slice = i-1;
        in_blob = false;
        took_slice_index = -1;//no blob has taken this slice
        for(blob_index=0; blob_index<blob_list.size(); ++blob_index)
        {
          if(blob_list[blob_index]->offer(start_slice, end_slice) == true) 
          {
            took_slice_index=blob_index;
          }
        }
        if(took_slice_index == -1)//this slice was not taken by any blob.  Start new blob.
        {
          blob* blob_p = new blob(start_slice, end_slice, this);
          blob_list.append(blob_p);
        }
      }
    }
    dust_streak_list[i].add_line(pixel_value);
    ++position_p;
  }
  for(blob_index=0; blob_index<blob_list.size(); ++blob_index)
  {
    if (blob_list[blob_index]->remove)//indicates this blob is to be removed because it combined with another blob
    {
      delete blob_list[blob_index];
      blob_list.removeAt(blob_index);
    }
    else 
    {
      if(blob_list[blob_index]->finish_line == -1)//signals that blob is not ready to finish
      {
        if (blob_list[blob_index]->took_slice == false) //this blob got no slice on this line.  It is complete.
        {
          blob_list[blob_index]->last_line = line_count - 1;
          if(blob_list[blob_index]->area > detect_size)
          {
            blob_list[blob_index]->form_raster_image();
            blob_list[blob_index]->fill_end_line_point_list();
            blob_list[blob_index]->form_outline();
            blob_list[blob_index]->finish_line = line_count + dust_streak_line_allowance;//to allow dust treaks to be detected before finishing
            // from playback.cpp,   int delayed_lines_above_cout_line = lines_above_count_line + 110;
          }
          else//too small to count
          {
            delete blob_list[blob_index];
            blob_list.removeAt(blob_index);
          }
        }
      }
    }
  }
  for(blob_index=0; blob_index<blob_list.size(); ++blob_index)//look for blobs that need to be finished
  {
    
//    cout<<"blob list loop.  finish_line="<<blob_list[blob_index]->finish_line<<".  blob_index="<<blob_index<<"\n";
    
    if(blob_list[blob_index]->finish_line!=-1 && blob_list[blob_index]->finish_line<=line_count)
    {
      blob_list[blob_index]->calculate_characteristics();
      int blob_seed_count;
  
//      cout<<"add_line point 1.  current_crop.calibrated="<<current_crop.calibrated<<".  calibration_crop.calibrated="<<calibration_crop.calibrated<<endl;
  
      if(current_crop.calibrated==false)
      {
        calibration_area_list[calibration_area_list_index] = blob_list[blob_index]->area;
        calibration_max_inflection_1_list[calibration_area_list_index] = blob_list[blob_index]->max_inflection_1;
        calibration_max_inflection_3_list[calibration_area_list_index] = blob_list[blob_index]->max_inflection_3;
        calibration_max_inflection_9_list[calibration_area_list_index] = blob_list[blob_index]->max_inflection_9;
        blob_seed_count = 1;
        ++calibration_area_list_index;
        
//        cout<<"  calibration_area_list_index = "<<calibration_area_list_index<<endl;
        
        if(calibration_area_list_index == 50) 
        {
  
//          cout<<"add_line point 2.  current_crop.calibrated="<<current_crop.calibrated<<".  calibration_crop.calibrated="<<calibration_crop.calibrated<<endl;
  
          calibrate();
          calibration_area_list_index = 0;
          calibration_crop.calibrated = true;
          
//          cout<<"in processor::add_line.  calibration_crop.name = "<<calibration_crop.name.toStdString()<<".  calibration_crop.calibrated = "<<calibration_crop.calibrated<<endl;
          
          emit send_calibrated_crop(calibration_crop);
//          current_crop = calibration_crop;
        }
      }
      else //already calibrated
      {
        blob_seed_count = blob_list[blob_index]->seeds_in_blob();
        calibration_crop = current_crop;//this does not happen if crop is being calibrated, so calibration crop data can be different
  
//        cout<<"add_line point 3.  current_crop.calibrated="<<current_crop.calibrated<<".  calibration_crop.calibrated="<<calibration_crop.calibrated<<endl;
  



/*
      if(show_blob_bool)
      //if(show_blob_bool && blob_seed_count>1) //TEST~~~ //to disable and re-enable seed raster generation
      {
        cout<<blob_seed_count <<" seeds in blob\n";
        cout<<"  area = "<<blob_list[blob_index]->area<<endl;
        cout<<"  detect_size "<<detect_size<<endl;
        cout<<"  max_inflection = "<<blob_list[blob_index]->max_inflection<<endl;
        cout<<"  inflection_count = "<<blob_list[blob_index]->inflection_count<<endl;
        
        blob_list[blob_index]->form_display_raster();
        cout<<endl;
        //------------------------------------------------------------------------------------//
        //functions for finding centroids
        //blob_list[blob_index]->kmeans_clustering(blob_seed_count);     //TEST~~~
        //blob_list[blob_index]->kmeans_clustering(4);     //TEST~~~
        
        //------------------------------------------------------------------------------------//        
        blob_list[blob_index]->print_display_raster();        //original image

//===========================================================================================================================// //TEST~~~        
        
        //secondary_processor_p->width = (blob_list[blob_index] -> width);
        //secondary_processor_p->height = (blob_list[blob_index] -> height);
        //secondary_processor_p->test_variables();                          //TEST~~~
        
        //secondary_processor_p->test_cout((blob_list[blob_index] -> width), (blob_list[blob_index] -> height), blob_seed_count);        //TEST~~~
        //secondary_processor_p->send_raster(blob_list.size(), ); //TEST~~~
        //secondary_processor_p->send_raster(1, 2);               //TEST~~~
        //secondary_processor_p->send_raster(1, 2, blob_list[blob_index] -> display_raster_p); //TEST~~~
        
        //secondary_processor_p->send_raster(&(blob_list[blob_index]->form_display_raster()));
        
        //secondary_processor_p->overlap_test((blob_list[blob_index] -> width), (blob_list[blob_index] -> height));          //TEST~~~
        //secondary_processor_p->print_display_raster((blob_list[blob_index] -> width), (blob_list[blob_index] -> height));  //TEST~~~
        //secondary_processor_p->test_cout(1, 2);                                                                            //TEST~~~
//===========================================================================================================================// //TEST~~~
        //blob_list[blob_index]->kmeans_clustering(blob_seed_count);     //ORIGINAL~~~
        //blob_list[blob_index]->overlap_test(blob_seed_count);          //TEST~~~
        //blob_list[blob_index]->print_display_raster();                 //TEST~~~  //modified image
        //------------------------------------------------------------------------------------//
//=====================================================================================================// //TEST~~~
        calibration_data = false; //temporary
        
        //if(calibration_data) blob_list[blob_index]->store_raw_image_rasters();
        
        //if(number_listed_rasters >= calibration_seed_number) blob_list[blob_index]->raster_comparison();  //TEST~~~
        //if(number_listed_rasters >= calibration_seed_number)
        //{
          //blob_list[blob_index]->raster_comparison(blob_seed_count);           //TEST~~~
          //blob_list[blob_index]->overlap_test(blob_seed_count);                //TEST~~~   //Single Cluster TEST
          
          //blob_list[blob_index]->overlap_test_multiple(blob_seed_count);       //TEST~~~  //Multiple Cluster TEST
          //blob_list[blob_index]->overlap_test_multiple(3);                     //TEST~~~  //Multiple Cluster TEST
          
          //blob_list[blob_index]->silhouette_scoring();
        //}
        
        //else if(!calibration_data && blob_seed_count < 2) blob_list[blob_index]->store_raw_image_rasters(); 
            //  1.store_raw_image_rasters()
            //  2.store_rotated_rasters()
            //  3.remove_similar_rasters()




        if(blob_seed_count < 2) blob_list[blob_index]->store_raw_image_rasters(); 	//TEST~~	(more than 50 seeds)
          //blob_list[blob_index]->store_rotated_rasters();    //toggle rotation functions 
        }
        


        
//=====================================================================================================// //TEST~~~
        //------------------------------------------------------------------------------------//
        cout<<endl<<endl<<endl;
      }
      */
      }
      
      
      increment_count(blob_seed_count);
    
      delete blob_list[blob_index];
      blob_list.removeAt(blob_index);
    }
  }
  
  //dust streak detection
  for(int i=0; i<line_length; ++i)
  {
    dust_streak_list[i].in_streak_zone = false;//blank out all dust streak zones before re-generating
  }
  dust_streak_present = false;
  for(int i=0; i<line_length; ++i)
  {
    if(dust_streak_list[i].confirmed == true)
    {
      int streak_start = i-8;
      if(streak_start < 0) streak_start = 0;
      int streak_end = i+8;
      if(streak_end >= line_length) streak_end = line_length;
      for(int j=streak_start; j<streak_end; ++j)
      {
        dust_streak_list[j].in_streak_zone = true;
      }
      dust_streak_present = true;
    }
  }
}

void processor::run()
{
  
  
  if(play_back == false)//count using images from camera.  one execution processes one camera image
  {
    int result;
    while(1)
    {
      result = SK_GETIMAGE(0, &image_p, 1000);
      if (result == SK_RESULT_IMAGE_COMPLETED)
      {
        break;
      }
      else
      {
        cout<<"SK_GETIMAGE did not complete\n";
        if (result != SK_RESULT_OPERATION_PENDING)
        {
          std::cout << "SK_GETIMAGE failure "<<result<< std::endl;
        }
      }
    }
    new_image(image_p);
    QTimer::singleShot(5, this, SLOT(run()));
  }
}

blob::blob(int start, int end, processor* set_processor_p)
{
  processor_p = set_processor_p;
  area = end-start;
  took_slice = true;
  remove = false;
  bool_raster_p=0;
  display_raster_p=0;
  first_line = last_line = processor_p->line_count;
  min_x = start;
  max_x = end;
  slice* slice_p = new slice;
  slice_p->start = start;
  slice_p->end = end;
  slice_p->line = processor_p->line_count;
  slice_list.append(slice_p);
  calibrating = !processor_p->current_crop.calibrated;
  inflection_count = 0;
  max_inflection_1 = -1000;
  max_inflection_3 = -1000;
  max_inflection_9 = -1000;
  finish_line = -1;//signal not to finish yet
  
  average_calibration_seed_size = 0;  //TEST~~~//
}

blob::~blob()
{
  if(bool_raster_p!=0) 
  {
    delete [] bool_raster_p;
  }
  bool_raster_p=0;
  if(display_raster_p!=0)
  {
    delete [] display_raster_p;
    display_raster_p = 0;
  }
  for (int i=0; i<slice_list.size(); ++i)
  {
    delete slice_list[i];
    slice_list[i] = 0;
  }
  for (int i=0; i<outline_point_list.size(); ++i)
  {
    delete outline_point_list[i];
    outline_point_list[i] = 0;
  }
}

bool blob::offer(int slice_start, int slice_end)
{
  if (remove == true) //true indicates that this blob is to be removed.  Do not accept slices
  {
    return false;
  }
  if(finish_line != -1) //this indicates that the blob is complete and waiting to finish.  do not accept slices
  {
    return false;
  }
  long long previous_line = processor_p->line_count-1;
  for(int i=0; i<slice_list.size(); ++i)
  {
    if(slice_list[i]->line == previous_line)
    {
      if(slice_start<slice_list[i]->end && slice_end>slice_list[i]->start) //take slice for this blob if not already taken
      {
        if(processor_p->took_slice_index != -1)//another blob already took this slice.  combine two blobs
        {
          combine(processor_p->took_slice_index);
          took_slice = false;
          return false;//slice not accepted because already taken
        }
        else
        {
          area += (slice_end-slice_start);
          if(slice_start<min_x) min_x=slice_start;
          if(slice_end>max_x) max_x=slice_end;
          slice* slice_p = new slice;
          slice_p->start = slice_start;
          slice_p->end = slice_end;
          slice_p->line = processor_p->line_count;
          slice_list.append(slice_p);
          took_slice = true;
          return true;//slice is accepted
        }
      }
    }
  }
  return false;//if function gets to here without returning, slice is not accepted
}

void blob::combine(int combine_index)//combine this blob with another blob and mark this one for removal.
{
  (processor_p->blob_list[combine_index]->area) += area;
  remove = true;//signals that this blob is to be removed.  This happens in processor::add_line()
  processor_p->blob_list[combine_index]->slice_list.append(slice_list);
  slice_list.clear();
  processor_p->blob_list[combine_index]->min_x = qMin(min_x, processor_p->blob_list[combine_index]->min_x);
  processor_p->blob_list[combine_index]->max_x = qMax(max_x, processor_p->blob_list[combine_index]->max_x);
  processor_p->blob_list[combine_index]->first_line = qMin(first_line, processor_p->blob_list[combine_index]->first_line);
  processor_p->blob_list[combine_index]->last_line = processor_p->line_count;
}

void blob::form_raster_image()
{
  width = max_x-min_x+3;
  height = last_line-first_line+3;
  bool_raster_p = new bool[width*height];
  for(int y=0; y<height; ++y)
  {
    for (int x=0; x<width; ++x)
    {
      bool_raster_p[x+y*width] = false;
    }
  }
  for(int i=0; i<slice_list.size(); ++i)
  {
    int y = slice_list[i]->line-first_line+1; //y value of slice in bool_raster
    if (y<0 || y>=height) {cout<<"raster image y out of bounds.  y="<<y<<"   height="<<height<<"\n"; exit(1);}
    int x = slice_list[i]->start-min_x+1;//x value of beginning of slice in bool_raster
    for(int j=slice_list[i]->start; j<=slice_list[i]->end; ++j)
    {
      if (x<0 || x>=width) {cout<<"raster image x out of bounds\n"; exit(1);}
      bool_raster_p[x+y*width] = true;
      ++x;
    }
  }
}

void blob::fill_end_line_point_list()
{
  bool last_pixel = false;
  for(el_y=0; el_y<height; ++el_y)
  {
    for(el_x=0; el_x<width; ++el_x)
    {
      if( (last_pixel==false) && (bool_raster_p[el_x+el_y*width]==true) )//current (el_x, el_y) is left end of a line
      {
        end_line_point* end_line_point_p = new end_line_point;
        end_line_point_p -> x = el_x;
        end_line_point_p -> y = el_y;
        end_line_point_list.append(end_line_point_p);
      }
      else if( (last_pixel==true) && (bool_raster_p[el_x+el_y*width]==false) )//previous (el_x, el_y) is right end of a line
      {
        end_line_point* end_line_point_p = new end_line_point;
        end_line_point_p -> x = el_x-1;
        end_line_point_p -> y = el_y;
        end_line_point_list.append(end_line_point_p);
      }
      last_pixel = bool_raster_p[el_x+el_y*width];
    }
  }
}

void blob::form_outline()
{
  ol_x=1;
  ol_y=1;
  while(bool_raster_p[ol_x+ol_y*width] == false)
  {
    ++ol_x;
    if(ol_x>=width)//not expected
    {
      cout<<"failed to find blob first line\n";
      exit(1);
    }
  }
  ol_angle = 0;
  //(ol_x,ol_y) now at left end of bottom line of blob image
  
  int point_count = 0;//used to check if this does not terminate after a reasonable number of outline points are found.  Should terminate after the algorithm gets back to its starting point after following around the outline.
  int point_count_max = 3*(height+width);//this is the reasonable number.  possibly could be low for an extremely intricate object.
  while(1)//loop until break
  {
    //check if the current tracing position is in the end_line_point_list.  we only want to keep such points
    for(int i=0; i<end_line_point_list.size(); ++i)
    {
      if( (end_line_point_list[i]->x==ol_x) && (end_line_point_list[i]->y==ol_y) )
      {
        outline_point* outline_point_p = new outline_point;
        outline_point_p->x = ol_x;
        outline_point_p->y = ol_y;
        outline_point_list.append(outline_point_p);//direction, distance, and inflection information to be filled later
        break;
      }
    }
    switch(ol_angle)
    {
      case 0:
        ol_try_start(270); break;
      case 45:
        ol_try_start(315); break;
      case 90:
        ol_try_start(0); break;
      case 135:
        ol_try_start(45); break;
      case 180:
        ol_try_start(90); break;
      case 225:
        ol_try_start(135); break;
      case 270:
        ol_try_start(180); break;
      case 315:
        ol_try_start(225); break;
      default:
        cout<<"ol_angle not found\n";
        exit(1);
    }
    if(ol_x == outline_point_list[0]->x && ol_y == outline_point_list[0]->y) //trace is complete
    {
      break;//from while(1)
    }
    ++point_count;
    if(point_count>point_count_max)//not expected
    {
      cout<<"blob's point_count_max exceeded\n";
      exit(1);
    }
  }
  opl_size = outline_point_list.size();
  
  //fill in distance and direction information
  ol_length = 0;
  for(int i=0; i<opl_size; ++i)
  {
    int next_index = i+1;
    if(next_index>=opl_size) next_index-=opl_size;
    float d = distance(outline_point_list[i]->x, outline_point_list[i]->y, outline_point_list[next_index]->x, outline_point_list[next_index]->y);
    ol_length += d;
    outline_point_list[i]->distance = d;
    outline_point_list[i]->direction = anglexy(    outline_point_list[next_index]->x - outline_point_list[i]->x    ,   outline_point_list[next_index]->y - outline_point_list[i]->y    );
  }
/*
  //fill in inflection_0.  This is the inflection of the two lines that meet at the point
  for(int i=0; i<opl_size; ++i)
  {
    int previous_index = opl_decrement(i);
    if(outline_point_list[i]->direction<-500 || outline_point_list[previous_index]->direction<-500)//invalid angle signaled by direction value -1000.  See processor_calcs.hpp.
    {
      outline_point_list[i]->inflection_0 = -1000;//invalid inflection
    }
    else
    {
      float inflection_temp = (outline_point_list[previous_index]->direction) - (outline_point_list[i]->direction);
      if(inflection_temp<-M_PI) inflection_temp += (2.0*M_PI);
      if(inflection_temp>M_PI) inflection_temp -= (2.0*M_PI);
      outline_point_list[i] -> inflection_0 = inflection_temp;
    }
  }
*/




 
  





/*  
  //fill in other inflections
  //arm lengths to calculate inflections
  float d1 = ol_length*.01;
  float d3 = ol_length*.03;
  float d9 = ol_length*.09;
  for(int i=0; i<opl_size; ++i)
  {
    int forward_i = i;
    int back_i = opl_decrement(i);
    float ti1=0;//temporary inflection.  These will be added to to create final values.
    float ti3=0;
    float ti9=0;
    float dist_total = 0;
    bool got_arm_end_1 = false;
    bool got_arm_end_3 = false;
    bool got_arm_end_9 = false;
    bool inflection_invalid = false;//will be set to true if invalid angle encountered
    while(1)//loop until break.  move forward on curve, incrementing or decrementing start inflections until longest arm length reached
    {
      dist_total += outline_point_list[forward_i]->distance;
      if(outline_point_list[forward_i]->inflection_0 < -500) inflection_invalid = true;
      if(got_arm_end_1==false)
      {
        if(dist_total>=d1) got_arm_end_1=true;
        ti1 += outline_point_list[forward_i]->inflection_0;
      }
      if(got_arm_end_3==false)
      {
        if(dist_total>=d3) got_arm_end_3=true;
        ti3 += outline_point_list[forward_i]->inflection_0;
      }
      if(got_arm_end_9==false)
      {
        if(dist_total>=d9) got_arm_end_9=true;
        ti9 += outline_point_list[forward_i]->inflection_0;
      }
      if(got_arm_end_9==true) break;
      forward_i = opl_increment(forward_i);
    }

    //repeat process, but move backward on curve, incrementing or decrementing start inflections until longest arm length reached
    dist_total = 0;
    got_arm_end_1 = false;
    got_arm_end_3 = false;
    got_arm_end_9 = false;
    while(1)//loop until break.  
    {
      dist_total += outline_point_list[back_i]->distance;
      if(outline_point_list[back_i]->inflection_0 < -500) inflection_invalid = true;
      if(got_arm_end_1==false)
      {
        if(dist_total>=d1) got_arm_end_1=true;
        else ti1 += outline_point_list[back_i]->inflection_0;
      }
      if(got_arm_end_3==false)
      {
        if(dist_total>=d3) got_arm_end_3=true;
        else ti3 += outline_point_list[back_i]->inflection_0;
      }
      if(got_arm_end_9==false)
      {
        if(dist_total>=d9) got_arm_end_9=true;
        else ti9 += outline_point_list[back_i]->inflection_0;
      }
      if(got_arm_end_9==true) break;
      back_i = opl_decrement(back_i);
    }
    if(inflection_invalid==true)
    {
      outline_point_list[i]->inflection_1 = -1000;
      outline_point_list[i]->inflection_3 = -1000;
      outline_point_list[i]->inflection_9 = -1000;
    }
    else
    {
      outline_point_list[i]->inflection_1 = ti1;
      outline_point_list[i]->inflection_3 = ti3;
      outline_point_list[i]->inflection_9 = ti9;
    }
  }
*/  
  
  
  
  
  
}

void blob::ol_try_start(int start_angle)//start_angle >= 0, <360.  45 degree steps.
{
  int angle=start_angle;
  int try_count=0;
  while(1)
  {
    ol_shift_point(angle);
    if(bool_raster_p[ol_try_x+ol_try_y*width] == true)
    {
      ol_x=ol_try_x;
      ol_y=ol_try_y;
      ol_angle=angle;
      return;
    }
    angle += 45;
    if(angle>=360) angle -= 360;
    ++try_count;
    if(try_count>8)//not expected
    {
      cout<<"failed to find blob edge\n";
      exit(1);
    }
  }
}
  
void blob::ol_shift_point(int angle)//sets ol_try_x and ol_try_y one unit from ol_x and ol_y at indicated angle
{
  switch (angle)
  {
    case 0:
      ol_try_x = ol_x + 1;
      ol_try_y = ol_y;
      break;
    case 45:
      ol_try_x = ol_x+1;
      ol_try_y = ol_y+1;
      break;
    case 90:
      ol_try_x = ol_x;
      ol_try_y = ol_y+1;
      break;
    case 135:
      ol_try_x = ol_x-1;
      ol_try_y = ol_y+1;
      break;
    case 180:
      ol_try_x = ol_x-1;
      ol_try_y = ol_y;
      break;
    case 225:
      ol_try_x = ol_x-1;
      ol_try_y = ol_y-1;
      break;
    case 270:
      ol_try_x = ol_x;
      ol_try_y = ol_y-1;
      break;
    case 315:
      ol_try_x = ol_x+1;
      ol_try_y = ol_y-1;
      break;
    default:
      cout<<"shift_point angle not found\n";
      exit (1);
  }
}

int blob::opl_increment(int start)
{
  int end = start+1;
  if(end>=opl_size) end -= opl_size;
  return end;
}

int blob::opl_decrement(int start)
{
  int end = start-1;
  if(end<0) end += opl_size;
  return end;
}

void blob::remove_inflection(int index)
{
  for(int i=index; i<inflection_count; ++i)
  {
    inflection_i[i] = inflection_i[i+1];
    inflection_x[i] = inflection_x[i+1];
    inflection_y[i] = inflection_y[i+1];
  }
  --inflection_count;
}


void blob::inflection_calc(int index)
{  
  //arm lengths to calculate inflections
  float d1 = ol_length*.01;
  float d3 = ol_length*.03;
  float d9 = ol_length*.09;
  int forward_i = index;
  int back_i = opl_decrement(index);
  float dist_total = 0;
  bool got_arm_end_1 = false;
  bool got_arm_end_3 = false;
  bool got_arm_end_9 = false;
  //co-ordinates of forward and backward arm ends at 3 distances.  (distances measured along curve)
  int xf1=0, yf1=0, xb1=0, yb1=0;
  int xf3=0, yf3=0, xb3=0, yb3=0;
  int xf9=0, yf9=0, xb9=0, yb9=0;
  while(1)//loop until break.  move forward on curve until longest arm length reached
  {
    dist_total += outline_point_list[forward_i]->distance;
    if(got_arm_end_1==false)
    {
      if(dist_total>=d1) 
      {
        got_arm_end_1=true;
        int next_i = opl_increment(forward_i);
        xf1 = outline_point_list[next_i]->x;
        yf1 = outline_point_list[next_i]->y;
      }
    }
    if(got_arm_end_3==false)
    {
      if(dist_total>=d3)
      {
        got_arm_end_3=true;
        int next_i = opl_increment(forward_i);
        xf3 = outline_point_list[next_i]->x;
        yf3 = outline_point_list[next_i]->y;
      }
    }
    if(got_arm_end_9==false)
    {
      if(dist_total>=d9)
      {
        got_arm_end_9=true;
        int next_i = opl_increment(forward_i);
        xf9 = outline_point_list[next_i]->x;
        yf9 = outline_point_list[next_i]->y;
      }
    }
    if(got_arm_end_9==true) break;
    forward_i = opl_increment(forward_i);
  }

  //repeat process, but move backward on curve, incrementing or decrementing start inflections until longest arm length reached
  dist_total = 0;
  got_arm_end_1 = false;
  got_arm_end_3 = false;
  got_arm_end_9 = false;
  while(1)//loop until break.  
  {
    dist_total += outline_point_list[back_i]->distance;
    if(got_arm_end_1==false)
    {
      if(dist_total>=d1) 
      {
        got_arm_end_1=true;
        xb1 = outline_point_list[back_i]->x;
        yb1 = outline_point_list[back_i]->y;
      }
    }
    if(got_arm_end_3==false)
    {
      if(dist_total>=d3)
      {
        got_arm_end_3=true;
        xb3 = outline_point_list[back_i]->x;
        yb3 = outline_point_list[back_i]->y;
      }
    }
    if(got_arm_end_9==false)
    {
      if(dist_total>=d9)
      {
        got_arm_end_9=true;
        xb9 = outline_point_list[back_i]->x;
        yb9 = outline_point_list[back_i]->y;
      }
    }
    if(got_arm_end_9==true) break;
    back_i = opl_decrement(back_i);
  }
  
  int x = outline_point_list[index] -> x;
  int y = outline_point_list[index] -> y;
  
  float direction_before_1 = anglexy(x-xb1, y-yb1);
  float direction_after_1 = anglexy(xf1-x, yf1-y);
  float inflection_temp = (direction_before_1 - direction_after_1);
  if(inflection_temp<-M_PI) inflection_temp += (2.0*M_PI);
  if(inflection_temp>M_PI) inflection_temp -= (2.0*M_PI);
  inflection_1_out = inflection_temp;
  
  float direction_before_3 = anglexy(x-xb3, y-yb3);
  float direction_after_3 = anglexy(xf3-x, yf3-y);
  inflection_temp = (direction_before_3 - direction_after_3);
  if(inflection_temp<-M_PI) inflection_temp += (2.0*M_PI);
  if(inflection_temp>M_PI) inflection_temp -= (2.0*M_PI);
  inflection_3_out = inflection_temp;
  
  float direction_before_9 = anglexy(x-xb9, y-yb9);
  float direction_after_9 = anglexy(xf9-x, yf9-y);
  inflection_temp = (direction_before_9 - direction_after_9);
  if(inflection_temp<-M_PI) inflection_temp += (2.0*M_PI);
  if(inflection_temp>M_PI) inflection_temp -= (2.0*M_PI);
  inflection_9_out = inflection_temp;
  
}

void blob::calculate_characteristics()
{
  if(calibrating)
  {
    for(int i=0; i<opl_size; ++i)
    {
      inflection_calc(i);//stores inflection values in inflection_1_out etc.
      if(inflection_1_out > max_inflection_1)  max_inflection_1 = inflection_1_out;
      if(inflection_3_out > max_inflection_3)  max_inflection_3 = inflection_3_out;
      if(inflection_9_out > max_inflection_9)  max_inflection_9 = inflection_9_out;
    }
  }
  else //not calibrating
  {
    
    /*
    if(processor_p->show_blob_bool)
    {
      cout<<"\n\n\ncalculate characteristics\n";
    }
    */
    
    
    
    float crop_max_inflection_1 = processor_p->current_crop.max_inflection_1;
    float crop_max_inflection_3 = processor_p->current_crop.max_inflection_3;
    float crop_max_inflection_9 = processor_p->current_crop.max_inflection_9;
    max_inflection_1 = -1000;
    max_inflection_3 = -1000;
    max_inflection_9 = -1000;
    //fill in inflection_scores in outline_point_list
    for(int i=0; i<opl_size; ++i)
    {
      inflection_calc(i);//stores inflection values in inflection_1_out etc.
      if(inflection_1_out > max_inflection_1)  max_inflection_1 = inflection_1_out;
      if(inflection_3_out > max_inflection_3)  max_inflection_3 = inflection_3_out;
      if(inflection_9_out > max_inflection_9)  max_inflection_9 = inflection_9_out;
      float i_score_1 = inflection_1_out - crop_max_inflection_1;
      if(i_score_1<0) i_score_1=0;
      float i_score_3 = inflection_3_out - crop_max_inflection_3;
      if(i_score_3<0) i_score_3=0;
      float i_score_9 = inflection_9_out - crop_max_inflection_9;
      if(i_score_9<0) i_score_9=0;
      float inflection_score = 0.1*i_score_1 + 0.5*i_score_3 + i_score_9;
      outline_point_list[i] -> inflection_score = inflection_score;



/*
      if(processor_p->show_blob_bool)
      {
        cout
        <<setw(20)<<i
        <<setw(20)<<std::fixed<<std::setprecision(2)<<crop_max_inflection_1
        <<setw(20)<<std::fixed<<std::setprecision(2)<<inflection_1_out
        <<setw(20)<<std::fixed<<std::setprecision(2)<<crop_max_inflection_3
        <<setw(20)<<std::fixed<<std::setprecision(2)<<inflection_3_out
        <<setw(20)<<std::fixed<<std::setprecision(2)<<crop_max_inflection_9
        <<setw(20)<<std::fixed<<std::setprecision(2)<<inflection_9_out
        <<setw(20)<<std::fixed<<std::setprecision(2)<<inflection_score
        <<endl;
      }
*/






    }
    
    float inflection_peak = 0;
    inflection_count = 0;
    float inflection;
    int current_inflection_i=0;//index in outline point list of newly found inflection peak.
    int i=0;//outline_point_list subscript
    int j=0;//loop counter.  may exceed outline_point_list size.
    
    while(1)
    {
      inflection = outline_point_list[i]->inflection_score;
      if((inflection>0) && (inflection>inflection_peak)) 
      {
        inflection_peak = inflection;
        if(inflection_count<100)
        //inflection_x and inflection_y for current inflection_count change with each outline_point until inflection_count is incremented
        {
          inflection_x[inflection_count] = outline_point_list[i]->x;
          inflection_y[inflection_count] = outline_point_list[i]->y;
          inflection_i[inflection_count] = i;
          current_inflection_i = i;
        }
      }
      if((inflection_peak>0) && (inflection<=0))//inflection peak has passed.  process and prepare for next peak
      //inflection_x and inflection_y for current inflection_count will now remain at last values recorded
      {
        //ignore inflection if it is in a dust streak zone
        int inflection_x_global = inflection_x[inflection_count] + min_x - 1;//inflection_x is co-ordinate in the bool_raster.  We need the co-ordinate in the entire image width.
        if( (processor_p->dust_streak_list[inflection_x_global].in_streak_zone==false)
          && (outline_point_list[current_inflection_i]->inflection_score > 0.3)
          && (inflection_count<100) )//if there is a dust streak zone at this point, or if its score is too low, do not increment inflection_count.  
          //this allows inflection values to over-write this inflection next time inflection is increasing
        {
          ++inflection_count;//note index of inflection in list is 1 less than inflection count
        }
        
        inflection_peak = 0;
  
      }
      ++i;
      ++j;
      if(i>=opl_size) i=0;
      if(j>=opl_size)//possibly might need to go beyond end (i.e. back to the beginning) to finish inflection peak
      {
        if(inflection<=0) break;//from while(1) loop.  any inflection peak has been processed.
        if(j>2*opl_size)//not expected
        {
          cout<<"in blob::calculate_characteristics, j exceeded expected bounds.\n";
          cout<<"j="<<j<<".  ol_length="<<ol_length<<".  inflection="<<inflection<<".  inflection_count="<<inflection_count<<endl;
          exit(1);
        }
      }
    }
    
    //look for inflections that are too close together.  if too close, eliminate one.
    float minimum_inflection_spacing = ol_length/20.0;
    for(int i=0; i<inflection_count; ++i)
    {
      float spacing = 0;
      int opl_index = inflection_i[i];
      while(1)
      {
        spacing += outline_point_list[opl_index]->distance;
        opl_index = opl_increment(opl_index);
        for(int j=0; j<inflection_count; ++j)
        {
          if(opl_index==inflection_i[j])
          {
            remove_inflection(j);
          }
        }
        if(spacing>minimum_inflection_spacing) break;
      }
    }
  }
}
  
int blob::seeds_in_blob()
{
  float area_n = processor_p->current_crop.area_mean;
  float area_sd = processor_p->current_crop.area_stdev;
  float mi1_n = processor_p->current_crop.max_inflection_1;
  float mi3_n = processor_p->current_crop.max_inflection_3;
  float mi9_n = processor_p->current_crop.max_inflection_9;
  
  for(int i=0;i<max_seeds;++i)
  {
    score_area[i] = 0;
    score_mi1[i] = 0;
    score_mi3[i] = 0;
    score_mi9[i] = 0;
    score_ic[i] = 0;
  }
  
  float ic_estimate = float(inflection_count)/2.0 +1.0;
  float ic_uncertainty = 1.0;
  for(int i=1;i<max_seeds;++i)//start at i=1 seed.  0 position in matrix is not used.
  {
    expected_mean_area[i] = area_n + .8*(i-1)*area_n; //factor .8 is to allow for seeds overlapping, which will reduce area of entire blob
    expected_variance_area[i] = float(i) * area_sd * area_sd;
    float f = pow(2.0*3.14*expected_variance_area[i], -0.5) * exp(-pow(float(area)-expected_mean_area[i], 2.0)/2.0/expected_variance_area[i]);//height of normal dist
//    score_area[i] = f * expected_mean_area[i] * 6.0;
    score_area[i] = -0.002/(f*expected_mean_area[i]+.0001);
    score_ic[i] = 120.0/((ic_estimate-float(i))*(ic_estimate-float(i))+1)/(1+ic_uncertainty);
    score_likelihood[i] = -15*i;
  }

//  score_mi1[1] = 2 * (mi1_n - max_inflection_1);//negative if single seed is unlikely
//  score_mi3[1] = 8 * (mi3_n - max_inflection_3);//negative if single seed is unlikely
//  score_mi9[1] = 32 * (mi9_n - max_inflection_9);//negative if single seed is unlikely

  score_mi1[1] = 2 * (mi1_n*1.2 - max_inflection_1);//negative if single seed is unlikely
  score_mi3[1] = 8 * (mi3_n*1.2 - max_inflection_3);//negative if single seed is unlikely
  score_mi9[1] = 32 * (mi9_n*1.2 - max_inflection_9);//negative if single seed is unlikely

  int most_likely_number = 0;
  int max_score = -1000;
  for(int i=1;i<max_seeds;++i)//start at i=1 seed.  0 position in matrix is not used.
  {
    int score = score_area[i]+score_ic[i]+score_mi1[i]+score_mi3[i]+score_mi9[i]+score_likelihood[i];
    if(score>max_score)
    {
      max_score = score;
      most_likely_number = i;
    }
  }
  
  if(processor_p -> show_blob_bool)
//  if(processor_p->show_blob_bool && most_likely_number>1)

  {
    cout<<"\n\n\nseeds_in_blob\n";

    cout<<"  "<<most_likely_number<<" most_likely_number\n";
    cout<<"  area = "<<area<<endl;
    cout<<"  detect_size "<<processor_p->get_detect_size()<<endl;
    cout<<"  max_inflection_1 = "<<max_inflection_1<<endl;
    cout<<"  max_inflection_3 = "<<max_inflection_3<<endl;
    cout<<"  max_inflection_9 = "<<max_inflection_9<<endl;
    
    form_display_raster();
    cout<<endl;
    print_display_raster();



    cout<< "\n              number  expected_mean_area   expected_variance          score_area            score_mi1           score_mi3           score_mi9           score_ic    score_likelihood         total score\n";
    cout<<setprecision(4);
    for(int i=1;i<max_seeds;++i)
    {
      cout<<setw(20)<<i<<setw(20)<<expected_mean_area[i]<<setw(20)<<expected_variance_area[i]<<setw(20)<<score_area[i]<<setw(20)<<score_mi1[i]<<setw(20)<<score_mi3[i]<<setw(20)<<score_mi9[i]<<setw(20)<<score_ic[i]<<setw(20)<<score_likelihood[i];
      cout<<setw(20)<<score_area[i]+score_mi1[i]+score_mi3[i]+score_mi9[i]+score_ic[i]+score_likelihood[i]<<endl;
    }
    cout<<endl;
    
    /*
    cout<< "outline point list\n";
    cout<<         "                   i                   x                   y           direction          inflection_score\n";
    for(int i=0; i<outline_point_list.size(); ++i)
    {
      cout
      <<setw(20)<<i
      <<setw(20)<<outline_point_list[i]->x
      <<setw(20)<<outline_point_list[i]->y
      <<setw(20)<<outline_point_list[i]->direction
      <<setw(20)<<std::fixed<<std::setprecision(2)<<outline_point_list[i]->inflection_score
      <<endl;
    }
    cout<<endl;
    */
    
    
  }
  return most_likely_number;
}

void blob::form_display_raster()
{
  display_raster_p = new char[width*height];
  for (int y=0; y<height; ++y)
  {
    for (int x=0; x<width; ++x)
    {
      if(bool_raster_p[x+width*y]) display_raster_p[x+width*y]='-';
      else display_raster_p[x+width*y]=' ';
    }
  } 
  cout<<"\ninflection_count "<<inflection_count<<endl;
  
  for(int i=0; i<inflection_count; ++i)
  {
    int x = inflection_x[i];
    int y = inflection_y[i];
    cout<<"  inflection at ("<<inflection_x[i]<<", "<<inflection_y[i]<<").  score "<<outline_point_list[inflection_i[i]]->inflection_score<<"\n";
    display_raster_p[x+width*y] = 'I';
  





    int index = inflection_i[i];//index of inflection point in outline_point_list

    //arm lengths to calculate inflections
    float d1 = ol_length*.01;
    float d3 = ol_length*.03;
    float d9 = ol_length*.09;
    int forward_i = index;
    int back_i = opl_decrement(index);
    float dist_total = 0;
    bool got_arm_end_1 = false;
    bool got_arm_end_3 = false;
    bool got_arm_end_9 = false;
    //co-ordinates of forward and backward arm ends at 3 distances.  (distances measured along curve)
    int xf1=0, yf1=0, xb1=0, yb1=0;
    int xf3=0, yf3=0, xb3=0, yb3=0;
    int xf9=0, yf9=0, xb9=0, yb9=0;
    while(1)//loop until break.  move forward on curve until longest arm length reached
    {
      dist_total += outline_point_list[forward_i]->distance;
      if(got_arm_end_1==false)
      {
        if(dist_total>=d1) 
        {
          got_arm_end_1=true;
          int next_i = opl_increment(forward_i);
          xf1 = outline_point_list[next_i]->x;
          yf1 = outline_point_list[next_i]->y;
        }
      }
      if(got_arm_end_3==false)
      {
        if(dist_total>=d3)
        {
          got_arm_end_3=true;
          int next_i = opl_increment(forward_i);
          xf3 = outline_point_list[next_i]->x;
          yf3 = outline_point_list[next_i]->y;
        }
      }
      if(got_arm_end_9==false)
      {
        if(dist_total>=d9)
        {
          got_arm_end_9=true;
          int next_i = opl_increment(forward_i);
          xf9 = outline_point_list[next_i]->x;
          yf9 = outline_point_list[next_i]->y;
        }
      }
      if(got_arm_end_9==true) break;
      forward_i = opl_increment(forward_i);
    }
  
    //repeat process, but move backward on curve, incrementing or decrementing start inflections until longest arm length reached
    dist_total = 0;
    got_arm_end_1 = false;
    got_arm_end_3 = false;
    got_arm_end_9 = false;
    while(1)//loop until break.  
    {
      dist_total += outline_point_list[back_i]->distance;
      if(got_arm_end_1==false)
      {
        if(dist_total>=d1) 
        {
          got_arm_end_1=true;
          xb1 = outline_point_list[back_i]->x;
          yb1 = outline_point_list[back_i]->y;
        }
      }
      if(got_arm_end_3==false)
      {
        if(dist_total>=d3)
        {
          got_arm_end_3=true;
          xb3 = outline_point_list[back_i]->x;
          yb3 = outline_point_list[back_i]->y;
        }
      }
      if(got_arm_end_9==false)
      {
        if(dist_total>=d9)
        {
          got_arm_end_9=true;
          xb9 = outline_point_list[back_i]->x;
          yb9 = outline_point_list[back_i]->y;
        }
      }
      if(got_arm_end_9==true) break;
      back_i = opl_decrement(back_i);
    }
  
  
    display_raster_p[xf9+width*yf9] = 'A';
    display_raster_p[xb9+width*yb9] = 'A';
    display_raster_p[xf3+width*yf3] = 'B';
    display_raster_p[xb3+width*yb3] = 'B';
    display_raster_p[xf1+width*yf1] = 'C';
    display_raster_p[xb1+width*yb1] = 'C';
  }






  
}

void blob::print_display_raster()
{
  for (int y=height-1; y>=0; --y)
  {
    for (int x=0; x<width; ++x)
    {
      cout<<display_raster_p[x + width*y];
    }
    cout<<endl;
  }
}
//===============================================================================================================//
void blob::kmeans_clustering(int cluster_num)
{
  //cluster variable initializations//
  int c1_num, c2_num, c3_num, c4_num;
  int c1_sum_x, c2_sum_x, c3_sum_x, c4_sum_x;
  int c1_sum_y, c2_sum_y, c3_sum_y, c4_sum_y;
  
  //define cluster seed starting locations//
  c1_x = 0;         //cluster 1: bottom left
  c1_y = 0;
  c2_x = width;     //cluster 2: top right
  c2_y = height;
  c3_x = width;     //cluster 3: bottom right
  c3_y = 0;
  c4_x = 0;         //cluster 4: top left
  c4_y = height;

  //iterations of k-means clustering//
  for(int q=0; q<10; ++q)
  {
    c1_num = 0;
    c2_num = 0;
    c3_num = 0;
    c4_num = 0;
    c1_sum_x = 0;
    c1_sum_y = 0;
    c2_sum_x = 0;
    c2_sum_y = 0;
    c3_sum_x = 0;
    c3_sum_y = 0;
    c4_sum_x = 0;
    c4_sum_y = 0;
    c1_mean_x = 0;
    c1_mean_y = 0;
    c2_mean_x = 0;
    c2_mean_y = 0;
    c3_mean_x = 0;
    c3_mean_y = 0; 
    c4_mean_x = 0;
    c4_mean_y = 0;
  
    //1. Datapoint Assignment Step//
    for (int y=0; y<height; ++y)
    {
      for (int x=0; x<width; ++x)
      {
        if(bool_raster_p[x+width*y])
        {
          float euclid_1, euclid_2, euclid_3, euclid_4;   //euclidean distance of points from cluster seed locations
          
          if(cluster_num >= 1) euclid_1 = qSqrt(qPow((x-c1_x),2)+qPow((y-c1_y),2));
          else euclid_1 = 10000;
          if(cluster_num >= 2) euclid_2 = qSqrt(qPow((x-c2_x),2)+qPow((y-c2_y),2));
          else euclid_2 = 10000;
          if(cluster_num >= 3) euclid_3 = qSqrt(qPow((x-c3_x),2)+qPow((y-c3_y),2));
          else euclid_3 = 10000;
          if(cluster_num >= 4) euclid_4 = qSqrt(qPow((x-c4_x),2)+qPow((y-c4_y),2));
          else euclid_4 = 10000;
        
          if(euclid_1 < euclid_2 && euclid_1 < euclid_3 && euclid_1 < euclid_4)
          {
            display_raster_p[x+width*y]='1';
            ++c1_num;
            c1_sum_x = c1_sum_x + x;
            c1_sum_y = c1_sum_y + y;
          }
          else if(euclid_2 < euclid_1 && euclid_2 < euclid_3 && euclid_2 < euclid_4)
          {
            display_raster_p[x+width*y]='2';
            ++c2_num;
            c2_sum_x = c2_sum_x + x;
            c2_sum_y = c2_sum_y + y;
          }
          else if(euclid_3 < euclid_1 && euclid_3 < euclid_2 && euclid_3 < euclid_4)
          {
            display_raster_p[x+width*y]='3';
            ++c3_num;
            c3_sum_x = c3_sum_x + x;
            c3_sum_y = c3_sum_y + y;
          }
          else if(euclid_4 < euclid_1 && euclid_4 < euclid_2 && euclid_4 < euclid_3)
          {
            display_raster_p[x+width*y]='4';
            ++c4_num;
            c4_sum_x = c4_sum_x + x;
            c4_sum_y = c4_sum_y + y;
          }
        }
      }
    }
    if(cluster_num >= 1)
    {
      c1_mean_x = c1_sum_x/c1_num;
      c1_mean_y = c1_sum_y/c1_num;
      display_raster_p[c1_mean_x+width*c1_mean_y]='~';      //centroid 1
    }
    if(cluster_num >= 2)
    {
      c2_mean_x = c2_sum_x/c2_num;
      c2_mean_y = c2_sum_y/c2_num;
      display_raster_p[c2_mean_x+width*c2_mean_y]='~';      //centroid 2
    }
    if(cluster_num >= 3)
    {
      c3_mean_x = c3_sum_x/c3_num;
      c3_mean_y = c3_sum_y/c3_num;
      display_raster_p[c3_mean_x+width*c3_mean_y]='~';      //centroid 3
    }
    if(cluster_num >= 4)
    {
      c4_mean_x = c4_sum_x/c4_num;
      c4_mean_y = c4_sum_y/c4_num;
      display_raster_p[c4_mean_x+width*c4_mean_y]='~';      //centroid 4
    }
    //2. Update Centroid//
    c1_x = c1_mean_x;
    c1_y = c1_mean_y;
    c2_x = c2_mean_x;
    c2_y = c2_mean_y;
    c3_x = c3_mean_x;
    c3_y = c3_mean_y;
    c4_x = c4_mean_x;
    c4_y = c4_mean_y;
    
    centroid_x[0] = c1_mean_x;   //store centroids to array//
    centroid_x[1] = c2_mean_x;
    centroid_x[2] = c3_mean_x;
    centroid_x[3] = c4_mean_x;
    centroid_y[0] = c1_mean_y;
    centroid_y[1] = c2_mean_y;
    centroid_y[2] = c3_mean_y;
    centroid_y[3] = c4_mean_y;
  }
  
  if(cluster_num >= 1) cout<<"Estimated seed #1 size: "<<c1_num<<endl;
  if(cluster_num >= 2) cout<<"Estimated seed #2 size: "<<c2_num<<endl;
  if(cluster_num >= 3) cout<<"Estimated seed #3 size: "<<c3_num<<endl;
  if(cluster_num >= 4) cout<<"Estimated seed #4 size: "<<c4_num<<endl;
}
//==================================================================================================================================================//
void blob::silhouette_scoring() //calculates and scores accuracy cluster configurations// -> helps determine number of clusters/seeds are in blob
{
  QList<int> cluster_1_x_list, cluster_2_x_list, cluster_3_x_list, cluster_4_x_list;
  QList<int> cluster_1_y_list, cluster_2_y_list, cluster_3_y_list, cluster_4_y_list;
  
  //simplified to 4 corners of rectangular raster image size//
  c1_x = 0;
  c1_y = 0;
  c2_x = width;
  c2_y = height;
  c3_x = width;
  c3_y = 0;
  c4_x = 0;
  c4_y = height;
  
  cluster_num = 0;
  
  //cluster calculation variables//
  int c1_num, c2_num, c3_num, c4_num;
  int c1_sum_x, c2_sum_x, c3_sum_x, c4_sum_x;
  int c1_sum_y, c2_sum_y, c3_sum_y, c4_sum_y;
  
  for(int p=1; p<5; ++p)  //cluster configuration using 1-4 clusters//
  {
    cluster_num = p;  //TEST~~~
    
    if(cluster_num > 4) cluster_num = 4;
    //cluster_num = 4;
    //if(!(cluster_num>4)) ++cluster_num;
    //else cluster_num = 4;
    //cluster_num = 4;
    
    if(cluster_num >=1)
    {
      cluster_1_x_list.clear();
      cluster_1_y_list.clear();
    }
    if(cluster_num >=2)
    {
      cluster_2_x_list.clear();
      cluster_2_y_list.clear();
    }
    if(cluster_num >=3)
    {
      cluster_3_x_list.clear();
      cluster_3_y_list.clear();
    }
    if(cluster_num >=4)
    {
      cluster_4_x_list.clear();
      cluster_4_y_list.clear();
    }
    cout<<"FLOATING POINT 1"<<endl; //OMIT~~~

    
    //iterations of k-means clustering//
    for(int q=0; q<10; ++q)
    {
      c1_num = 0;
      c2_num = 0;
      c3_num = 0;
      c4_num = 0;
      c1_sum_x = 0;
      c1_sum_y = 0;
      c2_sum_x = 0;
      c2_sum_y = 0;
      c3_sum_x = 0;
      c3_sum_y = 0;
      c4_sum_x = 0;
      c4_sum_y = 0;
      c1_mean_x = 0;
      c1_mean_y = 0;
      c2_mean_x = 0;
      c2_mean_y = 0;
      c3_mean_x = 0;
      c3_mean_y = 0; 
      c4_mean_x = 0;
      c4_mean_y = 0;
  
      for (int y=0; y<height; ++y)
      {
        for (int x=0; x<width; ++x)
        {
          if(bool_raster_p[x+width*y])
          {
            float euclid_1, euclid_2, euclid_3, euclid_4;   //euclidean distance of points from cluster seed locations
         cout<<"FLOATING POINT 2a"<<endl; //OMIT~~~

            if(cluster_num >= 1) euclid_1 = qSqrt(qPow((x-c1_x),2)+qPow((y-c1_y),2));
            else euclid_1 = 10000;
            if(cluster_num >= 2) euclid_2 = qSqrt(qPow((x-c2_x),2)+qPow((y-c2_y),2));
            else euclid_2 = 10000;
            if(cluster_num >= 3) euclid_3 = qSqrt(qPow((x-c3_x),2)+qPow((y-c3_y),2));
            else euclid_3 = 10000;
            if(cluster_num >= 4) euclid_4 = qSqrt(qPow((x-c4_x),2)+qPow((y-c4_y),2));
            else euclid_4 = 10000;
        
        cout<<"FLOATING POINT 2"<<endl; //OMIT~~~
        
            if(euclid_1 < euclid_2 && euclid_1 < euclid_3 && euclid_1 < euclid_4)
            {
              display_raster_p[x+width*y]='1';
              ++c1_num;
              c1_sum_x = c1_sum_x + x;
              c1_sum_y = c1_sum_y + y;
              if(q == 9)                    //TEST~~~
              {
                cluster_1_x_list.append(x); //TEST~~~
                cluster_1_y_list.append(x); //TEST~~~
              }
            }
            else if(euclid_2 < euclid_1 && euclid_2 < euclid_3 && euclid_2 < euclid_4)
            {
              display_raster_p[x+width*y]='2';
              ++c2_num;
              c2_sum_x = c2_sum_x + x;
              c2_sum_y = c2_sum_y + y;
              if(q == 9)                    //TEST~~~
              {
                cluster_2_x_list.append(x); //TEST~~~
                cluster_2_y_list.append(x); //TEST~~~
              }
            }
            else if(euclid_3 < euclid_1 && euclid_3 < euclid_2 && euclid_3 < euclid_4)
            {
              display_raster_p[x+width*y]='3';
              ++c3_num;
              c3_sum_x = c3_sum_x + x;
              c3_sum_y = c3_sum_y + y;
              if(q == 9)                    //TEST~~~
              {
                cluster_3_x_list.append(x); //TEST~~~
                cluster_3_y_list.append(x); //TEST~~~
              }
            }
            else if(euclid_4 < euclid_1 && euclid_4 < euclid_2 && euclid_4 < euclid_3)
            {
              display_raster_p[x+width*y]='4';
              ++c4_num;
              c4_sum_x = c4_sum_x + x;
              c4_sum_y = c4_sum_y + y;
              if(q == 9)                    //TEST~~~
              {
                cluster_4_x_list.append(x); //TEST~~~
                cluster_4_y_list.append(x); //TEST~~~
              }
            }
          }
        }
      }
      cout<<"FLOATING POINT3a"<<endl; //OMIT~~~
      if(cluster_num >= 1)
      {
        c1_mean_x = c1_sum_x/c1_num;
        c1_mean_y = c1_sum_y/c1_num;
        display_raster_p[c1_mean_x+width*c1_mean_y]='~';      //centroid 1
      }
      if(cluster_num >= 2)
      {
        c2_mean_x = c2_sum_x/c2_num;
        c2_mean_y = c2_sum_y/c2_num;
        display_raster_p[c2_mean_x+width*c2_mean_y]='~';      //centroid 2
      }
      if(cluster_num >= 3)
      {
        c3_mean_x = c3_sum_x/c3_num;
        c3_mean_y = c3_sum_y/c3_num;
        display_raster_p[c3_mean_x+width*c3_mean_y]='~';      //centroid 3
      }
      if(cluster_num >= 4)
      {
        //cout<<"FLOATING POINT4a"<<endl; //OMIT~~~
        c4_mean_x = c4_sum_x/c4_num;
        c4_mean_y = c4_sum_y/c4_num;
        display_raster_p[c4_mean_x+width*c4_mean_y]='~';      //centroid 4
        //cout<<"FLOATING POINT4b"<<endl; //OMIT~~~
      }
      
      //Update Centroid//
      c1_x = c1_mean_x;
      c1_y = c1_mean_y;
      c2_x = c2_mean_x;
      c2_y = c2_mean_y;
      c3_x = c3_mean_x;
      c3_y = c3_mean_y;
      c4_x = c4_mean_x;
      c4_y = c4_mean_y;
    }
    cout<<"FLOATING POINT3"<<endl; //OMIT~~~
    //++cluster_num;  //TEST~~~
  }
  cout<<"FLOATING POINT 4"<<endl; //OMIT~~~
  //------------------------------------------------DIAGNOSTICS-TEST------------------------------------------------//
  cout<<endl<<"List Sizes:"<<endl
      <<"\tc1_x: "<<cluster_1_x_list.size()<<"\tc1_y: "<<cluster_1_y_list.size()<<endl
      <<"\tc2_x: "<<cluster_2_x_list.size()<<"\tc2_y: "<<cluster_2_y_list.size()<<endl
      <<"\tc3_x: "<<cluster_3_x_list.size()<<"\tc3_y: "<<cluster_3_y_list.size()<<endl
      <<"\tc4_x: "<<cluster_4_x_list.size()<<"\tc4_y: "<<cluster_4_y_list.size()<<endl;
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------//
  //centroids found -> use centroids as reference to determine silhouette scores//
  /*int c1_delta_sum_x[cluster_num];
  int c1_delta_sum_y[cluster_num];
  int c2_delta_sum_x[cluster_num];
  int c2_delta_sum_y[cluster_num];
  int c3_delta_sum_x[cluster_num];
  int c3_delta_sum_y[cluster_num];
  int c4_delta_sum_x[cluster_num];
  int c4_delta_sum_y[cluster_num];
  
  float c1_delta_mean_x;
  float c1_delta_mean_y;
  float c2_delta_mean_x;
  float c2_delta_mean_y;
  float c3_delta_mean_x;
  float c3_delta_mean_y;
  float c4_delta_mean_x;
  float c4_delta_mean_y;
  
  for(int y=0; y<cluster_num; ++y)  //initialize sum variables to zero//  
  {
    c1_delta_sum_x[y] = 0;
    c1_delta_sum_y[y] = 0;
    c2_delta_sum_x[y] = 0;
    c2_delta_sum_y[y] = 0;
    c3_delta_sum_x[y] = 0;
    c3_delta_sum_y[y] = 0;
    c4_delta_sum_x[y] = 0;
    c4_delta_sum_y[y] = 0;
  }
  
  for(int y=0; y<height; ++y)
  {
    for(int x=0; x<width; ++x)
    {
      if(display_raster_p[x+width*y] == '1')
      {
        c1_delta_sum_x[0] = c1_delta_sum_x[0] + (x-c1_x);
        c1_delta_sum_x[0] = c1_delta_sum_y[0] + (y-c1_y);
        c1_delta_sum_x[1] = c1_delta_sum_x[1] + (x-c2_x);
        c1_delta_sum_x[1] = c1_delta_sum_y[1] + (y-c2_y);
        c1_delta_sum_x[3] = c1_delta_sum_x[2] + (x-c3_x);
        c1_delta_sum_x[3] = c1_delta_sum_y[2] + (y-c3_y);
        c1_delta_sum_x[4] = c1_delta_sum_x[3] + (x-c4_x);
        c1_delta_sum_x[4] = c1_delta_sum_y[3] + (y-c4_y);
      }
      else if(display_raster_p[x+width*y] == '2')
      {
        c2_delta_sum_x[0] = c2_delta_sum_x[0] + (x-c1_x);
        c2_delta_sum_x[0] = c2_delta_sum_y[0] + (y-c1_y);
        c2_delta_sum_x[1] = c2_delta_sum_x[1] + (x-c2_x);
        c2_delta_sum_x[1] = c2_delta_sum_y[1] + (y-c2_y);
        c2_delta_sum_x[3] = c2_delta_sum_x[2] + (x-c3_x);
        c2_delta_sum_x[3] = c2_delta_sum_y[2] + (y-c3_y);
        c2_delta_sum_x[4] = c2_delta_sum_x[3] + (x-c4_x);
        c2_delta_sum_x[4] = c2_delta_sum_y[3] + (y-c4_y);
      }
      else if(display_raster_p[x+width*y] == '3')
      {
        c3_delta_sum_x[0] = c3_delta_sum_x[0] + (x-c1_x);
        c3_delta_sum_x[0] = c3_delta_sum_y[0] + (y-c1_y);
        c3_delta_sum_x[1] = c3_delta_sum_x[1] + (x-c2_x);
        c3_delta_sum_x[1] = c3_delta_sum_y[1] + (y-c2_y);
        c3_delta_sum_x[3] = c3_delta_sum_x[2] + (x-c3_x);
        c3_delta_sum_x[3] = c3_delta_sum_y[2] + (y-c3_y);
        c3_delta_sum_x[4] = c3_delta_sum_x[3] + (x-c4_x);
        c3_delta_sum_x[4] = c3_delta_sum_y[3] + (y-c4_y);
      }
      else if(display_raster_p[x+width*y] == '4')
      {
        c4_delta_sum_x[0] = c4_delta_sum_x[0] + (x-c1_x);
        c4_delta_sum_x[0] = c4_delta_sum_y[0] + (y-c1_y);
        c4_delta_sum_x[1] = c4_delta_sum_x[1] + (x-c2_x);
        c4_delta_sum_x[1] = c4_delta_sum_y[1] + (y-c2_y);
        c4_delta_sum_x[3] = c4_delta_sum_x[2] + (x-c3_x);
        c4_delta_sum_x[3] = c4_delta_sum_y[2] + (y-c3_y);
        c4_delta_sum_x[4] = c4_delta_sum_x[3] + (x-c4_x);
        c4_delta_sum_x[4] = c4_delta_sum_y[3] + (y-c4_y);
      }
    }
  }
  
  for(int z=0; z<cluster_num; ++z)
  {
    cout<<"cluster "<<z+1<<":"<<endl
        <<c1_delta_sum_x[z]<<"\t"<<c2_delta_sum_x[z]<<"\t"<<c3_delta_sum_x[z]<<"\t"<<c4_delta_sum_x[z]<<endl
        <<c1_delta_sum_y[z]<<"\t"<<c2_delta_sum_y[z]<<"\t"<<c3_delta_sum_y[z]<<"\t"<<c4_delta_sum_y[z]<<endl
        <<endl;
  }
  //c1_delta_mean_x = c1_delta_sum_x[1]/c1_num
//-------------------------------------------------------------------------------------------------------------------------------------------------//
  print_display_raster();*/
}
//==================================================================================================================================================//

void::blob::overlap_test_multiple(int cluster_num)  //TEST~~~ 10_22_2018
{
  //seed data variables//
  srand(time(NULL));      //same random number generated each time  //ORIGINAL~~~
  //srand((time(NULL) & 0xFFFF) | (getpid() << 16));      //TEST~~~
  //srand(static_cast<int>(time(0)));
  int random_calibration_seed;
  int seed_list_start_position = 0;
  int seed_list_end_position;
  QList<int> library_raster_seeds; //TEST~~~
  
  //array alternatives//
  int lcx[4] = {0, 0, 0, 0};
  int lcy[4] = {0, 0, 0, 0};

  int library_seed_sizes[4] = {0, 0, 0, 0};
  int library_seed_widths[4] = {0, 0, 0, 0};
  int library_seed_heights[4] = {0, 0, 0, 0};

  int largest_library_seed_width = 0;
  int largest_library_seed_height = 0;
  
  //int static_shift_x[4];  //x and y shifts to get from 0,0 to new starting location
  //int static_shift_y[4];  //
  
  //library seed retrieval//
  for(int i=0; i<cluster_num; ++i)
  {
    random_calibration_seed = rand() % (calibration_seed_number-1);             //select random library seed and retrieve raster image
    //cout<<"random_seed_num: "<<random_calibration_seed<<endl;   //OMIT~~~
    if(random_calibration_seed != 0) seed_list_start_position = raw_image_zeroes_list.at(random_calibration_seed*3-3) + 1;
    seed_list_end_position = raw_image_zeroes_list.at(random_calibration_seed*3) - 1;
    library_seed_widths[i] = raw_image_zeroes_list.at(random_calibration_seed*3 + 1);
    library_seed_heights[i] = raw_image_zeroes_list.at(random_calibration_seed*3 + 2);
    
    library_raster_seeds.append(i);                                             //insert seed num//
    for(int j=seed_list_start_position; j<seed_list_end_position; ++j)          //store raster image to QList
    {
      lcx[i] = lcx[i] + raw_image_list.at(j) % library_seed_widths[i];
      lcy[i] = lcy[i] + raw_image_list.at(j) / library_seed_widths[i];
      library_raster_seeds.append(raw_image_list.at(j));    //append to QList//
    }
    
    library_seed_sizes[i] = seed_list_end_position - seed_list_start_position;  //find centroid of library seed
    lcx[i] = lcx[i]/library_seed_sizes[i];
    lcy[i] = lcy[i]/library_seed_sizes[i]; 
    
    if(largest_library_seed_width < library_seed_widths[i]) largest_library_seed_width = library_seed_widths[i];      //determine largest seed dimensions
    if(largest_library_seed_height < library_seed_heights[i]) largest_library_seed_height = library_seed_heights[i];
  }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------//  
  int larger_container_width = width + largest_library_seed_width;
  int larger_container_height = height + largest_library_seed_height;
  
  int static_x_shift = largest_library_seed_width/2;
  int static_y_shift = (largest_library_seed_height/2)*(larger_container_width);
  
  display_raster_p = new char[larger_container_width*larger_container_height];      //define overlap raster container size
  for (int y=0; y<larger_container_height; ++y)                                     //generate blank display_raster
  {
    for (int x=0; x<larger_container_width; ++x)
    {
      display_raster_p[x+larger_container_width*y]='.';
    }
  }
  
  //--------------------------------------------position in larger container---------------------------------------//

  for (int y=0; y<height; ++y)         //form display_raster
  {
    for (int x=0; x<width; ++x)
    {
      if(bool_raster_p[x+width*y]) display_raster_p[x+width*y + static_x_shift + static_y_shift + largest_library_seed_width*y]= '~';  //live/camera seed
      else  display_raster_p[x+width*y + static_x_shift + static_y_shift + largest_library_seed_width*y]=' ';
    }
  }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------//  
  //define starting position of respective library images relative to overlapping cluster//
  int static_x_shift_l[number_overlaps_to_test];    //starting location of library image//
  int static_y_shift_l[number_overlaps_to_test];
  int static_scaling_l[number_overlaps_to_test];    //rightside of library seed in larger container//
  
  //int loop_counter;
  
  for(int q = 0; q<cluster_num; ++q)  //defines cluster centroids//
  {
    //centroid coordinates in larger container//
    centroid_lx[q] = (centroid_x[q]+width*centroid_y[q] + static_x_shift + static_y_shift + largest_library_seed_width*centroid_y[q]) % larger_container_width;
    centroid_ly[q] = (centroid_x[q]+width*centroid_y[q] + static_x_shift + static_y_shift + largest_library_seed_width*centroid_y[q]) / larger_container_width;
    display_raster_p[centroid_lx[q] + centroid_ly[q]*larger_container_width] = '1' + q;
    
    static_x_shift_l[q] = (centroid_lx[q] - lcx[q]);
    static_y_shift_l[q] = (centroid_ly[q] - lcy[q])*(larger_container_width);
    static_scaling_l[q] = larger_container_width - ((centroid_lx[q] - lcx[q]) + library_seed_widths[q]);
    
    //display_raster_p[centroid_lx[q] + centroid_ly[q]*larger_container_width]='S';                 //TEST~~~ library seed centroid//  
    //display_raster_p[static_x_shift_l[q] + static_y_shift_l[q]]='A';                              //TEST~~~ library seed bottom left//
    //display_raster_p[static_x_shift_l[q] + static_y_shift_l[q] + library_seed_widths[q]]='B';     //TEST~~~ library seed bottom left//
    
    int library_raster_start = q;
    for(int y=0; y<=q; ++y)       //awkward numbering, may cause segfault~~~~
    {
      if(y != 0) library_raster_start = library_raster_start + library_seed_sizes[y-1];
    }
    cout<<endl<<"library_seed_size"<<library_seed_sizes[q]<<endl;   //OMIT~~~
    cout<<"library_raster_start: "<<library_raster_start<<endl;     //OMIT~~~
    
    //segfault//
    for(int r=library_raster_start+1; r<(library_raster_start+library_seed_sizes[q]); ++r)    //TEST~~~
    {
      int raster_index = library_raster_seeds.at(r) + static_x_shift_l[q] + static_y_shift_l[q] + (static_scaling_l[q] + static_x_shift_l[q])*(library_raster_seeds.at(r) / library_seed_widths[q]);
      if(raster_index < larger_container_width*larger_container_height && raster_index > 0) display_raster_p[raster_index] = 'A' + q;      
    }
  }
//-------------------------------------------------------------------diagnostics-------------------------------------------------------------------//
  cout<<endl<<"centroids: "<<endl; //OMIT~~~
  for(int q = 0; q<cluster_num; ++q)
  {
    //display_raster_p[centroid_lx[q] + centroid_ly[q]*larger_container_width]='S';                   //TEST~~~ library seed centroid//  
    display_raster_p[centroid_lx[q] + centroid_ly[q]*larger_container_width]='0'+q;                   //TEST~~~ library seed centroid//  
    cout<<q<<"(o): "<<centroid_x[q]<<"-"<<centroid_y[q]<<endl;                                        //TEST~~~
    cout<<q<<"(l): "<<static_x_shift_l[q]<<"-"<<static_y_shift_l[q]/larger_container_width<<endl;     //TEST~~~
  }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------//  
  //print display_raster//
  for(int y=larger_container_height-1; y>=0; --y)
  {
    for (int x=0; x<larger_container_width; ++x)
    {
      cout<<display_raster_p[x + larger_container_width*y];
    }
    cout<<endl;
  }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------//  
}

//==================================================================================================================================================//
void blob::overlap_test(int cluster_num) //TEST~~~ 10_18_2018
{
  cout<<"cluster_num: "<<cluster_num<<endl; //OMIT~~~
  
  //variables used to determine library raster centroid//
  int library_seed_size;
  //int library_seed_size_1;
  //int lc_x, lc1_x, lc2_x, lc3_x, lc4_x; //temporary to remove warnings
  //int lc_y, lc1_y, lc2_y, lc3_y, lc4_y; //
  int lc1_x, lc1_y;                       //
  
  //lc_x = 0;     lc_y = 0;   //intermediate calculation variable//
  lc1_x = 0;    lc1_y = 0;
  //lc2_x = 0;    lc2_y = 0;
  //lc3_x = 0;    lc3_y = 0;
  //lc4_x = 0;    lc4_y = 0;
  
  //variables used to retrieve random library raster//
  srand(time(NULL));
  int random_calibration_seed;
  int seed_list_start_position = 0;
  int seed_list_end_position;
  //int library_seed_width_1, library_seed_width_2, library_seed_width_3, library_seed_width_4;
  //int library_seed_height_1, library_seed_height_2, library_seed_height_3, library_seed_height_4;
  
  int library_seed_height;  //OMIT~~~
  int library_seed_width;   //OMIT~~~
  
  //variables to store retrieved library rasters//
  QList<int> library_seed_list_1;
  QList<int> library_seed_list_2;
  QList<int> library_seed_list_3;
  QList<int> library_seed_list_4;
  
//===============================================================================================================//   //TEST~~~~~~~~~~
  //array alternatives//
  //int lcx[4];
  //int lcy[4];
  //int library_seed_sizes[4] = {0, 0, 0, 0};
  //int library_seed_widths[4] = {0, 0, 0, 0};
  //int library_seed_heights[4] = {0, 0, 0, 0};

  //int largest_library_seed_width = 0;
  //int largest_library_seed_height = 0;
  
  //int static_shift_x[4];  //x and y shifts to get from 0,0 to new starting location
  //int static_shift_y[4];  //

//===============================================================================================================//   //TEST~~~~~~~~~~
  //library seed retrieval//
  /*
  for(int i; i<cluster_num; ++i)
  {
    random_calibration_seed = rand() % (calibration_seed_number-1);             //select random library seed and retrieve raster image
    if(random_calibration_seed != 0) seed_list_start_position = raw_image_zeroes_list.at(random_calibration_seed*3-3) + 1;
    seed_list_end_position = raw_image_zeroes_list.at(random_calibration_seed*3) - 1;
    library_seed_widths[i] = raw_image_zeroes_list.at(random_calibration_seed*3 + 1);
    library_seed_heights[i] = raw_image_zeroes_list.at(random_calibration_seed*3 + 2);
    
    for(int j=seed_list_start_position; j<seed_list_end_position; ++j)          //store raster image to QList
    {
      lcx[i] = lcx[i] + raw_image_list.at(j) % library_seed_width;
      lcy[i] = lcy[i] + raw_image_list.at(j) / library_seed_width;
    }
    
    library_seed_sizes[i] = seed_list_end_position - seed_list_start_position;  //find centroid of library seed
    lcx[i] = lcx[i]/library_seed_sizes[i];
    lcy[i] = lcy[i]/library_seed_sizes[i]; 
    
    if(largest_library_seed_width < library_seed_widths[i]) largest_library_seed_width = library_seed_widths[i];      //determine largest seed dimensions
    if(largest_library_seed_height < library_seed_heights[i]) largest_library_seed_height = library_seed_heights[i];
  //---------------------------------------------------------------------------------------------------------------//  
    //re-define centroid variables found from kmeans clustering -> use an array to store centroids instead//
    
    int centroid_x[4];    //temporary placement variable, define in header file
    int centroid_y[4];    //
    
    //centroid_x[i] = (c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y) % larger_container_width;   //define centroids relative to larger container//
    //centroid_y[i] = (c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y) / larger_container_width;
    
    //determine starting position of library seeds in 
    //static_shift_x[i] =     
    
  }
  
  display_raster_p = new char[(width+largest_library_seed_width)*(height+largest_library_seed_height)];      //define overlap raster container size

  */

//===============================================================================================================//    //TEST~~~~~~~~~~
  
  
//---------------------------------------------------------------------------------------------------------------//  
  random_calibration_seed = rand() % (calibration_seed_number-1);         //select random library seed
  if(random_calibration_seed != 0) seed_list_start_position = raw_image_zeroes_list.at(random_calibration_seed*3-3) + 1;
  seed_list_end_position = raw_image_zeroes_list.at(random_calibration_seed*3) - 1;
  library_seed_width = raw_image_zeroes_list.at(random_calibration_seed*3 + 1);
  library_seed_height = raw_image_zeroes_list.at(random_calibration_seed*3 + 2);
  
  display_raster_p = new char[library_seed_width*library_seed_height];
  for (int y=0; y<library_seed_height; ++y)                               //generate blank display_raster
  {
    for (int x=0; x<library_seed_width; ++x)
    {
      display_raster_p[x+library_seed_width*y]='.';
    }
  }
  for(int i=seed_list_start_position; i<seed_list_end_position; ++i)      //form display_raster
  {
    display_raster_p[raw_image_list.at(i)] = 'L';
    
    lc1_x = lc1_x + raw_image_list.at(i) % library_seed_width;
    lc1_y = lc1_y + raw_image_list.at(i)/library_seed_width;
  }
  
  library_seed_size = seed_list_end_position - seed_list_start_position;  //find centroid of library seed
  lc1_x = lc1_x/library_seed_size;
  lc1_y = lc1_y/library_seed_size;
  display_raster_p[lc1_x+library_seed_width*lc1_y] = ' ';

 
  for (int y=library_seed_height-1; y>=0; --y)                            //print display_raster
  {
    for (int x=0; x<library_seed_width; ++x)
    {
      cout<<display_raster_p[x + library_seed_width*y];
    }
    cout<<endl;
  }
  cout<<endl<<endl; //OMIT~~~

  //--------------------------------------------position in larger container---------------------------------------//  
  int width_expansion = library_seed_width;
  int height_expansion = library_seed_height; 
  
  int larger_container_width = width + width_expansion;
  int larger_container_height = height + height_expansion;
  
  display_raster_p = new char[larger_container_width*larger_container_height];      //define display_raster
  
  for (int y=0; y<larger_container_height; ++y)                             //generate blank display_raster
  {
    for (int x=0; x<larger_container_width; ++x)
    {
      display_raster_p[x+larger_container_width*y]='.';
    }
  }
  int static_x_shift = width_expansion/2;
  int static_y_shift = (height_expansion/2)*(larger_container_width);
  
  for (int y=0; y<height; ++y)                                              //form display_raster
  {
    for (int x=0; x<width; ++x)
    {
      if(bool_raster_p[x+width*y]) display_raster_p[x+width*y + static_x_shift + static_y_shift + width_expansion*y]='-';
      else  display_raster_p[x+width*y + static_x_shift + static_y_shift + width_expansion*y]=' ';
    }
  }
  
  display_raster_p[c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y]='1';  //centroid cluster 1
  
  //---------------------------------------------------------------------------------------------------------------//  
  /*for (int y=larger_container_height-1; y>=0; --y)                          //print display_raster
  {
    for (int x=0; x<larger_container_width; ++x)
    {
      cout<<display_raster_p[x + larger_container_width*y];
    }
    cout<<endl;
  }*/
  
  //---------------------------------------------------------------------------------------------------------------//  

  //int centroid_x_1, centroid_x_2, centroid_x_3, centroid_x_4;
  int centroid_x_1;
  //int centroid_y_1, centroid_y_2, centroid_y_3, centroid_y_4;
  int centroid_y_1;

  centroid_x_1 = (c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y) % larger_container_width;   //define centroids relative to larger container//
  centroid_y_1 = (c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y) / larger_container_width;
  //centroid_x_1 = (c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y) % larger_container_width;   //define centroids relative to larger container//
  //centroid_y_1 = (c1_x+width*c1_y + static_x_shift + static_y_shift + width_expansion*c1_y) / larger_container_width;
  
  int static_x_shift_l1;  //starting location of library image//
  int static_y_shift_l1;
  int static_scaling_l1;  //rightside of library seed in larger container//
  
  static_x_shift_l1 = (centroid_x_1 - lc1_x);
  static_y_shift_l1 = (centroid_y_1 - lc1_y)*(larger_container_width);
  static_scaling_l1 = larger_container_width - ((centroid_x_1 - lc1_x) + library_seed_width);
  
  //display_raster_p[centroid_x_1 + centroid_y_1*larger_container_width]='S';           //TEST~~~ library seed centroid//  
  //display_raster_p[static_x_shift_l1 + static_y_shift_l1]='S';                        //TEST~~~ library seed bottom left//
  //display_raster_p[static_x_shift_l1 + static_y_shift_l1 + library_seed_width]='B';   //TEST~~~ library seed bottom right//
  
//---------------------------------------------------------------------------------------------------------------//  
  //form library raster in larger container//
  for(int i=0; i<library_seed_width; ++i)
  {
    for(int i=seed_list_start_position; i<seed_list_end_position; ++i)    //form display_raster
    {
      display_raster_p[raw_image_list.at(i) + static_x_shift_l1 + static_y_shift_l1 + (static_scaling_l1 + static_x_shift_l1)*((raw_image_list.at(i) / library_seed_width))] = 'A';
    }
  }
  //---------------------------------------------------------------------------------------------------------------//  
  //define centroids//
  display_raster_p[centroid_x_1 + centroid_y_1*larger_container_width]='S';           //TEST~~~ library seed centroid//  

  
  //---------------------------------------------------------------------------------------------------------------//  
  //print display_raster//
  for (int y=larger_container_height-1; y>=0; --y)
  {
    for (int x=0; x<larger_container_width; ++x)
    {
      cout<<display_raster_p[x + larger_container_width*y];
    }
    cout<<endl;
  }
}

//==================================================================================================================================================//
//===========================================================FUNCTIONS FOR OVERLAP ALGORITHM========================================================//
//==================================================================================================================================================//
void blob::store_raw_image_rasters()
{
  if(height > largest_dimension) largest_dimension = height;      //stores largest dimension size (used for normalized resizing//
  if(width > largest_dimension) largest_dimension = width;

  for(int y=0; y<height; ++y)
  {
    for(int x=0; x<width; ++x) if(bool_raster_p[x+width*y]) raw_image_list.append(x+width*y); 
  }
  
  raw_image_list.append(0);											                  //define endpoint of each raster
  raw_image_zeroes_list.append(raw_image_list.lastIndexOf(0));		//saves zero position to define start+end of segments
  raw_image_zeroes_list.append(width);
  raw_image_zeroes_list.append(height);
  raw_image_size_list.append(area);
  ++number_listed_rasters;
  
  cout<<endl<<"Creating seed library"<<endl;               //OMIT~~~
  cout<<"listed_rasters: "<<number_listed_rasters<<endl;   //OMIT~~~
  
  if(number_listed_rasters == calibration_seed_number)
  {
	  cout<<endl<<"largest_dimension: "<<largest_dimension<<endl;
	  store_rotated_rasters();
  }
}

void blob::store_rotated_rasters()	//function generates rotated raster images from the original raster image and stores into list
{
  int y_scaling = 0;				        //variable for resizing raster after rotation
  int zero_pos;						          //zero positions in Qlist
  int rotated_raster_index = 0;		  //remapped index for rasters after shifts and rotation  

  QList<int> index_segment;			    //temporary list that contains a single raster - for list sorting and removing duplicates
  int line_start;					          //define start + end positions of lines for gap fill functions
  int line_end;	
  int prev_start = 0;				        //gap fill - variables for extending shorter lines
  int prev_end = 0;
  int prev2_start = 0;
  int prev2_end = 0;

  bool use_scaling_factor;			    //separate between 0,90,180,270 from 45,135,225,315
  int segment_start = 0;
  int segment_end = 0;

  x_scale_factor = (1.0+scaling_buffer)*largest_dimension/rmax_x;  
  
//====================Retrieving Raw Raster Image Index Values===================//  
  QSet<int> raw_image_seed;		                              //store and retrieve raw image data
  
  for(int q=0; q<raw_image_zeroes_list.size(); q+=3)        //load rotated raster images into list
  {
    if(q>0) segment_start = raw_image_zeroes_list[q-3]+1;	  //retrieve start + end
    else segment_start = 0;
    segment_end = raw_image_zeroes_list[q];
    
    width = raw_image_zeroes_list[q+1];						          //retrieve width + height
    height = raw_image_zeroes_list[q+2];
    half_width = width/2;						                        //determine center position of raster in bitmap container
    half_height = height/2;
    start_raster_x = (rmax_x/2-half_width);		              //define boundaries of raster image in enlarged raster
    start_raster_y = (rmax_y/2-half_height);
    end_raster_x = (rmax_x/2+half_width);
    end_raster_y = (rmax_y/2+half_height);
//-------------------------------------------------------------------------------------------------------------//
	  for(int j=segment_start; j<segment_end; ++j)
	  {
	    raw_image_seed.insert(raw_image_list[j]);
	  }
    
    for (int z=0; z<8; ++z)	  	//apply rotation to seed raster formations
    {
      theta = pi/4;
      theta = z*theta;
      if(z == 0 || z == 2 || z == 4 || z == 6) use_scaling_factor = 1;	//separate between 0,90,180,270 and 45,135,225,315
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
	            shifted_x = (x-(rmax_x/2))*qSqrt(2)/x_scale_factor;						        //1. center raster at origin
              shifted_y = (y-(rmax_y/2))*qSqrt(2)*yfactor/x_scale_factor;				    //(scaling factor 2.5 (and Sqrt(2) to eliminate rounding caused by trig. equations))
	            rotated_x = (shifted_x*qCos(theta)-shifted_y*qSin(theta))/qSqrt(2);		//2. rotate raster about the origin
	            rotated_y = (shifted_x*qSin(theta)+shifted_y*qCos(theta))*yfactor;
	            unshifted_x = rotated_x+(rmax_x/2);										                //3. unshift raster from origin back to center of origin
	            unshifted_y = rotated_y+(rmax_y/2);
              rotated_raster_index = unshifted_x+rmax_x*unshifted_y;					      //define trasformed raster's new position in container
              y_scaling = rotated_y-rotated_y/(yfactor*qSqrt(2));						        //4. apply vertical squishing to raster image
              index_segment.append(rotated_raster_index-rmax_x*y_scaling);      		//5. Append to qlist          		
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
  	          shifted_x = (x-(rmax_x/2))/x_scale_factor;
	            shifted_y = (y-(rmax_y/2+0.0001))*yfactor/x_scale_factor;					     //shift by 0.0001 is used to eliminate rounding error in raster image
	            rotated_x = (shifted_x*qCos(theta)-shifted_y*qSin(theta));
	            rotated_y = (shifted_x*qSin(theta)+shifted_y*qCos(theta))*yfactor;
	            unshifted_x = rotated_x+(rmax_x/2);
	            unshifted_y = rotated_y+(rmax_y/2);
              rotated_raster_index = unshifted_x+rmax_x*unshifted_y;
              y_scaling = rotated_y-rotated_y/(yfactor);
            
              index_segment.append(rotated_raster_index-rmax_x*y_scaling);
            }
          }
        }
      }
//------------------------------------------------------------------------------------------------------------------------------------------//
      QSet<int> segment_set = index_segment.toSet(); 
      index_segment.clear();
//------------------------------------------------------------------------------------------------------------------------------------------//
      prev_start = rmax_x;		//Gap Fill - variables for extending shorter lines
      prev_end = 0;
      prev2_start = rmax_x;
      prev2_end = 0;
    
      if(z == 0 || z == 4)		//Gap Fill - columns
      {
        for(int y=0; y<rmax_y; ++y)
        {
          line_start = rmax_x;
          line_end = 0;
          for(int x=0; x<rmax_x; ++x)	//first pass: find start + end positions of column
          {
            if(segment_set.contains(y+rmax_x*x))       
            {  
              line_end = x;
              if(line_end<line_start) line_start = x;
            }
          }      
          for(int x=0; x<rmax_x; ++x)	//second pass: fill gaps that exist between endpoints of line
          {
            if((!segment_set.contains(y+rmax_x*x)) && x>line_start && x<line_end) segment_set.insert(y+rmax_x*x); //TEST~~
          }
        }
      }         
      else 		      			    //Gap Fill - rows
      {	
        for(int y=0; y<rmax_y; ++y)
        {
          line_start = rmax_x;
          line_end = 0;
          for(int x=0; x<rmax_x; ++x)
          {
            if(segment_set.contains(x+rmax_x*y))
            {  
              line_end = x;
              if(line_end<line_start) line_start = x;
            }
          }      
          for(int x=0; x<rmax_x; ++x)
          {
            if((!segment_set.contains(x+rmax_x*y)) && x>line_start && x<line_end) segment_set.insert(x+rmax_x*y);
          }
          while(prev_start>line_start && prev_start>prev2_start)	//extends line if line before and after are longer
          { 
            segment_set.insert((prev_start-1)+rmax_x*(y-1));
            --prev_start;
          }
          while(prev_end<line_end && prev_end<prev2_end) 
          {
            segment_set.insert((prev_end+1)+rmax_x*(y-1));
            ++prev_end;
          }	    
          prev2_end = prev_end;			//define endpoints of current line, previous line, and two lines previous
          prev2_start = prev_start;
          prev_end = line_end;
          prev_start = line_start;
        }
      }	
      if(z==0) average_calibration_seed_size += segment_set.size();	//TEST~~~
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
      index_segment.clear();	
    }
    raw_image_seed.clear();
  }    
  average_calibration_seed_size = average_calibration_seed_size/calibration_seed_number;	//TEST~~~
  remove_similar_rasters();		//compare calibration data library and removes similar rasters
}
//---------------------------------------------------------------------------------------------------------------------------------//  
void blob::remove_similar_rasters()	//compare and remove similar seeds in Qlist, link top 5 most similiar rasters to remaining
{
  int segment_start = 0;
  int segment_end = 0;
  QList<int> index_segment_1;		//temporary list that contains a single raster
  QList<int> index_segment_2;

  int seed_area_1;
  int seed_area_2;
  int score = 0;        //denotes number of differences between two rasters
  int i = 0;            //increment segment 1
  int j = 0;            //increment segment 2
  
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
  for(int b=0; b<index_zero_positions_list.size()-1; ++b)
  {	
    for(i=0; i<link_num; ++i) score_array[i] = 1;   //score_array
    full_score_list.append(b);                                   
    segment_end = index_zero_positions_list[b];  
    for(int i = segment_start; i<segment_end; ++i)
    {
      index_segment_1.append(index_list[i]);  
    }
    seed_area_1 = index_segment_1.size(); 
//------------------------load segment 2----------------------------// 
    for(int a=b+1; a<index_zero_positions_list.size(); ++a)
    {
      segment_start = index_zero_positions_list[a]+1;
      segment_end = index_zero_positions_list[a+1];
  
      for(j=segment_start; j<segment_end; ++j)
      {
        index_segment_2.append(index_list[j]); 
      }
      seed_area_2 = index_segment_2.size();
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

void blob::raster_comparison(int cluster_num)  //function for comparing raster library images to live/camera images
{
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
  
  
  cout<<"Expected Seed #: "<<cluster_num<<endl;                           //OMIT~~~
  if(cluster_num >= 1) cout<<"c1_x: "<<c1_x<<"  c1_y: "<<c1_y<<endl;  //OMIT~~~
  if(cluster_num >= 2) cout<<"c2_x: "<<c2_x<<"  c2_y: "<<c2_y<<endl;  //OMIT~~~
  if(cluster_num >= 3) cout<<"c3_x: "<<c3_x<<"  c3_y: "<<c3_y<<endl;  //OMIT~~~
  if(cluster_num >= 4) cout<<"c4_x: "<<c4_x<<"  c4_y: "<<c4_y<<endl;  //OMIT~~~
  
  //display_raster_p[c2_mean_x+width*c2_mean_y]='~';      //centroid
  
  //temporary: calculate percentage of centroid relative to centre//
  //float percentage_c1_x, percentage_c2_x, percentage_c3_x, percentage_c4_x;
  //float percentage_c1_y, percentage_c2_y, percentage_c3_y, percentage_c4_y;
  
  percentage_c1_x = 1.0*c1_x/(width);
  percentage_c2_x = 1.0*c2_x/(width);
  percentage_c3_x = 1.0*c3_x/(width);
  percentage_c4_x = 1.0*c4_x/(width);
  percentage_c1_y = 1.0*c1_y/(height);
  percentage_c2_y = 1.0*c2_y/(height);
  percentage_c3_y = 1.0*c3_y/(height);
  percentage_c4_y = 1.0*c4_y/(height);
  
  if(cluster_num >= 1) cout<<"c1x_%: "<<percentage_c1_x<<"  c1y_%: "<<percentage_c1_y<<endl;  //OMIT~~~
  if(cluster_num >= 2) cout<<"c2x_%: "<<percentage_c2_x<<"  c2y_%: "<<percentage_c2_y<<endl;  //OMIT~~~
  if(cluster_num >= 3) cout<<"c3x_%: "<<percentage_c3_x<<"  c3y_%: "<<percentage_c3_y<<endl;  //OMIT~~~
  if(cluster_num >= 4) cout<<"c4x_%: "<<percentage_c4_x<<"  c4y_%: "<<percentage_c4_y<<endl;  //OMIT~~~
  
//===========================================================================================================//
//-----------------------------------------------------------------------------------------------------------//
  x_scale_factor = (1.0+scaling_buffer)*largest_dimension/rmax_x;	//TEST~~~
  
  half_width = width/2;						          //determine center position of raster in bitmap container
  half_height = height/2;
  start_raster_x = (rmax_x/2-half_width);	  //define boundaries of raster image in enlarged raster
  start_raster_y = (rmax_y/2-half_height);
  end_raster_x = (rmax_x/2+half_width);
  end_raster_y = (rmax_y/2+half_height);
//-----------------------------------------------------------------------------------------------------------//
  for (double y=start_raster_y; y<end_raster_y; ++y)	//performs transformation on seeds from camera (or from playback)
  {
    for (double x=start_raster_x; x<end_raster_x; ++x)
    {  
      raster_position_in_container = (x-start_raster_x)+width*(y-start_raster_y);
      if(bool_raster_p[raster_position_in_container])
      {  
        shifted_x = (x-(rmax_x/2))/x_scale_factor;
        shifted_y = (y-(rmax_y/2))*(yfactor*1.2)/x_scale_factor;
        unshifted_x = shifted_x+(rmax_x/2);
        unshifted_y = shifted_y+(rmax_y/2);
        rotated_raster_index = unshifted_x+rmax_x*unshifted_y;
        //y_scaling = shifted_y-shifted_y/(yfactor);
        y_scaling = 1;
        camera_seed.append(rotated_raster_index-rmax_x*y_scaling);	//ORIGINAL~~~
      }
    }
  }
  //block of code for producing 2nd raster image of resized 20x20//
  //try adding n*rmax_y_store/2+rmax_x_store to each column pixel//
  //for(int q=0; q<
  
  
  //BUILD EMBEDDED for loops to shift each point over by: n*rmax_y_store/2+rmax_x_store//
  //double check round errors and multiples
  //should generate appropriate image without multiples, values should occupy exactly a sing
  //camera_seed_row_start = rmax_x*q;
  //camera_seed_row_end = x+rmax_x*q;
//------------------------------------------------------------------------------------------------------------------------------------------//    
  QSet<int> camera_seed_set = camera_seed.toSet();

  for(int y=0; y<rmax_y; ++y)
  {
    line_start = rmax_x;
    line_end = 0;
    for(int x=0; x<rmax_x; ++x)	//first pass through line to find start + end positions of column
    {
      if(camera_seed_set.contains(y+rmax_x*x))       
      {  
        line_end = x;
        if(line_end<line_start) line_start = x;
      }
    }      
    for(int x=0; x<rmax_x; ++x)	//fills gaps that exist between endpoints of line
    {
      if((!camera_seed_set.contains(y+rmax_x*x)) && x>line_start && x<line_end) camera_seed_set.insert(y+rmax_x*x); //TEST~~
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
//--------------------------------------------------Diagnostics-------------------------------------------------//
  cout<<"best_score: "<<best_score<<"   best_raster: "<<best_raster<<"	score: "<<best_score*larger_area<<endl;
  cout<<"camera_seed_size: "<< camera_seed_size<<"  best_fit_size: "<<best_raster_size<<endl;   
  cout<<"W: "<<width<<"	H: "<<height<<endl;
  cout<<"adjusted_W: "<<width/x_scale_factor<<"	adjusted_H: "<<height*(yfactor*1.2)/x_scale_factor<<endl;
  //cout<<"FILE WORKING"<<endl;
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
  
  int test_var = rmax_x*percentage_c1_x + rmax_x * (rmax_y*percentage_c1_y);
  //TEST~~~
  //if(cluster_num >= 1) display_rotated_raster_p[1*rmax_x*percentage_c1_x+1*rmax_x*(percentage_c1_y*rmax_y)]='1';
  //if(cluster_num >= 2) display_rotated_raster_p[1*rmax_x*percentage_c2_x+rmax_x*(percentage_c2_y*rmax_y)]='2';
  //if(cluster_num >= 3) display_rotated_raster_p[1*rmax_x*percentage_c3_x+rmax_x*(percentage_c3_y*rmax_y)]='3';
  //if(cluster_num >= 4) display_rotated_raster_p[1*rmax_x*percentage_c4_x+rmax_x*(percentage_c4_y*rmax_y)]='4';
  
  //if(cluster_num >= 1) display_rotated_raster_p[percentage_c1_x+1*rmax_x*(percentage_c1_y*rmax_y)]='1';  
  //if(cluster_num >= 1) display_rotated_raster_p[test_var*rmax_x*percentage_c1_x] ='1'];  
  if(cluster_num >= 1) display_rotated_raster_p[test_var] ='1';
  //if(cluster_num >= 1) display_rotated_raster_p[test_var] ='1';
  
  print_rotated_rasters();	//TEST~~~
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
  seed_number_overlap_test = 1;
     
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
  
  cout<<"single target: "<<target_score<<"	";	//OMIT~~~
  cout<<"best: "<<best_score<<"	larger: "<<larger_area<<"	average_size: "<<average_calibration_seed_size<<"	X= "<<best_score*larger_area/average_calibration_seed_size<<endl;			//OMIT~~~
  //cout<<"SD scale: "<<(5-60.0*variability_score)<<"	area:"<<area<<"	c.area:"<<(area_mean+((4-40.0*variability_score)*std_deviation_area))<<" ("<<(4-40.0*variability_score)<<")"<<endl;		//OMIT~~~
  cout<<"area:"<<area<<"	c.area:"<<(area_mean+(((((1-10.0*variability_score)-0.35)*10)-0.35)*std_deviation_area))<<" ("<<((((1-10.0*variability_score)-0.35)*10)-0.35)<<")"<<endl;		//OMIT~~~
  
  cout<<endl<<endl;																									                                            //OMIT~~~
  cout<<"<cond1> "<<best_score*larger_area/average_calibration_seed_size<<" - 0.25"									            //OMIT~~~
	  //<<"		adjusted: ("<<10*variability_score*best_score*larger_area/average_calibration_seed_size				      //OMIT~~~
	  //<<"		adjusted: ("<<(1/(30*variability_score))*best_score*larger_area/average_calibration_seed_size-0.1	  //OMIT~~~
      <<"		adjusted: ("<<(1/(20*variability_score))*best_score*larger_area/average_calibration_seed_size-0.1	    //OMIT~~~
      <<" - "<<(0.25+2*variability_score)<<")"<<endl;																                              //OMIT~~~
  cout<<"<cond2> "<<best_score<<" - "<<target_score<<endl;															                          //OMIT~~~
  cout<<"<cond3> "<<area<<" - "<<area_mean+(((((1-10.0*variability_score)-0.35)*10))*std_deviation_area)			    //OMIT~~~
      <<"		# of SD ("<<(area-area_mean)/std_deviation_area<<")"<<endl;											                      //OMIT~~~
  cout<<endl<<"<cond*1> "<<(1/(20*variability_score))*best_score*larger_area/average_calibration_seed_size<<" - "
      <<0.125*(1/(10*variability_score))<<endl;		//OMIT~~~ 	07_30_2018b
	  
  
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
    }
  }
//==================================================Diagnostics============================================================================//
  cout<<"seed_number_overlap_test: "<<seed_number_overlap_test<<"	best_score: "<<best_score<<endl;				      //OMIT~~~
  cout<<"prev.: "<<previous_best_score<<"	curr.: "<<current_best_score<<"	Target_Score: "<<target_score<<endl;	//OMIT~~~
  cout<<"============================================================"<<endl;
//=========================================================================================================================================//
}

bool blob::read_calibration_data_file()
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
    cout<<endl<<calibrated_crop_name<<endl<<largest_dimension<<" "<<average_calibration_seed_size<<endl<<height_mean<<" "		//OMIT~~~
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

bool blob::write_calibration_data_file() 
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

void blob::calculate_threshold_values()
{
  int num_rasters;
  float width_sum = 0;
  float height_sum = 0;
  float area_sum = 0;
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
}

//==========================================================DIAGNOSTIC OUTPUT FILE===========================================================//
//just output to file the score, best_score, larger_area, best_fit_size
bool blob::write_diagnostic_data_file(float seed_number_overlap_test, float score, float best_score, float camera_seed_size, float best_fit_size,
									  float variability_score, float larger_area) 
{
  {
    std::ofstream diagnostic_out("diagnostic_data/overlap_test",(std::ofstream::out|std::ofstream::app));
	
    diagnostic_out<<seed_number_overlap_test<<" "<<score<<" "<<best_score<<" "<<camera_seed_size<<" "<<best_fit_size<<" "
				  <<qFabs(camera_seed_size - best_fit_size)<<" "<<qFabs(camera_seed_size - best_fit_size)/larger_area<<" "<<variability_score<<endl;

    
    diagnostic_out.close();
    diagnostic_out.clear();
  }
  cout<<endl<<"Diagnostic data stored"<<endl;
  return 1;
}
//===========================================================================================================================================//

void blob::print_rotated_rasters()
{
  for (int y=rmax_y-1; y>=0; --y)
  {
    for (int x=0; x<rmax_x; ++x)
    {
      cout<<display_rotated_raster_p[x + rmax_x*y]<<" ";
    }
    cout<<endl;
  }	
}

void blob::print_resized_rasters()
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

