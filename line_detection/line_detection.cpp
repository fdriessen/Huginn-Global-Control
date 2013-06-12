// ifconfig to show ip
// netstat -tulnap to show used ports and pid
// kill -KILL <pid>
// send color image, works with StreamClient03

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>

// CV includes
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

// config
#include "../common.h"
#include "../config.h"
#include "line_detection.h"

#define SHOW_WINDOW

long long debut_mesure;
long long fin_mesure;

CvCapture*	capture;
IplImage*	img_in;
IplImage*	img_pgm;
IplImage*	img_shared;

int main(int argc, char** argv)
{
	char key = 0;
#ifdef SHOW_WINDOW
	cvNamedWindow("Example3", CV_WINDOW_AUTOSIZE);
#endif

	capture = cvCreateFileCapture(argv[1]);
	if (!capture) {
		quit("cvCapture failed", 1);
	}

	img_in = cvQueryFrame(capture);
	img_pgm = cvCreateImage(cvGetSize(img_in), IPL_DEPTH_8U, 1); //gray
	img_shared = cvCreateImage(cvGetSize(img_in), IPL_DEPTH_8U, 3); //color

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));

	cvZero(img_pgm);
	cvZero(img_shared);

	/* run the streaming server as a separate thread */
//	if (pthread_create(&thread_s, NULL, streamServer, NULL)) {
//		quit("pthread_create failed.", 1);
//	}

	// for text:
#if DEBUG_LEVEL <= 1
	char text[50];
	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int thickness = 1;
#endif

	// for line detection
	Mat dst, cdst;

	while(1) {
#if PLATFORM == PLATFORM_x86
		if(key == 27)
		{
#endif
#if DEBUG_LEVEL <= 1
			printf("\n\n\n\n\n\n\n\n\n\n");
#endif
			/* get a frame from camera */
			img_in = cvQueryFrame(capture);

			//gray scale image for edge detection
			cvCvtColor(img_in, img_pgm, CV_BGR2GRAY);
			Mat src = img_pgm;
			// make edges smoother
			//medianBlur(src,src, 3);
			Canny(src, dst, 50, 200, 3); //GRAY output format
			cdst = img_in;
			//cvtColor(dst, cdst, COLOR_GRAY2BGR); //convert to color to be able to draw color lines

			vector<Vec4i> lines;
			LineStruct detected_line1;
			LineStruct detected_line2;
			detected_line1.sum = Point(0,0);
			detected_line1.sum_abs = Point(0,0);
			detected_line1.n = 0;
			detected_line1.angle = 0;
			detected_line2.sum = Point(0,0);
			detected_line2.sum_abs = Point(0,0);
			detected_line2.n = 0;
			detected_line2.angle = 0;

			HoughLinesP(dst, lines, 1, CV_PI/180, 50, 10, 10);
			// loop through all lines found in image
			for(size_t i = 0; i < lines.size(); i++)
			{
				Vec4i l = lines[i];
				LineStruct line_temp;

				line_temp.sum.x = l[2]-l[0]; // delta x
				line_temp.sum.y = l[3]-l[1]; // delta y
				// for center
				line_temp.sum_abs.x = l[2] + l[0];
				line_temp.sum_abs.y = l[3] + l[1];

				if(line_temp.sum.y < 0)
				{
					line_temp.sum_lh.x = -line_temp.sum.x;
					line_temp.sum_lh.y = -line_temp.sum.y;
				}
				else
				{
					line_temp.sum_lh.x = line_temp.sum.x;
					line_temp.sum_lh.y = line_temp.sum.y;
				}

				float angle = calcAngle(line_temp.sum);

				LineStruct *dest_line;

				// if line1 is still empty add it to line1
				if(detected_line1.n == 0)
					dest_line = &detected_line1;
				else if(belongs_to_line(angle, detected_line1.angle, ANGLE_THRESHOLD))
					dest_line = &detected_line1;
				// if line2 is empty and it doesn't belong to line1 add it to line 2
				else if(detected_line2.n == 0)
					dest_line = &detected_line2;
				else if(belongs_to_line(angle, detected_line2.angle, ANGLE_THRESHOLD))
					dest_line = &detected_line2;

				// if y difference == 0 then processed by calcAngle()
				// if x difference and y difference == 0 then reject line
				if((line_temp.sum.x != 0) && (line_temp.sum.y != 0))
				{
					dest_line->sum.x += line_temp.sum.x;
					dest_line->sum.y += line_temp.sum.y;
					dest_line->sum_abs.x += line_temp.sum_abs.x;
					dest_line->sum_abs.y += line_temp.sum_abs.y;
					dest_line->sum_lh.x += line_temp.sum_lh.x;
					dest_line->sum_lh.y += line_temp.sum_lh.y;

					dest_line->angle = calcAngle(dest_line->sum_lh);
					dest_line->n++;

#if DEBUG_LEVEL <= 1
					int current_line = 1;
					if(dest_line == &detected_line2)
						current_line = 2;

					printf("vector %d angle = %f\n", i, angle);
					printf("vector %d pos = s(%d,%d) e(%d,%d)\n", i, l[0], l[1], l[2], l[3]);
					printf("angle line %d = %f\n", current_line, dest_line->angle);

					sprintf(text, "%d", i);
					putText(cdst, text, Point(l[0], l[1]), fontFace, fontScale,Scalar(0,0,0), thickness, 8);

					if(current_line == 2)
						line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,255), 3, CV_AA);
					else
						line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
#endif
				}

			}

			// clear screen
#if DEBUG_LEVEL <= 1
			printf("Line1 size = %d\n",detected_line1.n);
			printf("Line2 size = %d\n",detected_line2.n);
#endif

			if(detected_line1.n>0)
			{
				detected_line1.av_sum.x = (int)(detected_line1.sum.x/detected_line1.n);
				detected_line1.av_sum.y = (int)(detected_line1.sum.y/detected_line1.n);
				detected_line1.av_abs.x = (int)(detected_line1.sum_abs.x/(2*detected_line1.n));
				detected_line1.av_abs.y = (int)(detected_line1.sum_abs.y/(2*detected_line1.n));
				detected_line1.av_angle = detected_line1.angle;

#if DEBUG_LEVEL <= 1 && 0
				printf("Start = (%d,%d)\nEnd = (%d,%d)\n"
					"av_abs = (%d,%d)\nav_sum = (%d,%d)\nAngle = %.3f degree\n",
					detected_line1.av_abs.x, detected_line1.av_abs.y, Point(detected_line1.av_abs + detected_line1.av_sum).x, Point(detected_line1.av_abs + detected_line1.av_sum).y,
					detected_line1.av_abs.x, detected_line1.av_abs.y, detected_line1.av_sum.x, detected_line1.av_sum.y, detected_line1.av_angle*180/CV_PI);

				// print text to the image
				sprintf(text, "Angle = %.3f", detected_line1.av_angle*180/CV_PI);
				Point textOrg1(20,20);
				putText(cdst, text, textOrg1, fontFace, fontScale,Scalar(0,255,0), thickness, 8);
				//draw average vector
				line( cdst, detected_line1.av_abs, detected_line1.av_abs + detected_line1.av_sum,Scalar(255,0,0), 3, CV_AA);
				//draw reference line
				line( cdst, Point(size.width/2,0), Point(size.width/2,size.height),Scalar(255,255,255), 2, CV_AA);
				// calculate deviation from centre
#endif
				float distance_average = detected_line1.av_abs.x;
				float centre = size.width/2;
				float deviation = centre - distance_average;
				float deviation_normalized = (deviation/size.width) * 100;

#if DEBUG_LEVEL <= 1 && 0
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
				 circle_center.y = detected_line1.av_abs.y;
				 //make dot green
				 circle( cdst, circle_center, 4, Scalar(0,255,0), -1, 8, 0 );
#endif
			}
			else
			{
#if DEBUG_LEVEL <= 1
				sprintf(text, "No line detected");
				Point textOrg1(20,20);
				putText(cdst, text, textOrg1, fontFace, fontScale, Scalar(0,0,255), thickness, 8);
#endif
			}


			/* print the width and height of the frame, needed by the client */
#if DEBUG_LEVEL <= 1
			//pthread_mutex_lock(&mutex);
			img_shared->imageData = (char *)cdst.data;
			//is_data_ready = 1;
			/* also display the video here on server */
#ifdef SHOW_WINDOW
			cvShowImage("Example3", img_shared);
#endif
			int img_shared_size = img_shared->imageSize;
			//pthread_mutex_unlock(&mutex);

			fprintf(stdout, "width:  %d\nheight: %d\nsize: %d\n", img_in->width, img_in->height, img_shared_size);
#endif
		}
#if PLATFORM == PLATFORM_x86
		key = cvWaitKey(33);
#endif
	}

	/* free memory */
	destroyAllWindows();
	quit(NULL, 0);
}

bool belongs_to_line(float vector_angle, float line_angle, float margin)
{
	bool ret = false;
	// if region of line1 crosses -90 deg
	if(line_angle - margin < -CV_PI/2)
	{
		if(vector_angle < line_angle + margin ||
				vector_angle > line_angle - margin + CV_PI)
			ret = true;
	}
	// if region of line1 crosses +90 deg
	else if(line_angle + margin > CV_PI/2)
	{
		if(vector_angle > line_angle - margin ||
				vector_angle < line_angle + margin - CV_PI)
			ret = true;
	}
	else
	{
		if(vector_angle > line_angle - margin &&
				vector_angle < line_angle + margin)
			ret = true;
	}

	return ret;
}

void draw_detected_line(Mat dst, LineStruct line)
{

}

/**
 * this function provides a way to exit nicely from the system
 */
void quit(char* msg, int retval)
{
	if (retval == 0) {
		fprintf(stdout, (msg == NULL ? "" : msg));
		fprintf(stdout, "\n");
	} else {
		fprintf(stderr, (msg == NULL ? "" : msg));
		fprintf(stderr, "\n");
	}

	if (capture) cvReleaseCapture(&capture);
	if (img_shared) cvReleaseImage(&img_shared);
	if (img_in) cvReleaseImage(&img_in);
	if (img_pgm) cvReleaseImage(&img_pgm);

	exit(retval);
}

float calcAngle(Point delta)
{
	float angle;

	if(delta.y == 0) angle = CV_PI/2;
	else if(delta.x == 0) angle = 0;
	else angle = (float)atan(((double)delta.x/-(double)delta.y));

	return angle;
}

long long getTimeMillis()
{
	struct timeval tv;
	gettimeofday( &tv, NULL );

	long long l = tv.tv_usec / 1000;
	l += tv.tv_sec * 1000;

	return l;
}

double calculeFrameRate()
{
    fin_mesure = getTimeMillis();

    double fps = 1000.0 / (fin_mesure - debut_mesure);

    debut_mesure = fin_mesure;

    return fps;
}


