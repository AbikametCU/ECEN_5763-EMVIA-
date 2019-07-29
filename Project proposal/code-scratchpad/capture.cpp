#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>
#include <sys/sysinfo.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace cv;
using namespace std;

#define _GNU_SOURCE

#define VRES_ROWS (480)
#define HRES_COLS (640)

#define NUM_THREADS 1
#define NUM_CPU_CORES 1

#define VIDEO 1

#define IGNORED_FRAMES 10
#define NO_OF_ANALYSIS_FRAMES 50
#define NO_OF_TRANSFORMATION_FRAMES 100
#define CUSTOM_RGB 0
unsigned char imagebuffer[1440*2560*3];

void help(){
  printf(" \n \n Usage: \n ./capture [Transform number] [IMAGE_WIDTH] [IMAGE_HEIGHT]  \n \n\
  Transform number \n 1:Sobel \n 2:Canny \n 3:Hough line \n ");
}

double getTimeMsec(void)
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}

Mat frame;

VideoCapture capture("22400001.AVI");

//variables for frame analysis
double time_before_frame_transform,time_after_frame_transform,time_taken_to_transform_frame;
double total_transformation_time;
double frame_rate;
double total_frame_count;
double reference_time;
double positive_jitter, negative_jitter;
int deadlines_miss_count=0;
int frames_required_to_be_transformed;
double deadline;

typedef struct
{
    int threadIdx;
    int image_height;
    int image_width;
} threadParams_t;

    cpu_set_t threadcpu;
    pthread_t threads[NUM_THREADS];
    threadParams_t threadParams[NUM_THREADS];
    pthread_attr_t rt_sched_attr[NUM_THREADS];
    int rt_max_prio, rt_min_prio;
    struct sched_param rt_param[NUM_THREADS];
    struct sched_param main_param;
    pthread_attr_t main_attr;
    pid_t mainpid;
    cpu_set_t allcpuset;

void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
     case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
     case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n"); exit(-1);
       break;
     case SCHED_RR:
           printf("Pthread Policy is SCHED_RR\n"); exit(-1);
           break;
     default:
       printf("Pthread Policy is UNKNOWN\n"); exit(-1);
   }

}

void *simpler_capture(void *threadp){

  while(1){
    time_before_frame_transform = getTimeMsec();
    capture >> frame;
    if(frame.empty()){ 
	    break;
	  }
    time_after_frame_transform = getTimeMsec();
    time_taken_to_transform_frame = time_after_frame_transform - time_before_frame_transform;
    printf("\nTime taken to transform frame: %f",time_taken_to_transform_frame);
    imshow("frame",frame);
    char c = cvWaitKey(1);
    if( c == 27 ){
      break;
    }
  }
}

void real_time_setup(){
    int i, rc, scope;
    printf("System has %d processors configured and %d available.\n", get_nprocs_conf(), get_nprocs());

   CPU_ZERO(&allcpuset);

   for(i=0; i < NUM_CPU_CORES; i++)
       CPU_SET(i, &allcpuset);

   printf("Using CPUS=%d from total available.\n", CPU_COUNT(&allcpuset));

    mainpid=getpid();

    rt_max_prio = sched_get_priority_max(SCHED_FIFO);
    rt_min_prio = sched_get_priority_min(SCHED_FIFO);

    rc=sched_getparam(mainpid, &main_param);
    main_param.sched_priority=rt_max_prio;
    rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    if(rc < 0) perror("main_param");
    print_scheduler();
    pthread_attr_getscope(&main_attr, &scope);

    if(scope == PTHREAD_SCOPE_SYSTEM)
      printf("PTHREAD SCOPE SYSTEM\n");
    else if (scope == PTHREAD_SCOPE_PROCESS)
      printf("PTHREAD SCOPE PROCESS\n");
    else
      printf("PTHREAD SCOPE UNKNOWN\n");

    printf("rt_max_prio=%d\n", rt_max_prio);
    printf("rt_min_prio=%d\n", rt_min_prio);
    

    for(i=0; i < NUM_THREADS; i++)
    {

      CPU_ZERO(&threadcpu);
      CPU_SET(3, &threadcpu);

      rc=pthread_attr_init(&rt_sched_attr[i]);
      rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
      rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
      rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);

      rt_param[i].sched_priority=rt_max_prio-i-1;
      pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

      threadParams[i].threadIdx=i;
    }
   
    printf("Service threads will run on %d CPU cores\n", CPU_COUNT(&threadcpu));    
}

Mat custom_threshold(Mat mat_frame){
    int x,y;
    for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){
            if(mat_frame.at<uchar>(x,y)>40){
                mat_frame.at<uchar>(x,y)=255;
          
            }else{
             mat_frame.at<uchar>(x,y)=0;
            }
      } 
   }
    return mat_frame;
}


Mat custom_rgb_threshold(Mat mat_frame){
    int x,y;
    printf("\nno of channels in rgb threshold:%d",mat_frame.channels());
    for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){
            
            if(mat_frame.at<Vec3b>(x,y)[0]<235){
                mat_frame.at<Vec3b>(x,y)[0]=255;
          
            }else{
             mat_frame.at<Vec3b>(x,y)[0]=0;
            }
            if(mat_frame.at<Vec3b>(x,y)[1]>235){
                mat_frame.at<Vec3b>(x,y)[1]=255;
          
            }else{
             mat_frame.at<Vec3b>(x,y)[1]=0;
            }
            if(mat_frame.at<Vec3b>(x,y)[2]>235){
                mat_frame.at<Vec3b>(x,y)[2]=255;
          
            }else{
             mat_frame.at<Vec3b>(x,y)[2]=0;
            }
      } 
   }
    return mat_frame;
}


Mat canny_transform(Mat mat_frame){

  int edgeThresh = 1;
  int lowThreshold=80;
  int const max_lowThreshold = 100; 
  int ratio = 3;
  int kernel_size = 3;
  char* window_name = "Edge Map";
  cvtColor( mat_frame, mat_frame, CV_BGR2GRAY );
  blur( mat_frame, mat_frame, Size(3,3) );
  Canny( mat_frame, mat_frame, lowThreshold, lowThreshold*ratio, kernel_size );
  return mat_frame;
}

Mat image_process_2(Mat mat_frame){
  printf("\nEntered image process function");
  int x,y;
  for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){
            
          mat_frame.at<Vec3b>(x,y)[0] = mat_frame.at<Vec3b>(x,y)[0]+40;
          mat_frame.at<Vec3b>(x,y)[1] = mat_frame.at<Vec3b>(x,y)[1]+40;
          mat_frame.at<Vec3b>(x,y)[2]= mat_frame.at<Vec3b>(x,y)[2]+40;
        }
    }
  return mat_frame;
}

Mat lane_detect(Mat mat_frame){
  vector<Vec4i> lines;
  Mat mat_frame_gray = mat_frame.clone();
  mat_frame_gray = canny_transform(mat_frame_gray);
  HoughLinesP(mat_frame_gray, lines, 1, CV_PI/180, 80, 150, 10);
  for( size_t i = 0; i < lines.size(); i++ )
        {
          Vec4i l = lines[i];
          line(mat_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        }
  return mat_frame;
}

int main(int argc, char** argv){
    int i=0,rc,x,y,z;
    real_time_setup();
    vector<Vec4i> lines;
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    Mat canny_frame;
    string image_name="image_1.ppm";

#if VIDEO
    while(1){
      capture >> frame;
      if(frame.empty()){ 
	      break;
	    }
      //canny_frame = frame.clone();
      canny_frame = canny_transform(frame);
      frame = lane_detect(frame);
      //imshow("canny frame",canny_frame);
      imshow("frame",frame);
      i++;
      image_name = "image_"+to_string(i)+".ppm";
      imwrite(image_name,frame);
      char c = cvWaitKey(1);
      if( c == 27 ){
      break;
      }
   }
#else
  frame = imread("lane_detection_test_image.jpeg",CV_LOAD_IMAGE_COLOR);
  Mat frame_copy = frame.clone();
  //cv::cvtColor(frame, frame_copy, CV_BGR2HSV);
  if(frame.empty()){
    printf("\nFrame is empty!");
  }

  #if NOT_FUNCTION
   frame_copy = canny_transform(frame_copy);
   HoughLinesP(frame_copy, lines, 1, CV_PI/180, 50, 50, 10);

   for( size_t i = 0; i < lines.size(); i++ )
        {
          Vec4i l = lines[i];
          line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        }
  //frame_copy = image_process_2(frame_copy);
  //inRange(frame_copy, Scalar(0, 80, 0), Scalar(180, 255, 255), frame_copy);
  #else
    frame = lane_detect(frame);
  #endif
  while(1){
    imshow("frame_copy",frame_copy);
    imshow("frame",frame);
    char c = cvWaitKey(33);
    if( c == 27 ){
      break;
    }
  }  
#endif
   printf("\nTEST COMPLETE\n");
}

