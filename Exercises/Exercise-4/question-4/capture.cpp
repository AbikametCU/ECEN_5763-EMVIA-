
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <syslog.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


string image_name_2;

void custom_skeletal_thinning(Mat mfblur,int i){
    int x,y;
	int sigma;
	bool finished;
    bool A0,A1,A2,A3,A4,A5,A6,A7,A8;
	uint8_t chi;

    do{
		finished = true;
		for(x=1;x<mfblur.rows-1;x++){
			for(y=1;y<mfblur.cols-1;y++){
				sigma = 0;
				A0=mfblur.at<uchar>(x,y)==255?1:0;
				A1=mfblur.at<uchar>(x+1,y)==255?1:0;
				A2=mfblur.at<uchar>(x+1,y-1)==255?1:0;
				A3=mfblur.at<uchar>(x,y-1)==255?1:0;
				A4=mfblur.at<uchar>(x-1,y-1)==255?1:0;
				A5=mfblur.at<uchar>(x-1,y)==255?1:0;
				A6=mfblur.at<uchar>(x-1,y+1)==255?1:0;
				A7=mfblur.at<uchar>(x,y+1)==255?1:0;
				A8=mfblur.at<uchar>(x+1,y+1)==255?1:0;
				sigma = A1+A2+A3+A4+A5+A6+A7+A8; 
				chi = (int)(A1!=A3)+(int)(A3!=A5)+(int)(A5!=A7)+(int)(A7!=A1)+2*((int)((A2>A1)&&(A2>A3))+(int)((A4>A3)&&(A4>A5))+(int)((A6>A5)&&(A6>A7))+(int)((A8>A7)&&(A8>A1)));

                if((A0==1)&&(chi==2)&&(sigma!=1)){
				    mfblur.at<uchar>(x,y)=0;
				    finished=false;
			    }
			}	
		}
	}while(!finished);
    image_name_2 = "abs_image_"+to_string(i)+".pgm";
    imshow("skele",mfblur);
    imwrite(image_name_2,mfblur);
    return ;
}





int main( int argc, char** argv )
{
    VideoCapture cap(0);
    Mat frame,skel;
    int frames_count = 0;
    string image_name = "image_1";
    int i=0;
    Mat gray,binary,mfblur,mat_diff, mat_gray_prev,mat_current_gray;
    int black_pixel_count = 0;
    int white_pixel_count = 0;

    int x,y;
	int sigma;
	bool finished;
    bool A0,A1,A2,A3,A4,A5,A6,A7,A8;
	uint8_t chi;
    cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);

    cap >> frame;
    cvtColor(frame, gray, CV_BGR2GRAY);
    mat_gray_prev = gray.clone();
    mat_diff = gray.clone();

    
    while(1)
    {
        cap >> frame;
        if(frame.empty()){
            break;
        }
        frames_count++;
        if(frames_count>300){
            break;
        }

        cvtColor(frame, gray, CV_BGR2GRAY);
        absdiff(mat_gray_prev,gray, mat_diff);
        threshold(mat_diff, binary, 115, 255, CV_THRESH_BINARY);
        binary = 255 - binary;
        medianBlur(binary, mfblur, 3);
        
        custom_skeletal_thinning(mfblur,i);
        
        char c = cvWaitKey(10);
        if( c == 27 ) {
            break;
        }
        mat_gray_prev = gray.clone();
        
        i++;
    }
}



