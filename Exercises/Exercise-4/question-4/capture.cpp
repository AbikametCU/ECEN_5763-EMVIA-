#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

Mat frame_copy;
Mat custom_skeletal_transform(Mat frame){
    printf("\nEntered custom skeletal transform");
    frame_copy = frame;
    int x,y;
	int sigma;
	bool finished;
    uint8_t A0;
	uint8_t A1;
	uint8_t A2;
	uint8_t A3;
	uint8_t A4;
	uint8_t A5;
	uint8_t A6;
	uint8_t A7;
	uint8_t A8;
	uint8_t chi;
	//mat_frame.at<uchar>(x,y);
	do{
		finished = true;
		for(x=0;x<frame.rows;x++){
			for(y=0;y<frame.cols;y++){
				sigma = 0;
				A0=(uint8_t)frame.at<uchar>(x,y);
				A1=(uint8_t)frame.at<uchar>(x-1,y);
				A2=(uint8_t)frame.at<uchar>(x-1,y+1);
				A3=(uint8_t)frame.at<uchar>(x,y+1);
				A4=(uint8_t)frame.at<uchar>(x+1,y+1);
				A5=(uint8_t)frame.at<uchar>(x+1,y);
				A6=(uint8_t)frame.at<uchar>(x+1,y-1);
				A7=(uint8_t)frame.at<uchar>(x,y-1);
				A8=(uint8_t)frame.at<uchar>(x-1,y-1);
				sigma = A1+A2+A3+A4+A5+A6+A7+A8;
				chi = (int)(A1!=A3)+(int)(A3!=A5)+(int)(A5!=A7)+(int)(A7!=A1)+2*(int)((A2>A1)&&(A2>A3))+(int)((A4>A3)&&(A4>A5))+(int)((A6>A5)&&(A6>A7))+(int)((A8>A7)&&(A8>A1));
				
			}	
			if((A0==1)&&(chi==2)&&(sigma!=1)){
				frame_copy.at<uchar>(x,y)=0;
				finished=false;
			}
		
		}
	}while(!finished);


	return frame_copy;
}


int main( int argc, char** argv )
{
    printf("\nEntered main");
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    VideoCapture cap(0);
    Mat frame,src,skel;
    int frames_count = 0;
	Mat temp;
    Mat eroded;
    Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
    Mat gray,binary,mfblur;
    int x,y;
	int sigma;
	bool finished;
    uchar A0,A1,A2,A3,A4,A5,A6,A7,A8;
	uint8_t chi;

    while(1)
    {
        printf("\nEntered while loop");
        cap >> src;
        printf("\ncrossed cap >> frame");
        if(frame.empty()){
            break;
        }
        printf("\ncaptured frame");

        frames_count++;
        if(frames_count>100){
            break;
        }

		cvtColor(src, gray, CV_BGR2GRAY);
		printf("\ncaptured frame");
		threshold(gray, binary, 115, 255, CV_THRESH_BINARY);
		printf("\ncaptured frame");
    	binary = 255 - binary;
		printf("\ncaptured frame");
	    medianBlur(binary, mfblur, 1);

		imshow("median blur",mfblur);
        
    
	//mat_frame.at<uchar>(x,y);
	do{
        //printf("do while of algorithm");
		finished = true;
		for(x=1;x<mfblur.rows-1;x++){
			for(y=1;y<mfblur.cols-1;y++){
				printf("\n mfblur value at index %d,%d = %d",x,y,mfblur.at<uchar>(x,y));
				sigma = 0;
				A0=mfblur.at<uchar>(x,y)==255?1:0;
				A1=mfblur.at<uchar>(x-1,y)==255?1:0;
				A2=mfblur.at<uchar>(x-1,y+1)==255?1:0;
				A3=mfblur.at<uchar>(x,y+1)==255?1:0;
				A4=mfblur.at<uchar>(x+1,y+1)==255?1:0;
				A5=mfblur.at<uchar>(x+1,y)==255?1:0;
				A6=mfblur.at<uchar>(x+1,y-1)==255?1:0;
				A7=mfblur.at<uchar>(x,y-1)==255?1:0;
				A8=mfblur.at<uchar>(x-1,y-1)==255?1:0;
				sigma = A1+A2+A3+A4+A5+A6+A7+A8;
				chi = (int)(A1!=A3)+(int)(A3!=A5)+(int)(A5!=A7)+(int)(A7!=A1)+2*(int)((A2>A1)&&(A2>A3))+(int)((A4>A3)&&(A4>A5))+(int)((A6>A5)&&(A6>A7))+(int)((A8>A7)&&(A8>A1));
				
			}	
			if((A0==1)&&(chi==2)&&(sigma!=1)){
				mfblur.at<uchar>(x,y)=0;
				finished=false;
			}
		
		}
	}while(!finished);

        //skel = custom_skeletal_transform(frame); 
        //printf("\n crossed skeletal transform");
        imshow("Capture Example", mfblur);
        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }
    //cvReleaseCapture(&capture);
    cvDestroyWindow("Capture Example");
}
