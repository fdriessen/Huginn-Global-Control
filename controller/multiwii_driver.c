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

unsigned char send_buf[40];
unsigned char checksum;
unsigned char send_buf_counter;

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
	//if(c != rec_buf[5 + 8*2])
	//	return -1;
	
	md->fl = BufToB16(&rec_buf[5]);
	md->fr = BufToB16(&rec_buf[7]);
	md->bl = BufToB16(&rec_buf[9]);
	md->br = BufToB16(&rec_buf[11]);
	
	return 0;
}

int GetAttitudeMW(int fd, attitude_data *attd)
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
	unsigned char rec_buf[100]; 
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
	//if(c != rec_buf[5 + 4*2])
	//	return -1;
	
	attd->roll = BufToB16(&rec_buf[5]);
	attd->pitch = BufToB16(&rec_buf[7]);
	attd->yaw = BufToB16(&rec_buf[9]);
	attd->headfreemodehold = BufToB16(&rec_buf[11]);
	
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

int GetRcMW(int fd, rc_values *rc)
{
	send_buf_counter = 0;
	int data_length = 0;
	// send command
	Serialize8('$');
	Serialize8('M');
	Serialize8('<');
	checksum = 0;
	Serialize8(data_length);
	Serialize8(MSP_RC);
	Serialize8(checksum);

#ifdef DEBUG_MULTIWII
	printf("GetRcMW: sending\n");
#endif
	write(fd, send_buf, data_length + 6);
#ifdef DEBUG_MULTIWII
	printf("GetRcMW: writen %x %x %x %x %x %x\n", send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4], send_buf[5]);
#endif

	//read command
	unsigned char rec_buf [100];
	int n = read(fd, rec_buf, 6 + 8*2);  // read up to 100 characters if ready to read
	
#ifdef DEBUG_MULTIWII
	printf("GetRcMW: %d bytes read from serial\n", n);
#endif
	
	// check checksum
	unsigned char c = 0;
	for(int i = 3; i < 5 + 8*2; i++)
		c ^= rec_buf[i];
	if(c != rec_buf[5 + 8*2])
		return -1;
	
	rc->roll = Deserialize16(&rec_buf[5]);
	rc->pitch = Deserialize16(&rec_buf[7]);
	rc->yaw = Deserialize16(&rec_buf[9]);
	rc->throttle = Deserialize16(&rec_buf[11]);
	rc->aux1 = Deserialize16(&rec_buf[13]);
	rc->aux2 = Deserialize16(&rec_buf[15]);
	rc->aux3 = Deserialize16(&rec_buf[17]);
	rc->aux4 = Deserialize16(&rec_buf[19]);
	
	return 0;
}

int GetAltitudeMW(int fd, altitude_data *altd)
{
	send_buf_counter = 0;
	int data_length = 0;
	// send command
	Serialize8('$');
	Serialize8('M');
	Serialize8('<');
	checksum = 0;
	Serialize8(data_length);
	Serialize8(MSP_ALTITUDE);
	Serialize8(checksum);
	
#ifdef DEBUG_MULTIWII
	printf("GetAltitudeMW: sending\n");
#endif
	write(fd, send_buf, data_length + 6);
#ifdef DEBUG_MULTIWII
	printf("GetAltitudeMW: writen %x %x %x %x %x %x\n", send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4], send_buf[5]);
#endif

	//read command
	unsigned char rec_buf [100];
	unsigned char rec_data_length = 6;
	int n = read(fd, rec_buf, 6 + rec_data_length);  // read up to 100 characters if ready to read
	
#ifdef DEBUG_MULTIWII
	printf("GetAltitudeMW: %d bytes read from serial\n", n);
#endif
	
	// check checksum
	unsigned char c = 0;
	for(int i = 3; i < 5 + rec_data_length; i++)
		c ^= rec_buf[i];
	//if(c != rec_buf[5 + rec_data_length])
		//return -1;
	
	altd->altitude = Deserialize32(&rec_buf[5]);
	altd->vario = Deserialize16(&rec_buf[9]);
	
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

	for(int i = 3; i < 5 + data_length; i++)
		c ^= send_buf[i];

	send_buf[data_length + 5] = c;
	
	int bytes_write = write(fd, send_buf, data_length + 6);
	printf("SetRcMW: %2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x\n", send_buf[0],send_buf[1],send_buf[2],send_buf[3],send_buf[4],send_buf[5],send_buf[6],send_buf[7],send_buf[8],send_buf[9],send_buf[10],send_buf[11]);
	
	//read command
	unsigned char rec_buf[100];
	int n = read(fd, rec_buf, 6);  // read up to 100 characters if ready to read
	printf("SetRcMW: %d bytes read from serial\n", n);

	return bytes_write;
}

inline void Serialize8(unsigned char data)
{
	send_buf[send_buf_counter++] = data;
	checksum ^= data;
}

inline void Serialize16(short data)
{
	send_buf[send_buf_counter++] = (unsigned char)(data & 0xFF);
	checksum ^= (unsigned char)(data & 0xFF);
	send_buf[send_buf_counter++] = (unsigned char)((data >> 8) & 0xFF);
	checksum ^= (unsigned char)((data >> 8) & 0xFF);
}

inline short Deserialize16(unsigned char *buf)
{
	return (short)(buf[0]+((short)buf[1]*(1<<8)));
}

inline short Deserialize32(unsigned char *buf)
{
	return (short)(buf[0]+((short)buf[1]*(1<<8))+((short)buf[2]*(1<<16))+((short)buf[3]*(1<<24)));
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

