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

int ui()
{
	
	return 0;
}

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

	while(true)
	{
		ld_information ld;
		rc_values rc;
		motor_data md;
		sensor_data sd;
		
#if SERIAL
		// get multiwii data
		if(GetSensorMW(fd_ser, &sd) < 0)
			printf("ERROR: GetSensorMW");
		if(GetMotorMW(fd_ser, &md) < 0)
			printf("ERROR: GetMotorMW");
#endif

		//WaitForNewFrame(&ld);
		memset(&rc, sizeof(rc), 0);
		printf("info: mode = %d\n", ld.mode);
		printf("%30s: %4d, %4d, %4d", "RC (roll, pitch, yaw)\n", rc.roll, rc.pitch, rc.yaw);
#if SERIAL
		printf("%30s: %4d, %4d, %4d, %4d", "Motor (fl, fr, bl, br)\n", md.fl, md.fr, md.bl, md.br);
		printf("%30s: %4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d", "Sensor (acc(xyz), gyro(rpy), mag(rpy))\n", sd.acc_x, sd.acc_y, sd.acc_z, sd.gyro_roll, sd.gyro_pitch, sd.gyro_yaw, sd.mag_roll, sd.mag_pitch, sd.mag_yaw);
#endif

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
			printf("ERROR: SetRcMW");
#endif
		break;
	}
	
	return 0;
}

void WaitForNewFrame(ld_information *info)
{
	int fdfifo;
	
	/* open, read, and display the message from the FIFO */
	if((fdfifo = open(LINE_DETECT_FIFO, O_RDONLY /*| O_NONBLOCK*/)) < 0)
	{
		perror("fopen ldfifo");
		exit(1);
	}
	
	info->mode = 0;
	read(fdfifo, info, sizeof(ld_information));
	close(fdfifo);

	return;
}




























