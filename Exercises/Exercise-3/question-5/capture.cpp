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

Mat custom_threshold_filter(Mat mat_frame){
  //Mat filter_image(VRES_ROWS, HRES_COLS, CV_8UC1, imagebuffer);
  int x=0,y=0,histogram[255],sum,i;
  for(x=0;x<mat_frame.rows;x++){
    for(y=0;y<mat_frame.cols;y++){
        if(x>5){
            break;
        }
        sum=0,i=0;
        printf("basic image fray value @ %d,%d is %d",x,y,(int)mat_frame.at<uchar>(x,y));
        printf("\n");
        //updating the frequency of the intensity using histogram
        histogram[mat_frame.at<uchar>(x-1,y-1)]++;
        //printf("\nhistogram at %d,%d=%d value = %d",x-1,y-1,histogram[mat_frame.at<uchar>(x-1,y-1)],mat_frame.at<uchar>(x-1,y-1));
        histogram[mat_frame.at<uchar>(x-1,y-1)]++;
        //printf("\nhistogram at %d,%d=%d value = %d",x,y-1,histogram[mat_frame.at<uchar>(x,y-1)],mat_frame.at<uchar>(x-1,y-1));
        histogram[mat_frame.at<uchar>(x+1,y+1)]++;
         //printf("\nhistogram at %d,%d=%d value = %d",x+1,y+1,histogram[mat_frame.at<uchar>(x+1,y+1)],mat_frame.at<uchar>(x+1,y+1));
        histogram[mat_frame.at<uchar>(x+1,y)]++;
         //printf("\nhistogram at %d,%d=%d value = %d",x+1,y,histogram[mat_frame.at<uchar>(x+1,y)],mat_frame.at<uchar>(x+1,y));
        histogram[mat_frame.at<uchar>(x+1,y+1)]++;
        //printf("\nhistogram at %d,%d=%d value = %d",x+1,y+1,histogram[mat_frame.at<uchar>(x+1,y+1)],mat_frame.at<uchar>(x+1,y+1));
        histogram[mat_frame.at<uchar>(x,y+1)]++;
         //printf("\nhistogram at %d,%d=%d value = %d",x,y+1, histogram[mat_frame.at<uchar>(x,y+1)],mat_frame.at<uchar>(x,y+1));
        histogram[mat_frame.at<uchar>(x-1,y+1)]++;
        // printf("\nhistogram at %d,%d=%d value = %d",x-1,y+1,histogram[mat_frame.at<uchar>(x-1,y+1)],mat_frame.at<uchar>(x-1,y+1));
        histogram[mat_frame.at<uchar>(x-1,y)]++;
         //printf("\nhistogram at %d,%d=%d value = %d",x-1,y, histogram[mat_frame.at<uchar>(x-1,y)],mat_frame.at<uchar>(x-1,y));

        //finding median value which will be larger than atleast 4 neighbouring pixel values
        while(i<255){
          sum+=histogram[i]>40?1:0;
          i=i+1;
        }
       
         //Updating based on neighbouring high intensity pixel values
        if(sum>5){
            mat_frame.at<uchar>(x,y)=255;
        }else{
             mat_frame.at<uchar>(x,y)=0;
        }
        //printf("\n sum value at (%d,%d):%d",x,y,sum);
  
        //Clearing histogram for next cycle
        for(i=0;i<255;i++){
                histogram[i]=0;
        }

        histogram[mat_frame.at<uchar>(x-1,y-1)]=0;
        histogram[mat_frame.at<uchar>(x,y-1)]=0;
        histogram[mat_frame.at<uchar>(x+1,y+1)]=0;
        histogram[mat_frame.at<uchar>(x+1,y)]=0;
        histogram[mat_frame.at<uchar>(x+1,y+1)]=0;
        histogram[mat_frame.at<uchar>(x,y+1)]=0;
        histogram[mat_frame.at<uchar>(x-1,y+1)]=0;
        histogram[mat_frame.at<uchar>(x-1,y)]=0;

    }
  }

 return mat_frame;
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
    for(y=0;y<mat_frame.rows;y++){
        for(x=0;x<mat_frame.cols;x++){
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
        if(mat_frame.empty()){
            printf("Empty frame");
            break;
        }
        cvtColor(mat_frame,mat_frame,CV_BGR2GRAY); 
        mat_frame = custom_threshold(mat_frame);
        

        //mat_frame = custom_threshold_filter(mat_frame); 

        medianBlur(mat_frame,mat_frame,3);
        
        //Calculate x bar and y bar based on the edge transistion co-ordinates found in raster_image
        edges_1=raster_image(mat_frame);
        x_bar=(edges_1.x_right-edges_1.x_left)/2+edges_1.x_left;
        y_bar=(edges_1.y_bottom-edges_1.y_top)/2+edges_1.y_top;
        printf("\nx_bar:%f y_bar:%f",x_bar,y_bar);
        sprintf(difftext, "x_bar:%f y_bar:%f",x_bar,y_bar);
        line(mat_frame,Point(edges_1.x_left-30,y_bar),Point(edges_1.x_right+30,y_bar),255,3,8);
        line(mat_frame,Point(x_bar,edges_1.y_top-30),Point(x_bar,edges_1.y_bottom+30),255,3,8);
        cv::putText(mat_frame, difftext, cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, 255, 1, CV_AA);
        imshow("mat_frame", mat_frame);
        //image_name = "image_"+to_string(i)+".pgm";
        //imwrite(image_name,mat_frame);
        //i++;
        char c = cvWaitKey(33);
        if( c == 27 ) {
            imwrite("last_saved_image.pgm",mat_frame);
            //imshow("mat_frame", mat_frame);
            break;
        }
    }
    cvDestroyWindow("mat_frame"); 
};
