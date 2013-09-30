#include <cv.h>
#include <highgui.h>

int g_slider_position = 0;
CvCapture* g_capture = NULL;

void onTrackbarSlide(int pos)
{
	cvSetCaptureProperty(g_capture,CV_CAP_PROP_POS_FRAMES,pos);
}

int main ( int argc, char **argv )
{
	cvNamedWindow("Example3", CV_WINDOW_AUTOSIZE);
	g_capture = cvCreateFileCapture( argv[1] );

	int frames = (int) cvGetCaptureProperty(g_capture,CV_CAP_PROP_FRAME_COUNT);
	if( frames!= 0 ) {
		cvCreateTrackbar("Position","Example3",&g_slider_position,frames,onTrackbarSlide);
	}
	IplImage* frame;
	CvCapture* capture = cvCreateFileCapture( argv[1] );

	int frame_count = 0;

	// While loop capture & show video
	while(1)
	{
		frame = cvQueryFrame( capture );
		if( !frame ) break;
		cvShowImage( "Example3", frame );
		char c = cvWaitKey(33);
		if( c == 27 ) break;

		frame_count++;

		if(frame_count == frames-1)
		{
			cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,0);
			frame_count = 0;
		}
	}
	// Release memory and destroy window
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Example3" );
	return(0);
}



