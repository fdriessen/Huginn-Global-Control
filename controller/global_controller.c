/*
 * global_controller.c
 *
 *  Created on: May 30, 2013
 *      Author: Floris
 */

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "global_controller.h"
#include "multiwii_driver.h"

void main(int argc, char **argv)
{
	while(true)
	{
		int roll;
		int yaw;
		int pitch;
		int height;



		WaitForFrame(from_line_detect);

		// check prev. ack;

		int mode = GetMode(from_line_detect);

		switch(mode)
		{
			case MODE_LINE_FOLLOW:
				int angle_sp;
				int angle_cv;

				int x_sp;
				int x_cv;

				GetFollowSetpoints(&angle_sp, &x_sp, from_line_detect);
				GetFollowCurrentValue(&angle_cv, &x_cv, from_line_detect);

				if(angle_sp - angle_cv < FOLLOW_ANGLE_THRESHOLD)
					yaw = YAW_RIGHT;
				else if(angle_sp - angle_cv > FOLLOW_ANGLE_THRESHOLD)
					yaw = YAW_LEFT;
				else
					yaw = YAW_NEUTRAL;


				if(x_sp - x_cv < -FOLLOW_X_THRESHOLD)
					roll = ROLL_LEFT;
				else if(x_sp - x_cv > FOLLOW_X_THRESHOLD)
					yaw = ROLL_RIGHT;
				else
					yaw = ROLL_NEUTRAL;

				pitch = PITCH_FORWARD;

				break;

			case MODE_LINE_HOVER:
				int x_sp;
				int x_cv;

				int y_cv;
				int y_sp;

				int angle_sp;
				int angle_cv;

				GetHoverSetpoint(&x_sp, &y_sp, &angle_sp, from_line_detect);
				GetHoverCurrentValue(&x_cv, &y_cv, &angle_cv, from_line_detect);

				if(x_sp - x_cv < -HOVER_X_THRESHOLD)
					roll = ROLL_LEFT;
				else if(x_sp - x_cv > HOVER_X_THRESHOLD)
					roll = ROLL_RIGHT;
				else
					roll = ROLL_NEUTRAL;

				if(y_sp - y_cv < -HOVER_Y_THRESHOLD)
					pitch = PITCH_BACKWARD;
				else if(y_sp - y_cv > HOVER_Y_THRESHOLD)
					pitch = PITCH_FORWARD;
				else
					pitch = PITCH_NEUTRAL;

				if(angle_sp - angle_cv < HOVER_ANGLE_THRESHOLD)
					yaw = YAW_RIGHT;
				else if(angle_sp - angle_cv > HOVER_ANGLE_THRESHOLD)
					yaw = YAW_LEFT;
				else
					yaw = YAW_NEUTRAL;
			break;

			case MODE_LINE_ROTATE:
				int x_sp;
				int x_cv;

				int y_cv;
				int y_sp;

				GetRotateSetpoint(&x_sp, &y_sp, from_line_detect);
				GetRotateCurrentValue(&x_cv, &y_cv, from_line_detect);

				if(x_sp - x_cv < -ROTATE_X_THRESHOLD)
					roll = ROLL_LEFT;
				else if(x_sp - x_cv > ROTATE_X_THRESHOLD)
					roll = ROLL_RIGHT;
				else
					roll = ROLL_NEUTRAL;

				if(y_sp - y_cv < -ROTATE_Y_THRESHOLD)
					pitch = PITCH_BACKWARD;
				else if(y_sp - y_cv > ROTATE_Y_THRESHOLD)
					pitch = PITCH_FORWARD;
				else
					pitch = PITCH_NEUTRAL;

				yaw = YAW_LEFT;
			break;
		}

		SendToMultiWii(roll, pitch, yaw, height, 0, 0);
	}
}

void WaitForNewFrame(ld_information *info)
{
	FILE *fp;

	if((fp = fopen(LINE_DETECT_FIFO, "w")) == NULL)
	{
		perror("fopen");
		exit(1);
	}
}




























