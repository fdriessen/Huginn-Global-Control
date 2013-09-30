//argv[1] file path and name to store image
//argv[2] desired amount of frames per second

#include <cv.h>
#include <highgui.h>

int main(int argc, char** argv)
{
    CvCapture* capture;
   	//capture = cvCreateCameraCapture(0); //'-1' pick any camera
    capture = cvCaptureFromCAM(CV_CAP_ANY );
    printf("Input type = webcam\n\r");
   	assert( capture != NULL );

   	double fps = (double)atoi(argv[2]);

   	cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,fps);

   	IplImage *cam_frame=cvQueryFrame(capture); //Init the video read

   	// for video writing
   	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));
   	CvVideoWriter *writer = cvCreateVideoWriter(argv[1],CV_FOURCC('M','J','P','G'),fps,size,3);

    while((cam_frame=cvQueryFrame(capture)) != NULL)
    {
		if(!cam_frame) break;
        cvShowImage("Webcam", cam_frame );
        cvWriteFrame( writer, cam_frame );
		char c = cvWaitKey(33);
		if( c == 27 ) break;
    }

    cvReleaseVideoWriter( &writer );
	cvReleaseImage( &cam_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Webcam" );
	return(0);
}


