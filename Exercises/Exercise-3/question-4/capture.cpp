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

using namespace cv;
using namespace std;




int main( int argc, char** argv )
{
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    VideoCapture cap("Dark-Room-Laser-Spot.mpeg");
    Mat mat_frame;
    int i=1;
    string image_name="image_1.pgm";

    while(1)
    {
        cap  >> mat_frame;
        if(mat_frame.empty()){
            break;
        }
        mat_frame = mat_frame & Scalar(0,255,0);
        cvtColor(mat_frame,mat_frame,CV_BGR2GRAY);
        imshow("Capture Example", mat_frame);
        imwrite(image_name,mat_frame);
        i++;
        image_name = "image_"+to_string(i)+".pgm";
        
        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }
    destroyAllWindows();
};