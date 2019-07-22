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
    Mat binary;

    while(1)
    {
        cap >> mat_frame;
        if(mat_frame.empty()){
            break;
        }
        cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
        threshold(mat_diff, binary, 115, 255, CV_THRESH_BINARY);
        absdiff(mat_gray_prev, mat_gray, mat_diff);
        
        binary = 255 - binary;
        medianBlur(binary,mat_diff,3);
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