// kinect functions inspired by:
// https://github.com/OpenKinect/libfreenect/blob/master/wrappers/opencv/cvdemo.c
// https://github.com/OpenKinect/libfreenect/tree/master/wrappers/opencv
// http://stackoverflow.com/questions/10747107/udp-broadcast-in-c

// if application halts and still claims the port:
// ifconfig to show ip
// netstat -tulnap to show used ports and pid
// kill -KILL <pid>

#include <cv.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "libfreenect.h"
#include "libfreenect_sync.h"
#include <cxcore.h>
#include "highgui.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace cv;
using namespace std;

#define CLS() 	printf("\033[H\033[2J"); fflush(stdout)

#define PORT 8888
#define DEPTH_WIDTH		640
#define	DEPTH_HEIGHT	480
//#define SEND_STRING_LEN	2*DEPTH_WIDTH+2

CvCapture*	capture;
IplImage*	img0;
IplImage*	img1;
IplImage*	img_interm;
int			is_data_ready = 0;
int			serversock, clientsock;
int			usleep_time_udp;

//static char* depth_data = 0;
char* depth_data; //pointer

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* streamServer(void* arg);
void  quit(char* msg, int retval);

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

int main(int argc, char **argv)
{	
	pthread_t 	thread_s;
	int		key;
	bool use_depth_image;
	int i,j;
	
	// pointer to the freenect context
 	freenect_context *ctx;
 	// pointer to the device
 	freenect_device *dev;
	
	depth_data = (char*) malloc(DEPTH_WIDTH*DEPTH_HEIGHT*2); // reserve memory, depth_data points to 0 address
	
	fprintf(stdout,"Input type = Kinect\n");
	
	 if (freenect_init(&ctx, NULL) < 0)
 	{
  		cout<<"freenect_init() failed\n";
   		exit(EXIT_FAILURE);
 	}
	
	if(!argv[1]) quit("specify frame rate", 1);
	//if(((float)atoi(argv[1])<1)||((float)atoi(argv[1])>9)) quit("frame rate should be an int 1 ... 10", 1);
	int framerate = (int)atoi(argv[1]);
	int usleep_time_main = (1000000/framerate);
	usleep_time_udp = (1000000/framerate)/3;
	
	
	// Set the tilt angle (in degrees)
  	freenect_sync_set_tilt_degs(0, 0); //(level, device_id)

	/* print the width and height of the frame, needed by the client */
	fprintf(stdout, "width:  %d\nheight: %d\n", 640, 480);
	fprintf(stdout, "PORT: %d\n\n",PORT);

	/* run the streaming server as a separate thread */
	if (pthread_create(&thread_s, NULL, streamServer, NULL)) {
		quit("pthread_create failed.", 1);
	}
	
	fprintf(stdout, "if the application halts and still claims the port use commands:\n\
		ifconfig to show ip\n\
		netstat -tulnap to show used ports and pid\n\
		kill -KILL <pid> to kill the process\n\
		or exit application with ctrl+c\n\
		The application will start in several seconds...\n");
	
	/*
	// timeout to give user the chance to read the printed info
	for(int count=0; count<10; count++){
		usleep(500000);
		fprintf(stdout,".\n");
	}	
	*/
	
	fprintf(stdout, "Start sending data\n");
	
	while(1) {

			unsigned int timestamp;
			static char *data = 0;
			
			if (freenect_sync_get_depth((void**)&data, &timestamp, 0, FREENECT_DEPTH_11BIT))
					quit("kinect() get depth failed", 1);
					
			pthread_mutex_lock(&mutex);
				memcpy(depth_data, data, DEPTH_WIDTH*DEPTH_HEIGHT*2);			
				
				is_data_ready = 1; // start sending data over UDP (handled by different thread)
			pthread_mutex_unlock(&mutex);
		
		//usleep(1000); //1 ms
		//usleep(30000); //30 ms -> 33 fps
		//usleep(200000); //200ms -> 5fps
		//usleep(190000); //190ms -> 5fps
		//usleep(300000); //300ms -> 3fps
		//usleep(1000000);
		usleep(usleep_time_main);
		CLS();		
		fprintf(stdout, "fps : %.2f\n", calculeFrameRate());

	}

	/* user has pressed 'q', terminate the streaming server */
	if (pthread_cancel(thread_s)) {
		quit("pthread_cancel failed.", 1);
	}

	quit(NULL, 0); /* free memory */

	return 0;
}


/**
 * This is the streaming server, run as a separate thread
 * This function waits for a client to connect, and send the grayscaled images
 */
void* streamServer(void* arg)
{
	int sock;                        
	struct sockaddr_in broadcastAddr; 
	char *broadcastIP;                
	unsigned short broadcastPort;     
	char StartOfFrame[10] = "start";
	char EndOfFrame[10] = "end";
	int broadcastPermission;         
	int sendStringLen;     
	int sendStringLenLast;	
	int StartOfFrameStringLen;
	int EndOfFrameStringLen;
	int j, i;
	char* temp_data;
	temp_data = (char*) malloc(DEPTH_WIDTH*2*50+2);

   broadcastIP = "255.255.255.255";  
   broadcastPort = 8888;
		
	/* make this thread cancellable using pthread_cancel() */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

   if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
       quit("socket() failed", 1);
   }
	
	/* Construct local address structure */
   memset(&broadcastAddr, 0, sizeof(broadcastAddr));   
   broadcastAddr.sin_family = AF_INET;                 
   broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
   broadcastAddr.sin_port = htons(broadcastPort);     

     StartOfFrameStringLen = strlen(StartOfFrame); 
	 EndOfFrameStringLen = strlen(EndOfFrame); 
	 sendStringLen = 2*DEPTH_WIDTH*50+2; //2 bytes for every pixel for 50 lines + 2 bytes for sequence number
	 sendStringLenLast =  2*DEPTH_WIDTH*30+2;
	
   broadcastPermission = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,sizeof(broadcastPermission)) < 0){
       quit("socket() setsockopt error setting broadcast", 1);
   }

   char loopch=0;

	if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0) {
		quit("socket() setsockopt error setting multicast", 1);
	}

	/* start sending*/
	while(1)
	{
		/* send the frame, thread safe */
		pthread_mutex_lock(&mutex);
		if (is_data_ready) {
			
			/* Broadcast sendString in datagram to clients */

			if (sendto(sock, StartOfFrame, StartOfFrameStringLen, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != StartOfFrameStringLen){
				quit("socket() sendto error", 1);
			}

			i = 0;
			for(j = 0; j<450; j+=50)
			{	// 9 packages of 50 lines
				
				/* 	send:
				// 	2 bytes sequence number
				//	2 * DEPTH_WIDTH bytes (each pixel has 2 bytes)
				//	do this for every line of DEPTH_HEIGHT
				*/
				
				memcpy(&temp_data[2], &depth_data[j*DEPTH_WIDTH*2], DEPTH_WIDTH*2); // one line to a temp memory
				memcpy(&temp_data[0], &i, 2); // add sequence number
				
				if (sendto(sock, &temp_data[0], sendStringLen, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != sendStringLen){
					quit("socket() sendto error", 1);
				}
				
				i+=1; //sequence number
			}
			
			// last 30 lines
			
				memcpy(&temp_data[2], &depth_data[450*DEPTH_WIDTH*2], DEPTH_WIDTH*2); // one line to a temp memory
				memcpy(&temp_data[0], &i, 2); // add sequence number
				
				if (sendto(sock, &temp_data[0], sendStringLenLast, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != sendStringLenLast){
					quit("socket() sendto error", 1);
				}
			
			if (sendto(sock, EndOfFrame, EndOfFrameStringLen, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != EndOfFrameStringLen){
				quit("socket() sendto error", 1);
			}
			
			is_data_ready = 0;
		}
		pthread_mutex_unlock(&mutex);

		/* if something went wrong, restart the connection */
		//... ? necessary for broadcast?

		/* have we terminated yet? */
		pthread_testcancel();

		/* no, take a rest for a while */
		//usleep(1000);
		//usleep(30000); // 30ms
		//usleep(50000); // 50ms
		//usleep(150000); // 150ms
		usleep(usleep_time_udp);
		//usleep(500000);
	}
	
	free(temp_data);
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
	if (img1) cvReleaseImage(&img1);
	if (depth_data) free(depth_data);

	pthread_mutex_destroy(&mutex);

	exit(retval);
}

