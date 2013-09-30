// vertical line input

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

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)
#define ABS(a)	(((a)<(0))?(-1*a):(a))

typedef struct{
				Point sum;
				Point sum_abs;
				Point av_sum;
				Point av_abs;
				float angle;
				float av_angle;
				float prev_angle;
				int n;
			} LineStruct;

float calcAngle(Point delta)
{
	float angle;

	if(delta.y == 0) angle = CV_PI/2;
	else if(delta.x == 0) angle = 0;
	else angle = (float)atan(((double)delta.x/-(double)delta.y));

	return angle;
}

int main( int argc, char* argv[] ) {
	CvCapture* capture = 0;

	capture = cvCreateFileCapture( argv[1] );

	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));

	IplImage* pgm_frame = cvCreateImage(size,IPL_DEPTH_8U,1); //PGM image

	//IplImage* bw_frame = cvCreateImage(cvGetSize(pgm_frame),IPL_DEPTH_8U,1);

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

	// for line detection
	Mat dst, cdst;

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

		//for canny
		Mat src = pgm_frame; //can also be bgr_frame, but that is less accurate
		//Mat src = bw_frame;

		// for BW image
		//cvThreshold(pgm_frame, bw_frame, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		//cvAdaptiveThreshold(pgm_frame, bw_frame, 255,CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
		// change threshold to reduce camera disturbance... or use gausian blur?
		//cvThreshold(pgm_frame, bw_frame, 250, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		// http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html

		// make edges smoother
		GaussianBlur(src,src,Size(5,5),0);

		//erode(src,src,0,1); ?

		Canny(src, dst, 50, 200, 3); //GRAY output format
		cvtColor(dst, cdst, COLOR_GRAY2BGR); //convert to color to be able to draw color lines

		//Mat bw_mat;
		//Apply blur to smooth edges and use adapative thresholding
		//GaussianBlur(dst,bw_mat,Size(3,3),0);
		//adaptiveThreshold(bw_mat, bw_mat,255,CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,75,10);
		//bitwise_not(bw_mat, bw_mat);

		vector<Vec4i> lines;
		LineStruct line1, line2;

		line1.sum = Point(0,0);
		line1.sum_abs = Point(0,0);
		line1.n = 0;
		line1.angle = 0;
		line2.sum = Point(0,0);
		line2.sum_abs = Point(0,0);
		line2.n = 0;
		line2.angle = 0;

		bool first_entry = TRUE;

		//HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
		//HoughLinesP(dst, lines, 1, CV_PI/180, 10, 5, 50); //better line detection
		HoughLinesP(dst, lines, 1, CV_PI/180, 50, 10, 20);
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];

			LineStruct line0;
			float angle;

			// classify lines by angle

			line0.sum.x += l[2]-l[0];
			line0.sum.y += l[3]-l[1];
			// for center
			line0.sum_abs.x += l[2] + l[0];
			line0.sum_abs.y += l[3] + l[1];

			// if y difference == 0 then processed by calcAngle()
			// if x difference and y difference == 0 then reject line
			if((line0.sum.x != 0)&&(line0.sum.y != 0))
			{
				angle = calcAngle(line0.sum);
				//sperate lines based on angle
				if(first_entry)
				{
					first_entry = FALSE;
					line1.sum.x += line0.sum.x;
					line1.sum.y += line0.sum.y;
					line1.sum_abs.x += line0.sum_abs.x;
					line1.sum_abs.y += line0.sum_abs.y;
					line1.angle += angle;
					line1.prev_angle = angle;
					line1.n++;

				}

				if(ABS(angle-line1.prev_angle)>(CV_PI/2)-0.2) // within range of line 1
				{
					line1.sum.x += line0.sum.x;
					line1.sum.y += line0.sum.y;
					line1.sum_abs.x += line0.sum_abs.x;
					line1.sum_abs.y += line0.sum_abs.y;
					line1.angle += angle;
					line1.prev_angle = angle;
					line1.n++;
				}
				else // outside range of line 1
				{
					line2.sum.x += line0.sum.x;
					line2.sum.y += line0.sum.y;
					line2.sum_abs.x += line0.sum_abs.x;
					line2.sum_abs.y += line0.sum_abs.y;
					line2.angle += angle;
					line2.prev_angle = angle;
					line2.n++;
				}

				line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
			}
			// else do nothing with this line

		}

		// clear screen
		CLS();
		printf("Lines size = %d\n",line1.n);

		if(line1.n>0)
		{
			line1.av_sum.x = (int)(line1.sum.x/line1.n);
			line1.av_sum.y  = (int)(line1.sum.y/line1.n);
			line1.av_abs.x = (int)(line1.sum_abs.x/(2*line1.n));
			line1.av_abs.y = (int)(line1.sum_abs.y/(2*line1.n));
			line1.av_angle = (line1.angle/line1.n);

			printf("Start = (%d,%d)\nEnd = (%d,%d)\n"
					"av_abs = (%d,%d)\nav_sum = (%d,%d)\nAngle = %.3f degree\n",
					line1.av_abs.x, line1.av_abs.y, Point(line1.av_abs + line1.av_sum).x, Point(line1.av_abs + line1.av_sum).y,
					line1.av_abs.x, line1.av_abs.y, line1.av_sum.x, line1.av_sum.y, line1.av_angle*180/CV_PI);

			// print text to the image
			sprintf(text, "Angle = %.3f", line1.av_angle*180/CV_PI);
			Point textOrg1(20,20);
			putText(cdst, text, textOrg1, fontFace, fontScale,Scalar(0,255,0), thickness, 8);

			//draw average vector
			line( cdst, line1.av_abs, line1.av_abs + line1.av_sum,Scalar(255,0,0), 3, CV_AA);

			//draw reference line
			line( cdst, Point(size.width/2,0), Point(size.width/2,size.height),Scalar(255,255,255), 2, CV_AA);

			 // calculate deviation from centre
			 float distance_average = line1.av_abs.x;
			 float centre = size.width/2;
			 float deviation = centre - distance_average;
			 float deviation_normalized = (deviation/size.width) * 100;

			 printf("Distance average = %.3f\nCentre = %.3f\nDeviation = %.3f = %.3f [percent]\n",
				 distance_average, centre, deviation, deviation_normalized);

			 // print two dots to the image
			 Point circle_center;

			 // indicate center:
			 circle_center.x = size.width/2;
			 circle_center.y = size.height/2;

			 //make dot white for reference
			 circle( cdst, circle_center, 4, Scalar(255,255,255), -1, 8, 0 );

			 // indicate calculated line center
			 circle_center.x = distance_average;
			 circle_center.y = line1.av_abs.y;

			 //make dot green
			 circle( cdst, circle_center, 4, Scalar(0,255,0), -1, 8, 0 );


				 /*
				sprintf(text, "Deviation = %.2f = %.2f percent", deviation, deviation_normalized);
				Point textOrg2(20,size.height - 40);
				putText(cdst, text, textOrg2, fontFace, fontScale,Scalar(0,255,0), thickness, 8);
				*/
			}
			else
			{
				sprintf(text, "No line detected");
				Point textOrg1(20,20);
				putText(cdst, text, textOrg1, fontFace, fontScale,Scalar(0,0,255), thickness, 8);

			}

		// show image
		if((!pgm_frame)||(!bgr_frame) ) break;
		cvShowImage("Gray video", pgm_frame );
		cvShowImage("Original video", bgr_frame );
		//cvShowImage("Black and white", bw_frame );
		imshow("Detected lines", cdst);
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
					imwrite(screenshot,cdst);
					screenshot_counter++;
				}

			}
		}
		if( c == 27 ) break;
		if( c == 's' ){
			sprintf(screenshot,"Screenshot%d.jpg",screenshot_counter);
			imwrite(screenshot,cdst);
			screenshot_counter++;
		}
	}

	cvReleaseImage( &bgr_frame );
	cvReleaseImage( &pgm_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Gray video" );
	cvDestroyWindow( "Original video" );
	cvDestroyWindow( "Detected lines" );
	cvDestroyWindow( "Detected corners" );
	//cvDestroyWindow( "Black and white" );

	return(0);
}
