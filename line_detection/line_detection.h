/*
 * line_detection.h
 *
 *  Created on: 31 mei 2013
 *      Author: Floris
 */

#ifndef LINE_DETECTION_H_
#define LINE_DETECTION_H_

typedef struct {
	cv::Point sum;
	cv::Point sum_abs;
	cv::Point av_sum;
	cv::Point av_abs;
	float angle;
	float av_angle;
	float prev_angle;
	int n;
} LineStruct;

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)

#endif /* LINE_DETECTION_H_ */
