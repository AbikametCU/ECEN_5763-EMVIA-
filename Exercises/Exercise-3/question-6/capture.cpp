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
            if(mat_frame.at<uchar>(x,y)>100){
                mat_frame.at<uchar>(x,y)=255;
          
            }else{
             mat_frame.at<uchar>(x,y)=0;
            }
      } 
   }
    return mat_frame;
}

Mat salt_noise_removal(Mat mat_frame){
    int x,y,sum=0;
    for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){

            if(mat_frame.at<uchar>(x,y)=255){
                
                sum+=mat_frame.at<uchar>(x-1,y-1)>250?1:0;
                sum+=mat_frame.at<uchar>(x,y-1)>250?1:0;
                sum+=mat_frame.at<uchar>(x+1,y+1>250?1:0);
                sum+=mat_frame.at<uchar>(x+1,y)>250?1:0;
                sum+=mat_frame.at<uchar>(x+1,y+1)>250?1:0;
                sum+=mat_frame.at<uchar>(x,y+1)>250?1:0;
                sum+=mat_frame.at<uchar>(x-1,y+1)>250?1:0;
                sum+=mat_frame.at<uchar>(x-1,y)>250?1:0;

                if(sum>4){
                    mat_frame.at<uchar>(x,y)=0;
                }
                
            }
      } 
   }
    return mat_frame;
}



Mat custom_rgb_threshold(Mat mat_frame){
    int x,y;
    printf("\nno of channels in rgb threshold:%d",mat_frame.channels());
    for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){
            
            if(mat_frame.at<Vec3b>(x,y)[0]>160){
                mat_frame.at<Vec3b>(x,y)[0]=255;
          
            }else{
             mat_frame.at<Vec3b>(x,y)[0]=0;
            }
            if(mat_frame.at<Vec3b>(x,y)[1]>254){
                mat_frame.at<Vec3b>(x,y)[1]=255;
          
            }else{
             mat_frame.at<Vec3b>(x,y)[1]=0;
            }
            if(mat_frame.at<Vec3b>(x,y)[2]>254){
                mat_frame.at<Vec3b>(x,y)[2]=255;
          
            }else{
             mat_frame.at<Vec3b>(x,y)[2]=0;
            }
      } 
   }
    return mat_frame;
}

Mat red_green_intensity_flusher(Mat mat_frame){
    int x,y;
    printf("\nno of channels in rgb threshold:%d",mat_frame.channels());
    for(x=0;x<mat_frame.rows;x++){
        for(y=0;y<mat_frame.cols;y++){
            mat_frame.at<Vec3b>(x,y)[1]=mat_frame.at<Vec3b>(x,y)[1]-50;        
            mat_frame.at<Vec3b>(x,y)[2]=mat_frame.at<Vec3b>(x,y)[2]-50;  
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
    Mat rgb_threshold_frame;
    double x_bar,y_bar;
    //VideoCapture cap("Light-Room-Laser-Spot-with-Clutter (1).mpeg");
    VideoCapture cap("Dark-Room-Laser-Spot-with-Clutter.mpeg");
    //VideoWriter video("Dark-Room-Laser-Spot-with-Clutter_modified.avi",CV_FOURCC('M','J','P','G'),10, Size(1920,1080));
    struct edges edges_1 ={0,0,0,0};
	
    cap >> mat_frame;
    cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
    //cvNamedWindow("mat_frame", CV_WINDOW_AUTOSIZE);

    mat_diff = mat_gray;
    mat_gray_prev = mat_gray;


    while(1)
    {
	
	cap >> mat_frame;
    //imshow("mat_frame_normal",mat_frame);
    mat_frame =mat_frame & Scalar(0,255,0);
    //rgb_threshold_frame=custom_rgb_threshold(mat_frame); 
    //imshow("rgb threshold output",rgb_threshold_frame);  
   // mat_frame = red_green_intensity_flusher(mat_frame);
    //imshow("mat_frame after red intensity flush",mat_frame);
    mat_frame=custom_rgb_threshold(mat_frame);
    //rgb_threshold_frame =mat_frame & Scalar(0,255,0);
    
	cv::cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
    imshow("mat_frame_gray",mat_gray);
    mat_gray=custom_threshold(mat_gray);
    //imshow("mat_frame_gray after threshold",mat_gray);
    //medianBlur(mat_gray,mat_gray,3);
    GaussianBlur(mat_gray,mat_gray,Size(3,3),3);
    addWeighted(mat_gray,1.5,mat_gray,-0.5,0,mat_gray);
    imshow("mat_frame_gray after threshold and then median blur",mat_gray);
	absdiff(mat_gray_prev, mat_gray, mat_diff);
    imshow("Mat_diff before raster",mat_diff);
   
   
    medianBlur(mat_diff,mat_diff,3);
    mat_diff = custom_threshold(mat_diff);
    //mat_diff = salt_noise_removal(mat_diff);
    
    edges_1=raster_image(mat_gray);
    x_bar=(edges_1.x_right-edges_1.x_left)/2+edges_1.x_left;
    y_bar=(edges_1.y_bottom-edges_1.y_top)/2+edges_1.y_top;
    printf("\nx_bar:%f y_bar:%f",x_bar,y_bar);
    sprintf(difftext, "x_bar:%f y_bar:%f",x_bar,y_bar);
    line(mat_diff,Point(edges_1.x_left-30,y_bar),Point(edges_1.x_right+30,y_bar),255,3,8);
    line(mat_diff,Point(x_bar,edges_1.y_top-30),Point(x_bar,edges_1.y_bottom+30),255,3,8);
    

    imshow("mat_diff",mat_diff);
   
    //imshow("mat diff blue",diff_frame_blur);
   // video << mat_diff;
    //printf("\n capturing frame");


        char c = cvWaitKey(33); 
        if( c == 'q' ) break;

     mat_gray_prev = mat_gray.clone();
    }

};