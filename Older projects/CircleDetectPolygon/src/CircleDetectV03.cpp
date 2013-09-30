/**
 * @function generalContours_demo1.cpp
 * @brief Demo code to find contours in an image
 * @author OpenCV team
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 200;
int max_thresh = 255;
RNG rng(12345);
CvSize size;

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)

/// Function header
void thresh_callback(int, void* );

int main( int, char** argv )
{
	CvCapture* capture = 0;

	capture = cvCreateFileCapture( argv[1] );

	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read

	size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));

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

		src = bgr_frame;


		/// Convert image to gray and blur it
		cvtColor( src, src_gray, COLOR_BGR2GRAY );

		// make edges smoother
		GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
		//blur( src_gray, src_gray, Size(3,3) );

		/// Create Window
		namedWindow( "Source", WINDOW_AUTOSIZE );
		imshow( "Source", src );

		createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
		thresh_callback( 0, 0 );


		// print text to the image
		//sprintf(text, "Angle = %.3f", line1.av_angle);
		//Point textOrg1(20,20);
		//putText(cdst, text, textOrg1, fontFace, fontScale,Scalar(0,255,0), thickness, 8);


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
	  					imwrite(screenshot,src);
	  					screenshot_counter++;
	  				}

	  			}
	  		}
	  		if( c == 27 ) break;
	  		if( c == 's' ){
	  			sprintf(screenshot,"Screenshot%d.jpg",screenshot_counter);
	  			imwrite(screenshot,src);
	  			screenshot_counter++;
	  		}
	}
	cvReleaseImage( &bgr_frame );
	cvReleaseImage( &pgm_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow("Source");
	cvDestroyWindow( "Contours");
	cvDestroyWindow( "Edges");
	return(0);
}

/**
 * @function thresh_callback
 */
void thresh_callback(int, void* )
{
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using Threshold
  //threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
  Canny(src_gray, threshold_output, 50, thresh, 3); //better than threshold, thresh can be statically set to 200
  /// Find contours
  findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  vector<vector<Point> > contours_poly( contours.size() );
  //vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );

  for( size_t i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       //boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       minEnclosingCircle( contours_poly[i], center[i], radius[i] );
     }

  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

        // Detect and label circles
        //double area = contourArea(contours[i]);

        //if (abs(1 - ((double)boundRect[i].width / boundRect[i].height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(boundRect[i].width/2, 2)))) <= 0.2)
        //if (abs(1 - ((double)radius[i])) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius[i]/2, 2)))) <= 0.2)
        if((radius[i]>20)&&(radius[i]<100)) //check for area as well to reject non circular objects
        {
        	//drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        	//rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        	circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
        	CLS(); // clear screen
        	printf("Radius = %.3f\n",
        	   		 radius[i]);
        }

     }

	//for center reference
	Point circle_center;
	circle( drawing, Point(size.width/2,size.height/2), 4, Scalar(255,0,0), -1, 8, 0 );

  /// Show in a window
  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
  namedWindow( "Edges", WINDOW_AUTOSIZE );
  imshow( "Edges", threshold_output );

}
