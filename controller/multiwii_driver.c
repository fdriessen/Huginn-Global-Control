/*
 * multiwii_driver.c
 *
 *  Created on: 31 mei 2013
 *      Author: Floris
 */

#include "multiwii_driver.h"
#include "serial.h"

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
	int n = read(fd, rec_buf, 6 + 4*2);  // read up to 100 characters if ready to read
	
	// check checksum
	c = 0;
	for(int i = 3; i < 5 + 4*2; i++)
	{
		c ^= rec_buf[i];
	}
	if(c != rec_buf[5 + 4*2])
		return -1;
	
	md->fl = BufToB16(&rec_buf[5]);
	md->fr = BufToB16(&rec_buf[7]);
	md->bl = BufToB16(&rec_buf[9]);
	md->br = BufToB16(&rec_buf[11]);
	
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
	
	write (fd, send_buf, data_length + 6);
	//read command
	unsigned char rec_buf [100];
	int n = read(fd, rec_buf, 6 + 9*2);  // read up to 100 characters if ready to read
	
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
	int data_length = 16;
	char c = 0;
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'$';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = data_length; // data length
	send_buf[4] = MSP_SET_RAW_RC;

	B16ToBuf(&send_buf[5], values->roll);
	B16ToBuf(&send_buf[7], values->pitch);
	B16ToBuf(&send_buf[9], values->yaw);
	B16ToBuf(&send_buf[11], 1000); // throttle should be controlled by multiwii
	B16ToBuf(&send_buf[13], values->height + 1000);
	B16ToBuf(&send_buf[15], values->aux2);
	B16ToBuf(&send_buf[17], values->aux3);
	B16ToBuf(&send_buf[19], 1000); // aux4 is reserved for arming the QC

	for(int i = 3; i < 5 + send_buf[3]; i++)
	{
		c ^= send_buf[i];
	}

	send_buf[21] = c;
	
	return 0;
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
	return (short)(buf[0]+(buf[1]*(2<<8)));
}

