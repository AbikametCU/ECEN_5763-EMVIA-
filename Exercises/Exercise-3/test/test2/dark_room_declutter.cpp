/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;



Mat diff_frame_compute(Mat prev_frame,Mat current_frame){
    Mat diff_frame=prev_frame.clone();
    int x,y;

    for(x=0;x<current_frame.rows;x++){
        for(y=0;y<current_frame.cols;y++){
             
            if(current_frame.at<Vec3b>(x,y)[1]<70){
                diff_frame.at<Vec3b>(x,y)[0]=current_frame.at<Vec3b>(x,y)[0]-prev_frame.at<Vec3b>(x,y)[0];
                diff_frame.at<Vec3b>(x,y)[1]=current_frame.at<Vec3b>(x,y)[1]-prev_frame.at<Vec3b>(x,y)[1];
                diff_frame.at<Vec3b>(x,y)[2]=current_frame.at<Vec3b>(x,y)[2]-prev_frame.at<Vec3b>(x,y)[2];
            }
            
            /* 
            if(current_frame.at<Vec3b>(x,y)[0]>prev_frame.at<Vec3b>(x,y)[0]){
                diff_frame.at<Vec3b>(x,y)[0]=255;
            }else{
                diff_frame.at<Vec3b>(x,y)[0]=0;
            }
            if(current_frame.at<Vec3b>(x,y)[1]>prev_frame.at<Vec3b>(x,y)[1]){
                diff_frame.at<Vec3b>(x,y)[1]=255;
            }else{
                diff_frame.at<Vec3b>(x,y)[1]=0;
            }
            if(current_frame.at<Vec3b>(x,y)[2]>prev_frame.at<Vec3b>(x,y)[2]){
                diff_frame.at<Vec3b>(x,y)[2]=255;
            }else{
                diff_frame.at<Vec3b>(x,y)[2]=0;
            }
            */
            
        }
    }

    return diff_frame;
}


Mat diff_frame_compute_grayscale(Mat prev_frame,Mat current_frame){
    printf("\nEntered gray scale compute");
    Mat diff_frame=prev_frame.clone();
    int x,y;

    for(x=0;x<current_frame.rows;x++){
        for(y=0;y<current_frame.cols;y++){
            diff_frame.at<Vec3b>(x,y)=current_frame.at<Vec3b>(x,y)-prev_frame.at<Vec3b>(x,y);
        }
    }

    return diff_frame;
}


int main( int argc, char** argv )
{
    printf("\nEntered main");
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    //CvCapture* capture = cvCreateCameraCapture(0);
    
    Mat current_frame,prev_frame,diff_frame;
    Mat current_frame_gray,prev_frame_gray,diff_frame_gray;


    VideoCapture cap("Dark-Room-Laser-Spot-with-Clutter.mpeg");
    VideoWriter video("Dark-Room-Laser-Spot-with-Clutter_modified.avi",CV_FOURCC('M','J','P','G'),10, Size(1920,1080));

    cap >> current_frame;

    printf("\ncrossed first cap >> current_frame");

    diff_frame=current_frame.clone();
     
    //*diff_frame=current_frame.clone();
    //prev_frame = current_frame.clone();
    //cvtColor(mat_frame, mat_gray, CV_BGR2GRAY);
    cv::cvtColor(current_frame, current_frame_gray, CV_BGR2GRAY);
    cv::cvtColor(current_frame, prev_frame_gray, CV_BGR2GRAY);

    prev_frame_gray=current_frame_gray;
    //cv::cvtColor(prev_frame_gray, prev_frame_gray, CV_BGR2GRAY);
    //diff_frame_gray=prev_frame.clone();
    printf("\ncrossed clone");
    diff_frame=diff_frame_compute(current_frame,prev_frame);
    //diff_frame_gray=diff_frame_compute_grayscale(current_frame_gray,prev_frame_gray);

    while(1)
    {

        printf("\n***************Entering while lopp *********************");

        cap >> current_frame;
        printf("\ncrossed while loop cap >> current_frame");
        diff_frame=diff_frame_compute(current_frame,prev_frame);
        //cv::cvtColor(current_frame, current_frame_gray, CV_BGR2GRAY);
        printf("\ncrossed cvt color");

        
        //diff_frame_gray = diff_frame_compute_grayscale(prev_frame_gray,current_frame_gray);
        printf("\ncrossed diff frame");
        if(current_frame_gray.empty()){ 
            printf("End of recording?");
            break;
        }
        imshow("Capture Example",diff_frame);
        video.write(diff_frame);
        //printf("\ncaptured frame");

        char c = cvWaitKey(33);
        if( c == 27 ){ 
            break;
        }
         prev_frame = current_frame.clone();
        // prev_frame_gray=current_frame_gray.clone();
    }

    cap.release();
    video.release();
    
}