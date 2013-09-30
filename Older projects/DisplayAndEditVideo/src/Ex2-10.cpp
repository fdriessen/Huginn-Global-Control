// call: qc@ubuntu:~/Documents/opencv-sources/Ex2-10/Debug$ ./Ex2-10 ../../Video/Lifted.avi ../../Video/Lifted_pgm.avi
// argv[1]: input video file
// argv[2]: name of new output file

#include <cv.h>
#include <highgui.h>

int main( int argc, char* argv[] ) {
	CvCapture* capture = 0;
	capture = cvCreateFileCapture( argv[1] );

	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read
	double fps = cvGetCaptureProperty (capture,CV_CAP_PROP_FPS);

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));
	CvVideoWriter *writer = cvCreateVideoWriter(argv[2],CV_FOURCC('M','J','P','G'),fps,size,3);
	//IplImage* logpolar_frame = cvCreateImage(size,IPL_DEPTH_8U,3); //logpolar image
	IplImage* pgm_frame = cvCreateImage(size,IPL_DEPTH_8U,1); //PGM image

	while( (bgr_frame=cvQueryFrame(capture)) != NULL ) {
		//create logpolar image
		//cvLogPolar( bgr_frame, logpolar_frame,cvPoint2D32f(bgr_frame->width/2,bgr_frame->height/2),40,	CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS );
		//cvWriteFrame( writer, logpolar_frame );

		//create grayscale image
		cvConvertImage(bgr_frame, pgm_frame,0); //convert one type to the other
		//cvWriteFrame( writer, pgm_frame ); //store to writer for output, does not work, why?
		cvWriteFrame( writer, bgr_frame ); //store original to output, does work

		// show image
		if((!pgm_frame)||(!bgr_frame)/*||(!logpolar_frame)*/ ) break;
		cvShowImage("Processing video", pgm_frame );
		cvShowImage("Original video", bgr_frame );
		//cvShowImage("Logpolar video", logpolar_frame);
		char c = cvWaitKey(33);
		if( c == 27 ) break;
	}

	cvReleaseVideoWriter( &writer );
	//cvReleaseImage( &logpolar_frame );
	cvReleaseImage( &bgr_frame );
	cvReleaseImage( &pgm_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Processing video" );
	cvDestroyWindow( "Original video" );
	//cvDestroyWindow( "Logpolar video" );

	return(0);
}
