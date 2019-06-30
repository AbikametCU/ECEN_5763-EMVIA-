/*
 *
 *  Example by Sam Siewert 
 *
 *  Updated 12/6/18 for OpenCV 3.1
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

char difftext[20];

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

struct edges{
    double x_left;
    double x_right; 
    double y_top;
    double y_bottom;
};


struct edges raster_image(Mat mat_frame){
    printf("\nEntered raster image");
    int x,y;
    struct edges temp={0,0,0,0};
    int first_edge_detected=0;
    for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){
            if(first_edge_detected==0){
                if(mat_frame.at<uchar>(y,x)==255){
                    if(mat_frame.at<uchar>(y-1,x-1)==0 && mat_frame.at<uchar>(y-1,x)==0 && mat_frame.at<uchar>(y,x-1)==0){
                        first_edge_detected=1;
                        printf("\nfirst edge at x:%d y:%d",x,y);
                        temp.x_left=x;
                        temp.y_top=y; 
                    }
                
                }
            }else{

                if(mat_frame.at<uchar>(y,x)==0){
                    if(mat_frame.at<uchar>(y-1,x-1)==255 && mat_frame.at<uchar>(y-1,x)==255 && mat_frame.at<uchar>(y,x-1)==255){
                        printf("\nlast edge at x:%d y:%d",x,y); 
                        temp.x_right=x;
                        temp.y_bottom=y;
                        return temp;
                    }
                
                }

            }
       } 
   }
    return temp;
}

int main( int argc, char** argv )
{
    Mat mat_frame, mat_gray, mat_diff, mat_gray_prev,diff_frame_blur;
    VideoCapture vcap;
    unsigned int diffsum, maxdiff;
    double percent_diff;
    VideoCapture cap("Dark-Room-Laser-Spot-with-Clutter.mpeg");
    //VideoCapture cap("Light-Room-Laser-Spot-with-Clutter (1).mpeg");
    VideoWriter video("Dark-Room-Laser-Spot-with-Clutter_modified.avi",CV_FOURCC('M','J','P','G'),10, Size(1920,1080));

	
    cap >> mat_frame;
    cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
    //cvNamedWindow("mat_frame", CV_WINDOW_AUTOSIZE);

    mat_diff = mat_gray.clone();
    mat_gray_prev = mat_gray.clone();
    int i=1;
    string image_name="image_1.pgm";
    while(1)
    {
	
	cap >> mat_frame;
	cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);

	absdiff(mat_gray_prev, mat_gray, mat_diff);
    //mat_diff=custom_threshold(mat_diff);
    medianBlur(mat_diff,diff_frame_blur,15);

    image_name = "image_"+to_string(i)+".pgm";
    i++;
    imshow("mat_diff",mat_diff);
    imwrite(image_name,mat_diff);
    //imshow("mat diff blur",diff_frame_blur);
    video << mat_diff;
    //printf("\n capturing frame");


        char c = cvWaitKey(10); 
        if( c == 'q' ) break;

     mat_gray_prev = mat_gray.clone();
    }

};