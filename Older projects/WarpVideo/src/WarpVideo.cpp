
#include <cv.h>
 #include "opencv2/highgui/highgui.hpp"
 #include "opencv2/imgproc/imgproc.hpp"
 #include <iostream>
 #include <stdio.h>

IplImage *rotateImage(const IplImage *src, int angleDegrees)
{
    IplImage *imageRotated = cvCloneImage(src);

    if(angleDegrees!=0){
        CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);

        // Compute rotation matrix
        CvPoint2D32f center = cvPoint2D32f( cvGetSize(imageRotated).width/2, cvGetSize(imageRotated).height/2 );
        cv2DRotationMatrix( center, angleDegrees, 1, rot_mat );

        // Do the transformation
        cvWarpAffine( src, imageRotated, rot_mat);
    }

    return imageRotated;
}

int main( int argc, char* argv[] ) {
	char source[50];
	char destination[50];

	//argv[1]: filename
	//argv[2]: source path, for example: ../../Video/
	//argv[3]: destination path
	//argv[4]: "fast" fast computation
	strcpy(source, argv[2]);
	strcat(source, argv[1]);
	strcpy(destination, argv[3]);
	strcat(destination, argv[1]);

	CvCapture* capture = 0;
	capture = cvCreateFileCapture( source );

	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read
	double fps = cvGetCaptureProperty (capture,CV_CAP_PROP_FPS);

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));
	CvVideoWriter *writer = cvCreateVideoWriter(destination,CV_FOURCC('M','J','P','G'),fps,size,3);
	IplImage* out_frame = cvCreateImage(size,IPL_DEPTH_8U,3);

	while( (bgr_frame=cvQueryFrame(capture)) != NULL ) {

		//out_frame = bgr_frame;

		int angle=180;

		//rotate the image
		 out_frame=rotateImage(bgr_frame,angle);

		cvWriteFrame( writer, out_frame ); //store to output file

		// show image
		if(!bgr_frame) break;
		if(!argv[4])
		{
			cvShowImage("Processing video", out_frame );
			cvShowImage("Original video", bgr_frame );
			char c = cvWaitKey(33);
			if( c == 27 ) break;
		}
	}

	cvReleaseVideoWriter( &writer );
	cvReleaseImage( &bgr_frame );
	cvReleaseImage( &out_frame );
	cvReleaseCapture( &capture );
	if(!argv[4])
	{
		cvDestroyWindow( "Processing video" );
		cvDestroyWindow( "Original video" );
	}
	return(0);
}
