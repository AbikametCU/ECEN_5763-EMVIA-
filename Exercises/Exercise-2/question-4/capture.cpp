#include "opencv2/imgproc/imgproc.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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

    

int main( int argc, char** argv )
{
    
    //cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    //CvCapture* capture = cvCreateCameraCapture(0);
    //namedWindow( window_name, CV_WINDOW_AUTOSIZE );
    VideoCapture capture1(0);
    IplImage* frame;
    Mat mat_frame;
    char c,c2='a';

    while(1)
    {
        capture1  >> mat_frame; 
        c = (int)cvWaitKey(33);
        if(c >0 ){
          c2=c;
          printf("c2:%d",c2);
        }
        if(mat_frame.empty()){
         break;
        }
         
          if(c2 == 's'){
          mat_frame = sobel_transform(mat_frame);
          }
          else if(c2 == 'c'){
          mat_frame = canny_transform(mat_frame);
          }
        
       
        imshow("Frame",mat_frame);
        if( c == 27 ) break;
    }
    capture1.release();
    
};