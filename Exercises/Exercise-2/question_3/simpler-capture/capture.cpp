/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <syslog.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

double getTimeMsec(void)
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}

int main( int argc, char** argv )
{
    uint64_t diff;
    int  frameCount=0;
    struct timespec startTime = {0,0};
    struct timespec endTime = {0,0};
    struct timespec beforeFrame = {0,0};
    struct timespec afterFrame = {0,0};
    struct timespec maxTime = {0,0};
    double preFrame,postFrame;
    double totaltime,worstCaseTime,min_time=10000000,time_taken_for_frame,reference_frame_time,max_time;
    double negative_jitter,positive_jitter;
    float  framerate;
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(0);
    IplImage* frame;
    clock_gettime(CLOCK_MONOTONIC,&startTime);
    while(1)
    { 
        time_taken_for_frame = getTimeMsec()-preFrame; 
        //Frame 10-90 for finding out reference values to compare for jitter
        if(frameCount>=10 && frameCount<=100){
            if(time_taken_for_frame<min_time){
              min_time=time_taken_for_frame;
            }
            if(time_taken_for_frame>max_time){
              max_time = time_taken_for_frame;
            }
            
        }
        if(frameCount == 101){
           reference_frame_time=(min_time+max_time)/2;
        }
        preFrame=getTimeMsec();
        frame=cvQueryFrame(capture);

        if(!frame){ 
          break;
          }        
        frameCount++;
        cvShowImage("Capture Example", frame);
        postFrame=getTimeMsec();

        if(frameCount>= 1900)
        {
           break;
        }
        //Start computing only after 100 frames
        if(frameCount>100){
          totaltime += time_taken_for_frame; // cumulative sum of time taken from each frame
          if(time_taken_for_frame>worstCaseTime) {
          worstCaseTime = time_taken_for_frame;
          }
          if(time_taken_for_frame>reference_frame_time){
          positive_jitter += time_taken_for_frame - reference_frame_time;
          }else{
            negative_jitter += -(time_taken_for_frame - reference_frame_time);
          }

        }
        
        syslog(LOG_INFO,"Time taken for frame %d : %lf milliseconds",frameCount,time_taken_for_frame);

        char c = cvWaitKey(1);
        if( c == 27 ) break;
    }
   clock_gettime(CLOCK_MONOTONIC,&endTime); 
   printf("Total frame time  = %lf milliseconds\n",totaltime);
   printf("worst case time frame execution = %lf  milliseconds\n", worstCaseTime);
   printf("Total frames = %d \n", (frameCount-100));
   printf("average frame rate = %f \n", totaltime/(frameCount-100));
   printf("total positive jitter = %lf \n", positive_jitter);
   printf("total negative jitter = %lf \n", negative_jitter);
   syslog(LOG_INFO,"Total frame time  = %lf milliseconds\n",totaltime);
   syslog(LOG_INFO,"worst case time frame = %lf  milliseconds\n", worstCaseTime);
   syslog(LOG_INFO,"Total frames = %d \n", (frameCount-100));
   syslog(LOG_INFO,"average frame rate = %f \n",(frameCount-100)/totaltime);
   syslog(LOG_INFO,"positive jitter = %lf \n", positive_jitter);
   syslog(LOG_INFO,"negative jitter = %lf \n", negative_jitter);
   cvReleaseCapture(&capture);
   cvDestroyWindow("Capture Example");
};
