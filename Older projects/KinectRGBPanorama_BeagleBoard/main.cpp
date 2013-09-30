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

#define IMAGE_HEIGHT	640 // 320	640
#define IMAGE_WIDTH		480 // 240	480

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
	if (freenect_sync_get_depth((void**)&data, &timestamp, index, FREENECT_DEPTH_11BIT_PACKED))
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
 	freenect_context *f_ctx;
	freenect_device *f_dev;
	
	if(!argv[1]){ printf("specify frame rate", 1); exit(EXIT_FAILURE);}
	//if(((float)atoi(argv[1])<1)||((float)atoi(argv[1])>9)) quit("frame rate should be an int 1 ... 10", 1);
	int framerate = (int)atoi(argv[1]);
	int usleep_time_main = (1000000/framerate)-10000;
	
	int nr_screenshots;
	if(argv[2]) nr_screenshots = (int)atoi(argv[2]);
	else nr_screenshots = 100;
	
	printf("Started main\n");
	
	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		exit(EXIT_FAILURE);
	}
	
	
	freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	//freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);
	
	
	int user_device_number = 0;

	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return 1;
	}
	
	freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT_PACKED));
	
	printf("Init finished\n");
	
	while (1) {
		
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
		screenshot_counter++;
		
		CLS();
		printf("fps : %.2f\n", calculeFrameRate());
		
		usleep(usleep_time_main);
		
		if(screenshot_counter==nr_screenshots) break;
		
	}
	return 0;
}