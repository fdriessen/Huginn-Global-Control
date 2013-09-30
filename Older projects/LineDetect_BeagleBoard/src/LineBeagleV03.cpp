// ifconfig to show ip
// netstat -tulnap to show used ports and pid
// kill -KILL <pid>
// send color image, works with StreamClient03

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
#include <iostream>

using namespace cv;
using namespace std;

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)

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

#define PORT 8888

CvCapture*	capture;
IplImage*	img_in;
IplImage*	img_pgm;
IplImage*	img_shared;
int			is_data_ready = 0;
int			serversock, clientsock;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* streamServer(void* arg);
void  quit(char* msg, int retval);

int main(int argc, char** argv)
{
		pthread_t 	thread_s;
		int			key;

		CvCapture* capture = cvCreateFileCapture(argv[1]);

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
		if (pthread_create(&thread_s, NULL, streamServer, NULL)) {
			quit("pthread_create failed.", 1);
		}

		// for text:
		char text[50];
		int fontFace = FONT_HERSHEY_PLAIN;
		double fontScale = 1;
		int thickness = 1;

		// for line detection
		Mat dst, cdst;

		while(1) {
			/* get a frame from camera */
			img_in = cvQueryFrame(capture);
			if (!img_in) break;

			//gray scale image for edge detection
			cvCvtColor(img_in, img_pgm, CV_BGR2GRAY);
			Mat src = img_pgm;
			// make edges smoother
			GaussianBlur(src,src,Size(5,5),0);
			Canny(src, dst, 50, 200, 3); //GRAY output format
			cdst = img_in;
			//cvtColor(dst, cdst, COLOR_GRAY2BGR); //convert to color to be able to draw color lines

			vector<Vec4i> lines;
			LineStruct line1;
			line1.sum = Point(0,0);
			line1.sum_abs = Point(0,0);
			line1.n = 0;
			line1.angle = 0;

			HoughLinesP(dst, lines, 1, CV_PI/180, 50, 10, 20);
			for( size_t i = 0; i < lines.size(); i++ )
			{
				Vec4i l = lines[i];
				LineStruct line0;
				float angle;

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
					line1.sum.x += line0.sum.x;
					line1.sum.y += line0.sum.y;
					line1.sum_abs.x += line0.sum_abs.x;
					line1.sum_abs.y += line0.sum_abs.y;
					line1.angle += angle;
					line1.prev_angle = angle;
					line1.n++;

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
			}
			else
			{
				sprintf(text, "No line detected");
				Point textOrg1(20,20);
				putText(cdst, text, textOrg1, fontFace, fontScale,Scalar(0,0,255), thickness, 8);

			}


			pthread_mutex_lock(&mutex);
				img_shared->imageData = (char *)cdst.data;
				is_data_ready = 1;
				/* also display the video here on server */
				//cvShowImage("stream_server", img_shared);
				int img_shared_size = img_shared->imageSize;
			pthread_mutex_unlock(&mutex);

			usleep(30000);

			/* print the width and height of the frame, needed by the client */
			fprintf(stdout, "width:  %d\nheight: %d\nsize: %d\n", img_in->width, img_in->height, img_shared_size);
		}

		/* user has pressed 'q', terminate the streaming server */
		if (pthread_cancel(thread_s)) {
			quit("pthread_cancel failed.", 1);
		}

	/* free memory */
	//cvDestroyWindow("stream_server");
	quit(NULL, 0);
}

/**
 * This is the streaming server, run as a separate thread
 * This function waits for a client to connect, and send the color images
 */
void* streamServer(void* arg)
{
	struct sockaddr_in server;

	/* make this thread cancellable using pthread_cancel() */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/* open socket */
	//protocol specifying the actual transport protocol to use.
	//The most common are IPPROTO_TCP, IPPROTO_SCTP, IPPROTO_UDP, IPPROTO_DCCP.
	//These protocols are specified in <netinet/in.h>.
	//The value â€œ0â€ may be used to select a default protocol from the selected domain and type.
	if ((serversock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		quit("socket() failed", 1);
	}

	/* setup server's IP and port */
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	/* bind the socket */
	if (bind(serversock,  (const sockaddr*)&server, sizeof(server)) == -1) {
		quit("bind() failed", 1);
	}

	/* wait for connection */
	if (listen(serversock, 10) == -1) {
		quit("listen() failed.", 1);
	}

	/* accept a client */
	if ((clientsock = accept(serversock, NULL, NULL)) == -1) {
		quit("accept() failed", 1);
	}

	/* the size of the data to be sent */
	//pthread_mutex_lock(&mutex);
		int imgSize = img_in->imageSize;
		//int imgSize = img_shared->imageSize;
	//pthread_mutex_unlock(&mutex);
	int bytes, i;


	/* start sending images */
	while(1)
	{
		/* send the color frame, thread safe */
		pthread_mutex_lock(&mutex);
		if (is_data_ready) {
			bytes = send(clientsock, img_shared->imageData, imgSize, 0);
			is_data_ready = 0;
		}
		pthread_mutex_unlock(&mutex);

		/* if something went wrong, restart the connection */
		if (bytes != imgSize) {
			fprintf(stderr, "Connection closed.\n");
			close(clientsock);

			if ((clientsock = accept(serversock, NULL, NULL)) == -1) {
				quit("accept() failed", 1);
			}
		}

		/* have we terminated yet? */
		pthread_testcancel();

		/* no, take a rest for a while */
		usleep(1000);

	}
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

	if (clientsock) close(clientsock);
	if (serversock) close(serversock);
	if (capture) cvReleaseCapture(&capture);
	if (img_shared) cvReleaseImage(&img_shared);
	if (img_in) cvReleaseImage(&img_in);
	if (img_pgm) cvReleaseImage(&img_pgm);

	pthread_mutex_destroy(&mutex);

	exit(retval);
}


