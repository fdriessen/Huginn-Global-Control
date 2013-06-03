/*
 * global_controller.c
 *
 *  Created on: May 30, 2013
 *      Author: Floris
 */

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "../config.h"
#include "global_controller.h"
#include "multiwii_driver.h"

void main(int argc, char **argv)
{
	while(true)
	{
		ld_information ld;
		rc_values rc;

		WaitForNewFrame(&ld);
		memset(&rc, sizeof(rc_values), 0);

		switch(ld.mode)
		{
			case MODE_LINE_FOLLOW:
				if(ld.data.follow_info.angle_sp - ld.data.follow_info.angle_cv < FOLLOW_ANGLE_THRESHOLD)
					rc.yaw = YAW_RIGHT;
				else if(ld.data.follow_info.angle_sp - ld.data.follow_info.angle_cv > FOLLOW_ANGLE_THRESHOLD)
					rc.yaw = YAW_LEFT;
				else
					rc.yaw = YAW_NEUTRAL;


				if(ld.data.follow_info.x_sp - ld.data.follow_info.x_cv < -FOLLOW_X_THRESHOLD)
					rc.roll = ROLL_LEFT;
				else if(ld.data.follow_info.x_sp - ld.data.follow_info.x_cv > FOLLOW_X_THRESHOLD)
					rc.yaw = ROLL_RIGHT;
				else
					rc.yaw = ROLL_NEUTRAL;

				rc.pitch = PITCH_FORWARD;

				break;

			case MODE_LINE_HOVER:
				if(ld.data.hover_info.x_sp - ld.data.hover_info.x_cv < -HOVER_X_THRESHOLD)
					rc.roll = ROLL_LEFT;
				else if(ld.data.hover_info.x_sp - ld.data.hover_info.x_cv > HOVER_X_THRESHOLD)
					rc.roll = ROLL_RIGHT;
				else
					rc.roll = ROLL_NEUTRAL;

				if(ld.data.hover_info.y_sp - ld.data.hover_info.y_cv < -HOVER_Y_THRESHOLD)
					rc.pitch = PITCH_BACKWARD;
				else if(ld.data.hover_info.y_sp - ld.data.hover_info.y_cv > HOVER_Y_THRESHOLD)
					rc.pitch = PITCH_FORWARD;
				else
					rc.pitch = PITCH_NEUTRAL;

				if(ld.data.hover_info.angle_sp - ld.data.hover_info.angle_cv < HOVER_ANGLE_THRESHOLD)
					rc.yaw = YAW_RIGHT;
				else if(ld.data.hover_info.angle_sp - ld.data.hover_info.angle_cv > HOVER_ANGLE_THRESHOLD)
					rc.yaw = YAW_LEFT;
				else
					rc.yaw = YAW_NEUTRAL;
			break;

			case MODE_LINE_ROTATE:
				if(ld.data.rotate_info.x_sp - ld.data.rotate_info.x_cv < -ROTATE_X_THRESHOLD)
					rc.roll = ROLL_LEFT;
				else if(ld.data.rotate_info.x_sp - ld.data.rotate_info.x_cv > ROTATE_X_THRESHOLD)
					rc.roll = ROLL_RIGHT;
				else
					rc.roll = ROLL_NEUTRAL;

				if(ld.data.rotate_info.y_sp - ld.data.rotate_info.y_cv < -ROTATE_Y_THRESHOLD)
					rc.pitch = PITCH_BACKWARD;
				else if(ld.data.rotate_info.y_sp - ld.data.rotate_info.y_cv > ROTATE_Y_THRESHOLD)
					rc.pitch = PITCH_FORWARD;
				else
					rc.pitch = PITCH_NEUTRAL;

				rc.yaw = YAW_LEFT;
			break;
		}

		SendToMultiWii(&rc);
	}
}

void WaitForNewFrame(ld_information *info)
{
	FILE *fp;

	if((fp = fopen(LINE_DETECT_FIFO, "r")) == NULL)
	{
		perror("fopen");
		exit(1);
	}

	fgets(info, sizeof(ld_information), fp);
#ifdef DEBUG
	printf("Line Detect: new frame");
#endif
	fclose(fp);

	return;
}




























