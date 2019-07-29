
#include "opencv2/imgproc/imgproc.hpp"
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

#define VRES_ROWS (1080)
#define HRES_COLS (1920)

unsigned char imagebuffer[1440*2560*3];

Mat custom_median_filter(Mat basicimage){
  Mat filter_image(VRES_ROWS, HRES_COLS, CV_8UC1, imagebuffer);
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



int main( int argc, char** argv )
{
    Mat basicimage(VRES_ROWS, HRES_COLS, CV_8UC3, imagebuffer);
    basicimage = imread("q3_image.ppm", CV_LOAD_IMAGE_COLOR);
    
    //Remove red and blue channels
    Mat green_image = basicimage & Scalar(0,255,0);

    Mat gray_scale_green_channel,inbuilt_median_filtered_image,custom_median_filtered_image;

    cvtColor(green_image,gray_scale_green_channel,CV_BGR2GRAY);
    
    
    custom_median_filtered_image = custom_median_filter(gray_scale_green_channel);
    medianBlur(gray_scale_green_channel,inbuilt_median_filtered_image,3);

    
    cvNamedWindow("Original Image", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Gray scale green channel image", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("opencv median blurred image", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("custom median blurred image", CV_WINDOW_AUTOSIZE);


    while(1)
    { 
       imshow("Original Image", basicimage); 
       imshow("Gray scale green channel image", gray_scale_green_channel);
       imwrite("Gray_scale_image.pgm",gray_scale_green_channel);
       imshow("opencv_median_blurred_image", inbuilt_median_filtered_image);
       imwrite("openv_median_blur_image.pgm",gray_scale_green_channel);
       imshow("custom_median_blurred_image", custom_median_filtered_image);
       imwrite("custom_median_blur_image.pgm",gray_scale_green_channel);

       char c = cvWaitKey(33);
       if( c == 27 ) break;
    }

   cvDestroyWindow("Original Image");
   cvDestroyWindow("Gray scale green channel image");
   cvDestroyWindow("opencv_median_blurred_image");
   cvDestroyWindow("custom_median_blurred_image");
};
