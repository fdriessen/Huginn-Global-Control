/*
 * line_detection.h
 *
 *  Created on: 31 mei 2013
 *      Author: Floris
 */

#ifndef LINE_DETECTION_H_
#define LINE_DETECTION_H_

typedef struct {
	Point sum;
	Point sum_abs;
	Point av_sum;
	Point av_abs;
	float angle;
	float av_angle;
	float prev_angle;
	int n;
} LineStruct;

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)

// prototypes
float calcAngle(Point delta);
long long getTimeMillis();
double calculeFrameRate();
void quit(char* msg, int retval);

#endif /* LINE_DETECTION_H_ */
