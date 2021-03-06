/*
 * line_detection.h
 *
 *  Created on: 31 mei 2013
 *      Author: Floris
 */

#ifndef LINE_DETECTION_H_
#define LINE_DETECTION_H_

#define ANGLE_OFFSET	0.20
#define X_OFFSET		24
#define Y_OFFSET		23

typedef struct {
	int n;
	Point vector;
	Point location;
	float angle;
	Point vector_lh;
} LineStruct;

enum LineDetectedPoint{
	ld_none = 0,
	ld_plus_close = 1 << 0,
	ld_minus_close = 1 << 1,
	ld_plus_far = 1 << 2,
	ld_minus_far = 1 << 3,
	ld_plus = 1 << 0 | 1 << 2,
	ld_minus = 1 << 1 | 1 << 3,
	ld_close = 1 << 0 | 1 << 1,
	ld_far = 1 << 2 | 1 << 3,
	ld_all = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3
};

enum LineElement {
	le_cross = 1,
	le_corner_left,
	le_corner_right,
	le_corner_left_taken,
	le_corner_right_taken,
	le_end,
	le_begin,
	le_none,
	le_unknown
};

#define CLS() 	printf("\033[H\033[2J"); \
                    fflush(stdout)

#define ANGLE_THRESHOLD		0.523598776f // rad = 30 deg

// prototypes
float calcAngle(Point delta);
long long getTimeMillis();
double calculeFrameRate();
void quit(char* msg, int retval);
bool belongs_to_line(float vector_angle, float line_angle, float margin);
LineDetectedPoint DetectLinePresence(Mat &bw, const float angle, const Point crossing, int dist_close, int dist_far, int threshold, int sq_size);
Point LinesCrossing(const LineStruct* line0, const LineStruct* line1);
int VectorsToLines(const vector<Vec4i>& vectors, LineStruct* line0, LineStruct* line1);


#endif /* LINE_DETECTION_H_ */
