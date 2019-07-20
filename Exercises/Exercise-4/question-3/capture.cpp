
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

Mat skeletal_transform(Mat src){
    
    Mat temp;
    Mat eroded;
    Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
    Mat gray,binary,mfblur;

    cvtColor(src, gray, CV_BGR2GRAY);

 // show graymap of source image and wait for input to next step


 // Use 70 negative for Moose, 150 positive for hand
 // 
 // To improve, compute a histogram here and set threshold to first peak
 //
 // For now, histogram analysis was done with GIMP
 //
    threshold(gray, binary, 115, 255, CV_THRESH_BINARY);
    binary = 255 - binary;
 

 // show bitmap of source image and wait for input to next step

 // To remove median filter, just replace blurr value with 1
    medianBlur(binary, mfblur, 1);

 // show median blur filter of source image and wait for input to next step
 //imshow("mfblur", mfblur);
 //waitKey();

 // This section of code was adapted from the following post, which was
 // based in turn on the Wikipedia description of a morphological skeleton
 //
 // http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 //
 
 bool done;
 int iterations=0;
 Mat skel(mfblur.size(), CV_8UC1, Scalar(0));

 do
 {
   erode(mfblur, eroded, element);
   dilate(eroded, temp, element);
   subtract(mfblur, temp, temp);
   imshow("subtracted image",temp);
   bitwise_or(skel, temp, skel);
   eroded.copyTo(mfblur);

   done = (countNonZero(mfblur) == 0);
   iterations++;
 
 } while (!done && (iterations < 100));

 cout << "iterations=" << iterations << endl;
    

    return skel;
}


int main( int argc, char** argv )
{
    //cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    VideoCapture cap(0);
    Mat frame,skel;
    int frames_count = 0;

    while(1)
    {
        cap >> frame;
        if(frame.empty()){
            break;
        }
        frames_count++;
        if(frames_count>100){
            break;
        }

        skel = skeletal_transform(frame); 
        
        imshow("skeleton", skel);


        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }
    
    //cvReleaseCapture(&capture);
    cvDestroyWindow("Capture Example");
    
}