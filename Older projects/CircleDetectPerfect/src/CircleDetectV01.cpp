// vertical line input
// to detect a perfect circle

#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

//#define MIN(a,b) (((a)<(b))?(a):(b))
//#define MAX(a,b) (((a)>(b))?(a):(b))

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)

typedef struct{
				Point sum;
				Point sum_abs;
				Point av_sum;
				Point av_abs;
				float av_angle;
				int n;
			} LineStruct;

//RNG rng(12345);
// use:
// Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255))

float calcAngle(Point delta)
{
	float angle;

	if(delta.y != 0) angle = (float)atan(((double)delta.x/-(double)delta.y));
	else angle = CV_PI/2;

	return angle;
}

int main( int argc, char* argv[] ) {
	CvCapture* capture = 0;

	capture = cvCreateFileCapture( argv[1] );

	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));

	IplImage* pgm_frame = cvCreateImage(size,IPL_DEPTH_8U,1); //PGM image

	//loop video
	int frame_count = 0;
	int frames = (int) cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);

	// for text:
	char text[50];
	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int thickness = 1;

	// for screenshot
	char screenshot[50];
	int screenshot_counter = 0;

	while( (bgr_frame=cvQueryFrame(capture)) != NULL ) {

		// loop video
		frame_count++;
		if(frame_count==frames-1)
		{
			cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,0);
			frame_count = 0;
		}

		//create grayscale image
		cvConvertImage(bgr_frame, pgm_frame,0); //convert one type to the other

		Mat gray = pgm_frame;
		Mat dst = bgr_frame;

		// make edges smoother
		GaussianBlur( gray, gray, Size(9, 9), 2, 2 );

		Canny(gray, gray, 50, 200, 3);

		vector<Vec3f> contours;
		findContours(gray.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		// The array for storing the approximation curve
		vector<vector<Point> >approx;


		for (int i = 0; i < contours.size(); i++)
		{
		    // Approximate contour with accuracy proportional
		    // to the contour perimeter
		    approxPolyDP(Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		    // Skip small or non-convex objects
		    if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx)) continue;

		    if (approx.size() >= 4 && approx.size() > 6) //circle
		    {
		    	printf("Circle detected\n");
		    	/*
		    	 // Detect and label circles
		    	 double area = contourArea(contours[i]);
		    	 Rect r = boundingRect(contours[i]);
		    	        int radius = r.width / 2;

		    	        if (abs(1 - ((double)r.width / r.height)) <= 0.2 &&
		    	            abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2)
		    	        {
		    	            setLabel(dst, "CIR", contours[i]);
		    	        }
		    	*/
		    }
		}

		 //for center reference
		 Point circle_center;
		 circle( dst, Point(size.width/2,size.height/2), 4, Scalar(255,0,0), -1, 8, 0 );

		// clear screen
		CLS();
		//printf("Lines size = %d\n",circles.size());

		// print text to the image
		//sprintf(text, "Angle = %.3f", line1.av_angle);
		//Point textOrg1(20,20);
		//putText(cdst, text, textOrg1, fontFace, fontScale,Scalar(0,255,0), thickness, 8);


		// show image
		if((!pgm_frame)||(!bgr_frame) ) break;
		cvShowImage("Edges", pgm_frame );
		cvShowImage("Result", bgr_frame );
		//imshow("Detected lines", dst);
		char c = cvWaitKey(33);
		if( c == 'p')
		{
			c = 0; //just to be sure
			while(1)
			{
				c = cvWaitKey(33);
				if( c == 'p' ) break;
				if( c == 27 ) break;
				if( c == 's' ) {
					sprintf(screenshot,"Screenshot%d.jpg",screenshot_counter);
					imwrite(screenshot,dst);
					screenshot_counter++;
				}

			}
		}
		if( c == 27 ) break;
		if( c == 's' ){
			sprintf(screenshot,"Screenshot%d.jpg",screenshot_counter);
			imwrite(screenshot,dst);
			screenshot_counter++;
		}
	}

	cvReleaseImage( &bgr_frame );
	cvReleaseImage( &pgm_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Edges" );
	cvDestroyWindow( "Result" );
	//cvDestroyWindow( "Detected lines" );

	return(0);
}
