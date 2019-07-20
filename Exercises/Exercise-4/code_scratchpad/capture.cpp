#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

unsigned char imagebuffer[1440*2560*3];

Mat custom_median_filter(Mat basicimage){
  Mat filter_image(1920, 1080, CV_8UC1, imagebuffer);
  int x=0,y=0,histogram[255],sum,i;
  for(x=0;x<filter_image.rows;x++){
    for(y=0;y<filter_image.cols;y++){
        sum=0,i=0;
        //updating the frequency of the intensity using histogram
        histogram[basicimage.at<uchar>(x-1,y-1)]++;
        histogram[basicimage.at<uchar>(x,y-1)]++;
        histogram[basicimage.at<uchar>(x+1,y+1)]++;
        histogram[basicimage.at<uchar>(x+1,y)]++;
        histogram[basicimage.at<uchar>(x+1,y+1)]++;
        histogram[basicimage.at<uchar>(x,y+1)]++;
        histogram[basicimage.at<uchar>(x-1,y+1)]++;
        histogram[basicimage.at<uchar>(x-1,y)]++;

        //finding median value which will be larger than atleast 4 neighbouring pixel values
        while(sum<5){
          sum+=histogram[i];
          i=i+1;
        }
        //Updating median value on the pixel
        filter_image.at<uchar>(x,y)=i-1;
  
        //Clearing histogram for next cycle
        histogram[basicimage.at<uchar>(x-1,y-1)]=0;
        histogram[basicimage.at<uchar>(x,y-1)]=0;
        histogram[basicimage.at<uchar>(x+1,y+1)]=0;
        histogram[basicimage.at<uchar>(x+1,y)]=0;
        histogram[basicimage.at<uchar>(x+1,y+1)]=0;
        histogram[basicimage.at<uchar>(x,y+1)]=0;
        histogram[basicimage.at<uchar>(x-1,y+1)]=0;
        histogram[basicimage.at<uchar>(x-1,y)]=0;

    }
  }

 return filter_image;
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

int main( int argc, char** argv )
{
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    //cvNamedWindow("binary", CV_WINDOW_AUTOSIZE);
    int frameCount;
    VideoCapture cap(0);
    bool done;
    int iterations;
    Mat mat_frame,mat_gray,mat_diff,mat_gray_prev;
    Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
    Mat temp;
    Mat eroded;
    cap >> mat_frame;
    cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
    mat_diff = mat_gray.clone();
    mat_gray_prev = mat_gray.clone();

    while(1)
    {
        cap >> mat_frame;
        if(mat_frame.empty()){
            break;
        }
        cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
        absdiff(mat_gray_prev, mat_gray, mat_diff);
        //mat_diff = custom_median_filter(mat_diff);
        mat_diff = custom_threshold(mat_diff);
        medianBlur(mat_diff,mat_diff,3);
        dilate(mat_diff, mat_diff, element);
        //mat_diff=255-mat_diff;
        imshow("Capture Example", mat_diff);
        char c = cvWaitKey(33);
        if( c == 27 ){
             break;
        }
        mat_gray_prev = mat_gray.clone();
    }    
    cvDestroyWindow("Capture Example");   
};