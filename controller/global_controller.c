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

	//open fifo (read)
	int fd_ldfifo = open(LINE_DETECT_FIFO, O_RDONLY/* | O_NONBLOCK*/);
	if (fd_ldfifo < 0)
	{
		printf("ERROR opening fifo %s, error nr: %d\n", LINE_DETECT_FIFO, fd_ldfifo);
		return -1;
	}

	ld_information ld;
	rc_values rc_set;
	rc_values rc_get;
	motor_data md;
	attitude_data attd;
	altitude_data altd;
	
		// wait for multiwii
#if SERIAL
	printf("MESSAGE: Waiting for MultiWii\n");
	GetAttitudeMW(fd_ser, &attd);
#endif	
	while(true)
	{
		usleep(40000);
		printf("\033c");
#if SERIAL
		// test
		// rc_set.roll = ROLL_NEUTRAL;
		// rc_set.pitch = PITCH_FORWARD;
		// rc_set.yaw = YAW_NEUTRAL;
		
		// SetRcMW(fd_ser, &rc_set);
		
		// get multiwii data
		if(GetRcMW(fd_ser, &rc_get) < 0)
			printf("ERROR: GetRcMW\n");
		else
			printf("\n");
		if(GetAttitudeMW(fd_ser, &attd) < 0)
			printf("ERROR: GetAttitudeMW\n");
		else
			printf("\n");
		if(GetAltitudeMW(fd_ser, &altd) < 0)
			printf("ERROR: GetAltitudeMW\n");
		else
			printf("\n");
		if(GetMotorMW(fd_ser, &md) < 0)
			printf("ERROR: GetMotorMW\n");
		else
			printf("\n");
#endif
		
		memset(&rc_set, sizeof(rc_set), 0);
#if SERIAL
		printf("%s: %4d, %4d, %4d, %4d\n", "RC get (roll, pitch, yaw, throttle)\n", rc_get.roll, rc_get.pitch, rc_get.yaw, rc_get.throttle);
		printf("%s: %4d, %4d\n", "Altitude (altitude, vario)\n", altd.altitude, altd.vario);
		printf("%s: %4d, %4d, %4d, %4d\n", "Attitude (roll, pitch, yaw)\n", attd.roll, attd.pitch, attd.yaw, attd.headfreemodehold);
		printf("%s: %4d, %4d, %4d, %4d\n", "Motor (fl, fr, bl, br)\n", md.fl, md.fr, md.bl, md.br);
#endif
		
		// continue;
		//ld.mode = 0;
		if(WaitForNewFrame(fd_ldfifo, &ld) > 0)
		{
			rc_set.roll = ROLL_NEUTRAL;
			rc_set.pitch = PITCH_NEUTRAL;
			rc_set.yaw = YAW_NEUTRAL;
			
			printf("ld.mode = %d\n", ld.mode);
			printf("ld.element = %d\n", ld.element);
			printf("ld.data.hover.angle_sp = %f\n", ld.data.hover.angle_sp);
			printf("ld.data.hover.angle_cv = %f\n", ld.data.hover.angle_cv);
			printf("ld.data.hover.x_sp = %d\n", ld.data.hover.x_sp);
			printf("ld.data.hover.x_cv = %d\n", ld.data.hover.x_cv);
			printf("ld.data.hover.y_sp = %d\n", ld.data.hover.y_sp);
			printf("ld.data.hover.y_cv = %d\n", ld.data.hover.y_cv);

			
			switch(ld.mode)
			{
				case MODE_LINE_FOLLOW:
					if(ld.data.follow.angle_cv - ld.data.follow.angle_sp < FOLLOW_ANGLE_THRESHOLD)
						rc_set.yaw = YAW_LEFT;
					else if(ld.data.follow.angle_cv - ld.data.follow.angle_sp > FOLLOW_ANGLE_THRESHOLD)
						rc_set.yaw = YAW_RIGHT;
					else
						rc_set.yaw = YAW_NEUTRAL;


					if(ld.data.follow.x_cv - ld.data.follow.x_sp < -FOLLOW_X_THRESHOLD)
						rc_set.roll = ROLL_LEFT;
					else if(ld.data.follow.x_cv - ld.data.follow.x_sp > FOLLOW_X_THRESHOLD)
						rc_set.roll = ROLL_RIGHT;
					else
						rc_set.roll = ROLL_NEUTRAL;

					rc_set.pitch = PITCH_FORWARD;

				break;

				case MODE_LINE_HOVER:
					if(ld.data.hover.x_cv - ld.data.hover.x_sp < -HOVER_X_THRESHOLD)
						rc_set.roll = ROLL_LEFT;
					else if(ld.data.hover.x_cv - ld.data.hover.x_sp > HOVER_X_THRESHOLD)
						rc_set.roll = ROLL_RIGHT;
					else
						rc_set.roll = ROLL_NEUTRAL;

					if(ld.data.hover.y_cv - ld.data.hover.y_sp < -HOVER_Y_THRESHOLD)
						rc_set.pitch = PITCH_FORWARD;
					else if(ld.data.hover.y_cv - ld.data.hover.y_sp > HOVER_Y_THRESHOLD)
						rc_set.pitch = PITCH_BACKWARD;
					else
						rc_set.pitch = PITCH_NEUTRAL;

					if(ld.data.hover.angle_cv - ld.data.hover.angle_sp < -HOVER_ANGLE_THRESHOLD)
						rc_set.yaw = YAW_LEFT;
					else if(ld.data.hover.angle_cv - ld.data.hover.angle_sp > HOVER_ANGLE_THRESHOLD)
						rc_set.yaw = YAW_RIGHT;
					else
						rc_set.yaw = YAW_NEUTRAL;
				break;

				case MODE_LINE_ROTATE:
					if(ld.data.rotate.x_cv - ld.data.rotate.x_sp < -ROTATE_X_THRESHOLD)
						rc_set.roll = ROLL_LEFT;
					else if(ld.data.rotate.x_cv - ld.data.rotate.x_sp > ROTATE_X_THRESHOLD)
						rc_set.roll = ROLL_RIGHT;
					else
						rc_set.roll = ROLL_NEUTRAL;

					if(ld.data.rotate.y_cv - ld.data.rotate.y_sp < -ROTATE_Y_THRESHOLD)
						rc_set.pitch = PITCH_FORWARD;
					else if(ld.data.rotate.y_cv - ld.data.rotate.y_sp > ROTATE_Y_THRESHOLD)
						rc_set.pitch = PITCH_BACKWARD;
					else
						rc_set.pitch = PITCH_NEUTRAL;

					rc_set.yaw = YAW_LEFT;
				break;
			}

			printf("%s: %4d, %4d, %4d\n", "RC set (roll, pitch, yaw)\n", rc_set.roll, rc_set.pitch, rc_set.yaw);
	#if SERIAL
			if(SetRcMW(fd_ser, &rc_set) < 0)
			{
				printf("ERROR: SetRcMW");
			}
	#endif
		}
		else
			printf("no new frame detected");
	}
	
#if SERIAL
	close(fd_ser);
#endif
	close(fd_ldfifo);
	return 0;
}

int WaitForNewFrame(int fdfifo, ld_information *info)
{
	// open line detection fifo in read mode
	int bytes_read = read(fdfifo, info, sizeof(ld_information));
	printf("WaitForNewFrame: bytes read = %d\n", bytes_read);
	return bytes_read;
}




























