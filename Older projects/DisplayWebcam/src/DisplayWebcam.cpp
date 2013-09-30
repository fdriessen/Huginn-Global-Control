#include <cv.h>
#include <highgui.h>

int main(int argc, char** argv)
{
    CvCapture* capture;
    if( argc==1 ) {
    	//capture = cvCreateCameraCapture(0); //'-1' pick any camera
    	capture = cvCaptureFromCAM(CV_CAP_ANY );
    	printf("Input type = webcam\n\r");
   	}
   	else {
   		capture = cvCreateFileCapture( argv[1] );
   		printf("Input type = video\n\r");
   	}
   	assert( capture != NULL );

   	IplImage *cam_frame=cvQueryFrame(capture); //Init the video read

    while((cam_frame=cvQueryFrame(capture)) != NULL)
    {
		if(!cam_frame) break;
        cvShowImage("Webcam", cam_frame );
		char c = cvWaitKey(33);
		if( c == 27 ) break;
    }

	cvReleaseImage( &cam_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Webcam" );
	return(0);
}



