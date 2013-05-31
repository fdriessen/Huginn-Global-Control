/*
 * global_controller.h
 *
 *  Created on: 30 mei 2013
 *      Author: Floris
 */

#ifndef GLOBAL_CONTROLLER_H_
#define GLOBAL_CONTROLLER_H_

#define FOLLOW_ANGLE_THRESHOLD 0.0174532925f // 1 deg to rad
#define FOLLOW_X_THRESHOLD	10 //pixels

#define ROTATE_X_THRESHOLD	10 //pixels
#define ROTATE_Y_THRESHOLD	10 //pixels

#define HOVER_X_THRESHOLD	10 //pixels
#define HOVER_Y_THRESHOLD	10 //pixels
#define HOVER_ANGLE_THRESHOLD 0.0174532925f // 1 deg to rad

#define YAW_NEUTRAL			1500
#define YAW_RIGHT			YAW_NEUTRAL + 10
#define YAW_LEFT			YAW_NEUTRAL - 10

#define ROLL_NEUTRAL		1500
#define ROLL_RIGHT			ROLL_NEUTRAL + 10
#define ROLL_LEFT			ROLL_NEUTRAL - 10

#define PITCH_NEUTRAL		1500
#define PITCH_FORWARD		PITCH_NEUTRAL + 10
#define PITCH_BACKWARD		PITCH_NEUTRAL - 10

void WaitForNewFrame(ld_information *info);

#endif /* GLOBAL_CONTROLLER_H_ */
