/*
 * multiwii_driver.c
 *
 *  Created on: 31 mei 2013
 *      Author: Floris
 */

#include <stdio.h>
#include <string.h>
#include "multiwii_driver.h"
#include "serial.h"

//#define DEBUG_MULTIWII

int GetMWInfo(int fd)
{
	int data_length = 0;
	char c = 0;
	// send command
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'$';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = data_length; // data length
	send_buf[4] = MSP_IDENT;
	for(int i = 3; i < 5 + send_buf[3]; i++)
	{
		c ^= send_buf[i];
	}
	send_buf[5] = c;
	
	write(fd, send_buf, data_length + 6);
	//read command
	unsigned char rec_buf [100];
	int n = read(fd, rec_buf, 6 + 7);  // read up to 100 characters if ready to read
	
#ifdef DEBUG_MULTIWII
	printf("GetMWInfo: %d bytes read from serial", n);
#endif

	// check checksum
	c = 0;
	for(int i = 3; i < 5 + 4*2; i++)
	{
		c ^= rec_buf[i];
	}
	if(c != rec_buf[5 + 4*2])
		return -1;
	
	return 0;
}

int GetMotorMW(int fd, motor_data *md)
{
	int data_length = 0;
	char c = 0;
	// send command
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'$';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = data_length; // data length
	send_buf[4] = MSP_MOTOR;
	for(int i = 3; i < 5 + send_buf[3]; i++)
	{
		c ^= send_buf[i];
	}
	send_buf[5] = c;
	
	write(fd, send_buf, data_length + 6);
	//read command
	unsigned char rec_buf [100];
	int n = read(fd, rec_buf, 6 + 8*2);  // read up to 100 characters if ready to read

#ifdef DEBUG_MULTIWII
	printf("GetMotorMW: %d bytes read from serial\n", n);
#endif
	
	// check checksum
	c = 0;
	for(int i = 3; i < 5 + 8*2; i++)
	{
		c ^= rec_buf[i];
	}
	if(c != rec_buf[5 + 8*2])
		return -1;
	
	md->fl = BufToB16(&rec_buf[5]);
	md->fr = BufToB16(&rec_buf[7]);
	md->bl = BufToB16(&rec_buf[9]);
	md->br = BufToB16(&rec_buf[11]);
	
	return 0;
}

int GetAttitudeMW(int fd, attitude_data *ad)
{
	int data_length = 0;
	char c = 0;
	// send command
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'$';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = data_length; // data length
	send_buf[4] = MSP_ATTITUDE;
	for(int i = 3; i < 5 + send_buf[3]; i++)
	{
		c ^= send_buf[i];
	}
	send_buf[5] = c;
	
#ifdef DEBUG_MULTIWII
	printf("GetAttitudeMW: sending\n");
#endif
	write(fd, send_buf, data_length + 6);
#ifdef DEBUG_MULTIWII
	printf("GetAttitudeMW: written %x %x %x %x %x %x\n", send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4], send_buf[5]);
#endif

	//read command
	unsigned char rec_buf [100];
	int n = read(fd, rec_buf, 6 + 4*2);  // read up to 100 characters if ready to read
	
#ifdef DEBUG_MULTIWII
	printf("GetAttitudeMW: %d bytes read from serial\n", n);
#endif
	
	// check checksum
	c = 0;
	for(int i = 3; i < 5 + 4*2; i++)
	{
		c ^= rec_buf[i];
	}
	if(c != rec_buf[5 + 4*2])
		return -1;
	
	ad->roll = BufToB16(&rec_buf[5]);
	ad->pitch = BufToB16(&rec_buf[7]);
	ad->yaw = BufToB16(&rec_buf[9]);
	ad->headfreemodehold = BufToB16(&rec_buf[11]);
	
	return 0;
}


int GetSensorMW(int fd, sensor_data *sd)
{
	int data_length = 0;
	char c = 0;
	// send command
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'$';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = data_length; // data length
	send_buf[4] = MSP_RAW_IMU;
	for(int i = 3; i < 5 + send_buf[3]; i++)
	{
		c ^= send_buf[i];
	}
	send_buf[5] = c;
	
#ifdef DEBUG_MULTIWII
	printf("GetSensorMW: sending\n");
#endif
	write(fd, send_buf, data_length + 6);
#ifdef DEBUG_MULTIWII
	printf("GetSensorMW: writen %x %x %x %x %x %x\n", send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4], send_buf[5]);
#endif

	//read command
	unsigned char rec_buf [100];
	int n = read(fd, rec_buf, 6 + 9*2);  // read up to 100 characters if ready to read
	
#ifdef DEBUG_MULTIWII
	printf("GetSensorMW: %d bytes read from serial\n", n);
#endif
	
	// check checksum
	c = 0;
	for(int i = 3; i < 5 + 9*2; i++)
	{
		c ^= rec_buf[i];
	}
	if(c != rec_buf[5 + 9*2])
		return -1;
	
	sd->acc_x = BufToB16(&rec_buf[5]);
	sd->acc_y = BufToB16(&rec_buf[7]);
	sd->acc_z = BufToB16(&rec_buf[9]);
	sd->gyro_roll = BufToB16(&rec_buf[11]);
	sd->gyro_pitch = BufToB16(&rec_buf[13]);
	sd->gyro_yaw = BufToB16(&rec_buf[15]);
	sd->mag_roll = BufToB16(&rec_buf[17]);
	sd->mag_pitch = BufToB16(&rec_buf[19]);
	sd->mag_yaw = BufToB16(&rec_buf[21]);
	
	return 0;
}

int SetRcMW(int fd, rc_values *values) 
{
	int data_length = 3*2;
	char c = 0;
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'$';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = data_length; // data length
	send_buf[4] = MSP_BEAGLEBOARD;

	B16ToBuf(&send_buf[5], values->roll);
	B16ToBuf(&send_buf[7], values->pitch);
	B16ToBuf(&send_buf[9], values->yaw);

	for(int i = 3; i < 5 + send_buf[3]; i++)
		c ^= send_buf[i];

	send_buf[data_length + 5] = c;
	
	int bytes_read = write(fd, send_buf, data_length + 6);
	
	return bytes_read;
}

inline void B16ToBuf(unsigned char *buf, int in)
{
	// LSB in first byte
	buf[0] = (unsigned char)(in & 0xFF);
	// MSB in second byte
	buf[1] = (unsigned char)((in >> 8) & 0xFF);
}

inline short BufToB16(unsigned char *buf)
{
	return (short)(buf[0]+((short)buf[1]*(1<<8)));
}

