/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CUSTOM_FUNCTION 0

using namespace cv;
using namespace std;

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

char difftext[50];

int main( int argc, char** argv )
{
    cvNamedWindow("mat_frame", CV_WINDOW_AUTOSIZE);
    VideoCapture cap("q4outputVideo.mp4");
    Mat mat_frame,mat_frame_blurred;
    double x_bar,y_bar;
    struct edges edges_1 ={0,0,0,0};
    int i=1;
    Rect r;
    string image_name="image_1.pgm";
    while(1)
    {
        cap  >> mat_frame;
        cvtColor(mat_frame,mat_frame,CV_BGR2GRAY);

        medianBlur(mat_frame,mat_frame,3);
        mat_frame = custom_threshold(mat_frame);
        edges_1=raster_image(mat_frame);

        //findContours( contourOutput, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
        
        x_bar=(edges_1.x_right-edges_1.x_left)/2+edges_1.x_left;
        y_bar=(edges_1.y_bottom-edges_1.y_top)/2+edges_1.y_top;
        printf("\nx_bar:%f y_bar:%f",x_bar,y_bar);
        sprintf(difftext, "x_bar:%f y_bar:%f",x_bar,y_bar);
        line(mat_frame,Point(edges_1.x_left-30,y_bar),Point(edges_1.x_right+30,y_bar),255,3,8);
        line(mat_frame,Point(x_bar,edges_1.y_top-30),Point(x_bar,edges_1.y_bottom+30),255,3,8);
        cv::putText(mat_frame, difftext, cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, 255, 1, CV_AA);
        imshow("mat_frame", mat_frame);
        image_name = "image_"+to_string(i)+".pgm";
        imwrite(image_name,mat_frame);
        i++;
        char c = cvWaitKey(33);
        if( c == 27 ) {
            imwrite("last_saved_image.pgm",mat_frame);
            //imshow("mat_frame", mat_frame);
            break;
        }
    }
    cvDestroyWindow("mat_frame"); 
};