/*
 * multiwii_driver.c
 *
 *  Created on: 31 mei 2013
 *      Author: Floris
 */

#include "multiwii_driver.h"

void SendToMultiWii(rc_values *values)
{
	int data_length = 16;
	char c = 0;
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'S';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = MSP_SET_RAW_RC;
	send_buf[4] = data_length; // data length

	BufSet16(&send_buf[5], values.roll);
	BufSet16(&send_buf[7], values.pitch);
	BufSet16(&send_buf[9], values.yaw);
	BufSet16(&send_buf[11], 1000); // throttle should be controlled by multiwii
	BufSet16(&send_buf[13], values.height + 1000);
	BufSet16(&send_buf[15], values.aux2);
	BufSet16(&send_buf[17], values.aux3);
	BufSet16(&send_buf[19], 1000); // aux4 is reserved for arming the QC

	for(int i = 5; i < 5 + send_buf[4]; i++)
	{
		c ^= send_buf[i];
	}

	send_buf[21] = c;
}

inline void BufSet16(unsigned char *buf, int in)
{
	// LSB in first byte
	buf[0] = in & 0xFF;
	// MSB in second byte
	buf[1] = (in >> 8) & 0xFF;
}

