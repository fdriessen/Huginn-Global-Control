// https://github.com/OpenKinect/libfreenect/blob/master/wrappers/opencv/cvdemo.c
// https://github.com/OpenKinect/libfreenect/tree/master/wrappers/opencv


#include <cv.h>
//#include <opencv/highgui.h>
#include <stdio.h>
#include "libfreenect.h"
#include "libfreenect_sync.h"
#include <unistd.h>
#include <cxcore.h>
#include "highgui.h"
#include <time.h>

using namespace cv;
using namespace std;

#define IMAGE_HEIGHT	640 // 640
#define IMAGE_WIDTH		480 // 480

#define CLS() 	printf("\033[H\033[2J"); fflush(stdout)

long long getTimeMillis(){
	struct timeval tv;
	gettimeofday( &tv, NULL );

	long long l = tv.tv_usec / 1000;
	l += tv.tv_sec * 1000;

	return l;
}

long long debut_mesure;
long long fin_mesure;

double calculeFrameRate(){
    fin_mesure = getTimeMillis();

    double fps = 1000.0 / (fin_mesure - debut_mesure);

    debut_mesure = fin_mesure;

    return fps;
}

IplImage *freenect_sync_get_depth_cv(int index)
{
	static IplImage *image = 0;
	static char *data = 0;
	if (!image) image = cvCreateImageHeader(cvSize(IMAGE_HEIGHT,IMAGE_WIDTH), 16, 1);
	unsigned int timestamp;
	if (freenect_sync_get_depth((void**)&data, &timestamp, index, FREENECT_DEPTH_11BIT))
	    return NULL;
	cvSetData(image, data, IMAGE_HEIGHT*2);
	return image;
}

IplImage *freenect_sync_get_rgb_cv(int index)
{
	static IplImage *image = 0;
	static char *data = 0;
	if (!image) image = cvCreateImageHeader(cvSize(IMAGE_HEIGHT,IMAGE_WIDTH), 8, 3);
	unsigned int timestamp;
	if (freenect_sync_get_video((void**)&data, &timestamp, index, FREENECT_VIDEO_RGB))
	    return NULL;
	cvSetData(image, data, IMAGE_HEIGHT*3);
	return image;
}


IplImage *GlViewColor(IplImage *depth)
{
	static IplImage *image = 0;
	if (!image) image = cvCreateImage(cvSize(IMAGE_HEIGHT,IMAGE_WIDTH), 8, 3);
	unsigned char *depth_mid = (unsigned char*)(image->imageData);
	int i;
	for (i = 0; i < IMAGE_HEIGHT*IMAGE_WIDTH; i++) {
		int lb = ((short *)depth->imageData)[i] % 256;
		int ub = ((short *)depth->imageData)[i] / 256;
		switch (ub) {
			case 0:
				depth_mid[3*i+2] = 255;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+0] = 255-lb;
				break;
			case 1:
				depth_mid[3*i+2] = 255;
				depth_mid[3*i+1] = lb;
				depth_mid[3*i+0] = 0;
				break;
			case 2:
				depth_mid[3*i+2] = 255-lb;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+0] = 0;
				break;
			case 3:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+0] = lb;
				break;
			case 4:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+0] = 255;
				break;
			case 5:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+0] = 255-lb;
				break;
			default:
				depth_mid[3*i+2] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+0] = 0;
				break;
		}
	}
	return image;
}


int main(int argc, char **argv)
{	
	
	char screenshot[50];
	int screenshot_counter = 0;

	// pointer to the freenect context
 	freenect_context *ctx;
	
	printf("Started main\n");
	
	 if (freenect_init(&ctx, NULL) < 0)
 	{
  		cout<<"freenect_init() failed\n";
   		exit(EXIT_FAILURE);
 	}

	//freenect_set_depth_mode(kinect->dev, freenect_find_depth_mode(FREENECT_RESOLUTION_LOW, fmt));
	//freenect_set_depth_mode(0, freenect_find_depth_mode(FREENECT_RESOLUTION_LOW,FREENECT_DEPTH_11BIT));
	
	// Set the tilt angle (in degrees)
  	freenect_sync_set_tilt_degs(0, 0); //(level, device_id)
	
	printf("Init finished\n");
	
	IplImage *image = freenect_sync_get_rgb_cv(0);
		if (!image) {
		    printf("Error: Kinect not connected?\n");
		    return -1;
		}
	
	cvCvtColor(image, image, CV_RGB2BGR);
	Mat mat_image = image;
	printf("test camera snapshot\n");
	sprintf(screenshot,"Camera_screenshot%d.jpg",screenshot_counter);
	imwrite(screenshot,mat_image);
	
	IplImage *depth = freenect_sync_get_depth_cv(0);
		if (!depth) {
		    printf("Error: Kinect not connected?\n");
		    return -1;
		}

	IplImage *depth_color = GlViewColor(depth);

	Mat mat_depth_color = depth_color;

	printf("test depth snapshot\n");
	sprintf(screenshot,"Depth_sreenshot%d.jpg",screenshot_counter);
	imwrite(screenshot,mat_depth_color);
	
	// timeout to give user the chance to read the printed info
	for(int count=0; count<100; count++){
		usleep(50000);
		printf(".");
	}
	printf("\n");
	
	while (1) {
		/*
		IplImage *image = freenect_sync_get_rgb_cv(0);
		if (!image) {
		    printf("Error: Kinect not connected?\n");
		    return -1;
		}
		*/
		
		IplImage *depth = freenect_sync_get_depth_cv(0);
		if (!depth) {
		    printf("Error: Kinect not connected?\n");
		    return -1;
		}
		
		CLS();
		printf("fps : %f\n", calculeFrameRate());
		
	}
	return 0;
}