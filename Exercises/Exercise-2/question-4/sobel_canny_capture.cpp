#include "opencv2/imgproc/imgproc.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <syslog.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace cv;
using namespace std;

//global variables
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";



Mat sobel_transform(Mat mat_frame){
  //printf("Performing sobel transform");
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  GaussianBlur( mat_frame, mat_frame, Size(3,3), 0, 0, BORDER_DEFAULT );
  cvtColor( mat_frame, mat_frame, CV_RGB2GRAY );
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  Sobel( mat_frame, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );   
  convertScaleAbs( grad_x, abs_grad_x );

  Sobel( mat_frame, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );   
  convertScaleAbs( grad_y, abs_grad_y );

  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, mat_frame );

  return mat_frame;
}

Mat canny_transform(Mat mat_frame){
  cvtColor( mat_frame, mat_frame, CV_BGR2GRAY );
  blur( mat_frame, mat_frame, Size(3,3) );
  Canny( mat_frame, mat_frame, lowThreshold, lowThreshold*ratio, kernel_size );
  return mat_frame;
}

double getTimeMsec(void)
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}
    
//Tried to update the values using pointers but got segmentation faults
void frame_computations(double *total_time,double *worst_case_time,double time_taken_for_frame){
    (*total_time) += time_taken_for_frame;
    if(time_taken_for_frame>(*worst_case_time)){
      (*worst_case_time=time_taken_for_frame);
    }
    return;
}

int main( int argc, char** argv )
{
    
    //cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    //CvCapture* capture = cvCreateCameraCapture(0);
    //namedWindow( window_name, CV_WINDOW_AUTOSIZE );
    VideoCapture capture1(0);
    IplImage* frame;
    Mat mat_frame;
    double total_frame_count,sobel_frame_count,canny_frame_count;
    double total_time_sobel,total_time_canny;
    double worst_case_time_sobel=0,worst_case_time_canny=0;
    double frame_rate_sobel,frame_rate_canny;
    double current_frame_time,time_taken_for_frame;

    char c,c2='a';

    while(1)
    {
            
        if(total_frame_count >= 30){
          time_taken_for_frame = getTimeMsec() - current_frame_time;
          if(c2 == 's' || c2 == 'S'){
              sobel_frame_count++;
              syslog(LOG_INFO,"Time taken for sobel transform frame %d: %lf",(int)sobel_frame_count,time_taken_for_frame);
              //syslog(LOG_INFO,"Current Total Sobel frame time  = %lf milliseconds\n",total_time_sobel);
              //frame_computations(total_time_sobel,worst_case_time_sobel,time_taken_for_frame);
              total_time_sobel += time_taken_for_frame;
              if(time_taken_for_frame>worst_case_time_sobel){
                  worst_case_time_sobel=time_taken_for_frame;
              }

          }else if(c2 == 'c' || c2 == 'C' ){
              canny_frame_count++;
              syslog(LOG_INFO,"Time for canny transform on  frame %d: %lf",(int)canny_frame_count,time_taken_for_frame);
              total_time_canny += time_taken_for_frame;
              if(time_taken_for_frame>worst_case_time_canny){
                  worst_case_time_canny=time_taken_for_frame;
              }
              //frame_computations(total_time_canny,worst_case_time_canny,time_taken_for_frame);
          }
        }
        capture1  >> mat_frame;
        current_frame_time = getTimeMsec(); 
        c = cvWaitKey(33);
        if(c != 0xff){
          c2=c;
          //printf("c2:%d",c2);
        }
        if(mat_frame.empty()){
         break;
        }
        total_frame_count++;
         
          if(c2 == 's' || c2 == 'S'){
          mat_frame = sobel_transform(mat_frame);
          }
          else if(c2 == 'c' || c2 == 'C' ){
          mat_frame = canny_transform(mat_frame);
          }
        
       
        imshow("Frame",mat_frame);
        if( c == 27 ) break;
    }
    //No of frames processed
    printf("Total Frames processed: %d \n",(int)total_frame_count);
    syslog(LOG_INFO,"Total Frames processed: %d\n",(int)total_frame_count);
    printf("Sobel Frames processed: %d\n",(int)sobel_frame_count);
    syslog(LOG_INFO,"Sobel Frames processed: %d\n",(int)sobel_frame_count);
    printf("Canny Frames processed: %d\n",(int)canny_frame_count);
    syslog(LOG_INFO,"Canny Frames processed: %d\n",(int)canny_frame_count);
    //Sobel frame stats
    frame_rate_sobel = sobel_frame_count*1000/total_time_sobel;
    printf("Total Sobel frame time  = %lf milliseconds\n",total_time_sobel);
    syslog(LOG_INFO,"Total Sobel frame time  = %lf milliseconds\n",total_time_sobel);
    printf("worst case time sobel frame = %lf  milliseconds\n", worst_case_time_sobel);
    syslog(LOG_INFO,"worst case time sobel frame = %lf  milliseconds\n", worst_case_time_sobel);
    printf("average sobel frame rate = %f frames per second\n", frame_rate_sobel);
    syslog(LOG_INFO,"average sobel frame rate = %f \n", frame_rate_sobel);
    //Canny frame stats
    frame_rate_canny = canny_frame_count*1000/total_time_canny;
    printf("Total canny frame time  = %lf milliseconds\n",total_time_canny);
    syslog(LOG_INFO,"Total canny frame time  = %lf milliseconds\n",total_time_canny);
    printf("worst case time canny frame = %lf  milliseconds\n", worst_case_time_canny);
    syslog(LOG_INFO,"worst case time canny frame = %lf  milliseconds\n", worst_case_time_canny);
    printf("average canny frame rate = %f frames per second \n", frame_rate_canny);
    syslog(LOG_INFO,"average canny frame rate = %f \n", frame_rate_canny);

    capture1.release();
    
};