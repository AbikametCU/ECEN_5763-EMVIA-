
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
    printf("\nEntered skeletal transform");
    imshow("Source",src);
    Mat temp;
    Mat eroded;
    Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
    Mat gray,binary,mfblur;
    int x,y;
	int sigma;
	bool finished;
    uchar A0,A1,A2,A3,A4,A5,A6,A7,A8;
	uint8_t chi;

    cvtColor(src, gray, CV_BGR2GRAY);

    printf("\nCrossed convert gray");

 // show graymap of source image and wait for input to next step


 // Use 70 negative for Moose, 150 positive for hand
 // 
 // To improve, compute a histogram here and set threshold to first peak
 //
 // For now, histogram analysis was done with GIMP
 //
    threshold(gray, binary, 115, 255, CV_THRESH_BINARY);
    binary = 255 - binary;
    printf("\nCrossed inverted bitmap conversion");
 

 // show bitmap of source image and wait for input to next step

 // To remove median filter, just replace blurr value with 1
    medianBlur(binary, mfblur, 1);
    imshow("median Blurred image",mfblur);
 // show median blur filter of source image and wait for input to next step
 //imshow("mfblur", mfblur);
 //waitKey();

 // This section of code was adapted from the following post, which was
 // based in turn on the Wikipedia description of a morphological skeleton
 //
 // http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 //
 do{
        //printf("do while of algorithm");
		finished = true;
		for(x=1;x<mfblur.rows-1;x++){
			for(y=1;y<mfblur.cols-1;y++){
                printf("\n mfblur value at index %d,%d = %d",x,y,mfblur.at<uchar>(x,y));
				sigma = 0;
				A0=mfblur.at<uchar>(x,y)==255?1:0;
                printf("\n A0:%d",A0);
				A1=mfblur.at<uchar>(x-1,y)==255?1:0;
                printf("\n A1:%d",A0);
				A2=mfblur.at<uchar>(x-1,y+1)==255?1:0;
                printf("\n A2:%d",A0);
				A3=mfblur.at<uchar>(x,y+1)==255?1:0;
                printf("\n A3:%d",A0);
				A4=mfblur.at<uchar>(x+1,y+1)==255?1:0;
                printf("\n A4:%d",A0);
				A5=mfblur.at<uchar>(x+1,y)==255?1:0;
                printf("\n A5:%d",A0);
				A6=mfblur.at<uchar>(x+1,y-1)==255?1:0;
                printf("\n A6:%d",A0);
				A7=mfblur.at<uchar>(x,y-1)==255?1:0;
                printf("\n A7:%d",A0);
				A8=mfblur.at<uchar>(x-1,y-1)==255?1:0;
                printf("\n A8:%d",A0);
				sigma = A1+A2+A3+A4+A5+A6+A7+A8;
				chi = (uint8_t)(A1!=A3)+(uint8_t)(A3!=A5)+(uint8_t)(A5!=A7)+(uint8_t)(A7!=A1)+2*(uint8_t)((A2>A1)&&(A2>A3))+(uint8_t)((A4>A3)&&(A4>A5))+(uint8_t)((A6>A5)&&(A6>A7))+(uint8_t)((A8>A7)&&(A8>A1));
				

			    if((A0==1)&&(chi==2)&&(sigma!=1)){
                    printf("\nEntered if condition");
				    mfblur.at<uchar>(x,y)=0;
				    finished=false;
			    }
            }
		
		}
	}while(finished==false);
    

    return mfblur;
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