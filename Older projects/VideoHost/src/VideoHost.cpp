//UDPServer
// http://simplestcodings.blogspot.nl/2010/08/udp-server-client-implementation-in-c.html
// ifconfig to show ip
// netstat -tulnap to show used ports and pid
// kill -KILL <pid>
#include <cv.h>
#include <highgui.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 512
#define PORT 9930

void err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char* argv[] )
{
	CvCapture* capture = 0;
	capture = cvCreateFileCapture( argv[1] );
	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read
	double fps = cvGetCaptureProperty (capture,CV_CAP_PROP_FPS);

    struct sockaddr_in my_addr, cli_addr;
    int sockfd, i;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];
    //IplImage *buf_frame = cvQueryFrame(capture);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) err("socket");
    else printf("Server : Socket() successful\n");

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1) err("bind");
    else printf("Server : bind() successful\n");

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));
	CvVideoWriter *writer = cvCreateVideoWriter(argv[2],CV_FOURCC('M','J','P','G'),fps,size,3);

    while(1)
    {
        if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1) err("recvfrom()");
        //printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);

        //buf_frame=cvQueryFrame(capture));
        if(!bgr_frame) break;

        cvWriteFrame( writer, bgr_frame ); //store to output
        cvShowImage("Received video", bgr_frame );

        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    close(sockfd);
    cvReleaseVideoWriter( &writer );
    cvReleaseImage( &bgr_frame );
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Received video" );
    return 0;
}
