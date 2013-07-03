/*
 * global_controller.c
 *
 *  Created on: May 30, 2013
 *      Author: Floris
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../common.h"
#include "../config.h"
#include "global_controller.h"
#include "multiwii_driver.h"
#include "serial.h"

#define SERIAL 1

const char *serial_port = "/dev/ttyUSB0";

int main(int argc, char **argv)
{
#if SERIAL
	int fd_ser = open(serial_port, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd_ser < 0)
	{
		printf("ERROR opening %s, error nr: %d\n", serial_port, fd_ser);
		printf("Probably forgot sudo\n");
		return -1;
	}

	set_interface_attribs(fd_ser, B115200, 0);	// set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd_ser, 1);	// set blocking
#endif

	// open line detection fifo in read mode
	int fd_ldfifo = open(LINE_DETECT_FIFO, O_RDONLY | O_NONBLOCK);
	if (fd_ldfifo < 0)
	{
		printf("ERROR opening fifo %s, error nr: %d\n", LINE_DETECT_FIFO, fd_ldfifo);
		return -1;
	}

	ld_information ld;
	rc_values rc;
	motor_data md;
	attitude_data ad;
		
	while(true)
	{
		usleep(100000);
		printf("\033c");
#if SERIAL
		// wait for multiwii
		printf("MESSAGE: Waiting for MultiWii\n");
		
		// get multiwii data
		if(GetAttitudeMW(fd_ser, &ad) < 0)
			printf("ERROR: GetAttitudeMW\n");
		if(GetMotorMW(fd_ser, &md) < 0)
			printf("ERROR: GetMotorMW\n");
#endif
		
		memset(&rc, sizeof(rc), 0);
		printf("info: mode = %d\n", ld.mode);
		printf("%s: %4d, %4d, %4d\n", "RC (roll, pitch, yaw)\n", rc.roll, rc.pitch, rc.yaw);
#if SERIAL
		printf("%s: %4d, %4d, %4d, %4d\n", "Attitude (roll, pitch, yaw)\n", ad.roll, ad.pitch, ad.yaw, ad.headfreemodehold);
		printf("%s: %4d, %4d, %4d, %4d\n", "Motor (fl, fr, bl, br)\n", md.fl, md.fr, md.bl, md.br);
#endif

		if(WaitForNewFrame(fd_ldfifo, &ld) > 0)
		{
			printf("info: mode = %d\n", ld.mode);
			switch(ld.mode)
			{
				case MODE_LINE_FOLLOW:
					if(ld.data.follow.angle_sp - ld.data.follow.angle_cv < FOLLOW_ANGLE_THRESHOLD)
						rc.yaw = YAW_RIGHT;
					else if(ld.data.follow.angle_sp - ld.data.follow.angle_cv > FOLLOW_ANGLE_THRESHOLD)
						rc.yaw = YAW_LEFT;
					else
						rc.yaw = YAW_NEUTRAL;


					if(ld.data.follow.x_sp - ld.data.follow.x_cv < -FOLLOW_X_THRESHOLD)
						rc.roll = ROLL_LEFT;
					else if(ld.data.follow.x_sp - ld.data.follow.x_cv > FOLLOW_X_THRESHOLD)
						rc.yaw = ROLL_RIGHT;
					else
						rc.yaw = ROLL_NEUTRAL;

					rc.pitch = PITCH_FORWARD;

				break;

				case MODE_LINE_HOVER:
					if(ld.data.hover.x_sp - ld.data.hover.x_cv < -HOVER_X_THRESHOLD)
						rc.roll = ROLL_LEFT;
					else if(ld.data.hover.x_sp - ld.data.hover.x_cv > HOVER_X_THRESHOLD)
						rc.roll = ROLL_RIGHT;
					else
						rc.roll = ROLL_NEUTRAL;

					if(ld.data.hover.y_sp - ld.data.hover.y_cv < -HOVER_Y_THRESHOLD)
						rc.pitch = PITCH_BACKWARD;
					else if(ld.data.hover.y_sp - ld.data.hover.y_cv > HOVER_Y_THRESHOLD)
						rc.pitch = PITCH_FORWARD;
					else
						rc.pitch = PITCH_NEUTRAL;

					if(ld.data.hover.angle_sp - ld.data.hover.angle_cv < HOVER_ANGLE_THRESHOLD)
						rc.yaw = YAW_RIGHT;
					else if(ld.data.hover.angle_sp - ld.data.hover.angle_cv > HOVER_ANGLE_THRESHOLD)
						rc.yaw = YAW_LEFT;
					else
						rc.yaw = YAW_NEUTRAL;
				break;

				case MODE_LINE_ROTATE:
					if(ld.data.rotate.x_sp - ld.data.rotate.x_cv < -ROTATE_X_THRESHOLD)
						rc.roll = ROLL_LEFT;
					else if(ld.data.rotate.x_sp - ld.data.rotate.x_cv > ROTATE_X_THRESHOLD)
						rc.roll = ROLL_RIGHT;
					else
						rc.roll = ROLL_NEUTRAL;

					if(ld.data.rotate.y_sp - ld.data.rotate.y_cv < -ROTATE_Y_THRESHOLD)
						rc.pitch = PITCH_BACKWARD;
					else if(ld.data.rotate.y_sp - ld.data.rotate.y_cv > ROTATE_Y_THRESHOLD)
						rc.pitch = PITCH_FORWARD;
					else
						rc.pitch = PITCH_NEUTRAL;

					rc.yaw = YAW_LEFT;
				break;
			}

	#if SERIAL
			if(SetRcMW(fd_ser, &rc) < 0)
			{
				printf("ERROR: SetRcMW");
				break;
			}
	#endif
		}
	}
	
	close(fd_ser);
	close(fd_ldfifo);
	return 0;
}

int WaitForNewFrame(int fdfifo, ld_information *info)
{
	//info->mode = 0;
	int bytes_read = read(fdfifo, info, sizeof(ld_information));

	return bytes_read;
}




























