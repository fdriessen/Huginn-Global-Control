// call: qc@ubuntu:~/Documents/opencv-sources/Ex2-10/Debug$ ./Ex2-10 ../../Video/Lifted.avi ../../Video/Lifted_pgm.avi
// argv[1]: input video file path and name
// argv[2]: output file path and name

#include <cv.h>
 #include "opencv2/highgui/highgui.hpp"
 #include "opencv2/imgproc/imgproc.hpp"
 #include <iostream>
 #include <stdio.h>

 /// Global variables
 Mat src, dst;
 Mat map_x, map_y;
 char* remap_window = "Remap demo";
 int ind = 0;

 /// Function Headers
 void update_map( void );

 /**
 * @function main
 */
 int main( int argc, char** argv )
 {
   /// Load the image
   src = imread( argv[1], 1 );

  /// Create dst, map_x and map_y with the same size as src:
  dst.create( src.size(), src.type() );
  map_x.create( src.size(), CV_32FC1 );
  map_y.create( src.size(), CV_32FC1 );

  /// Create window
  namedWindow( remap_window, CV_WINDOW_AUTOSIZE );

  /// Loop
  while( true )
  {
    /// Each 1 sec. Press ESC to exit the program
    int c = waitKey( 1000 );

    if( (char)c == 27 )
      { break; }

    /// Update map_x & map_y. Then apply remap
    update_map();
    remap( src, dst, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0, 0) );

    /// Display results
    imshow( remap_window, dst );
  }
  return 0;
 }

 /**
 * @function update_map
 * @brief Fill the map_x and map_y matrices with 4 types of mappings
 */
 void update_map( void )
 {
   ind = ind%4;

   for( int j = 0; j < src.rows; j++ )
   { for( int i = 0; i < src.cols; i++ )
       {
         switch( ind )
         {
           case 0: //reduce size
             if( i > src.cols*0.25 && i < src.cols*0.75 && j > src.rows*0.25 && j < src.rows*0.75 )
               {
                 map_x.at<float>(j,i) = 2*( i - src.cols*0.25 ) + 0.5 ;
                 map_y.at<float>(j,i) = 2*( j - src.rows*0.25 ) + 0.5 ;
                }
             else
               { map_x.at<float>(j,i) = 0 ;
                 map_y.at<float>(j,i) = 0 ;
               }
                 break;
           case 1: // flip y
                 map_x.at<float>(j,i) = i ;
                 map_y.at<float>(j,i) = src.rows - j ;
                 break;
           case 2: : //flip x
                 map_x.at<float>(j,i) = src.cols - i ;
                 map_y.at<float>(j,i) = j ;
                 break;
           case 3: // flip x and y
                 map_x.at<float>(j,i) = src.cols - i ;
                 map_y.at<float>(j,i) = src.rows - j ;
                 break;
         } // end of switch
       }
    }
  ind++;
}

int main( int argc, char* argv[] ) {
	CvCapture* capture = 0;
	capture = cvCreateFileCapture( argv[1] );

	if(!capture) return -1;

	IplImage *bgr_frame=cvQueryFrame(capture); //Init the video read
	double fps = cvGetCaptureProperty (capture,CV_CAP_PROP_FPS);

	CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));
	CvVideoWriter *writer = cvCreateVideoWriter(argv[2],CV_FOURCC('M','J','P','G'),fps,size,3);
	IplImage* out_frame = cvCreateImage(size,IPL_DEPTH_8U,3);

	while( (bgr_frame=cvQueryFrame(capture)) != NULL ) {

		out_frame = bgr_frame;

		cvWriteFrame( writer, out_frame ); //store to output file

		// show image
		if(!bgr_frame) break;
		cvShowImage("Processing video", out_frame );
		cvShowImage("Original video", bgr_frame );
		char c = cvWaitKey(33);
		if( c == 27 ) break;
	}

	cvReleaseVideoWriter( &writer );
	cvReleaseImage( &bgr_frame );
	cvReleaseImage( &out_frame );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Processing video" );
	cvDestroyWindow( "Original video" );

	return(0);
}
