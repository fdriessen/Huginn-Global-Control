/*
 * global_controller.c
 *
 *  Created on: May 30, 2013
 *      Author: Floris
 */


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

				if(angle_sp - angle_cv < ANGLE_THRESHOLD)
					yaw = YAW_RIGHT;
				else if(angle_sp - angle_cv > ANGLE_THRESHOLD)
					yaw = YAW_LEFT;
				else
					yaw = YAW_NEUTRAL;


				if(x_sp - x_cv < -X_THRESHOLD)
					roll = ROLL_LEFT;
				else if(x_sp - x_cv > X_THRESHOLD)
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

				if(angle_sp - angle_cv < ANGLE_THRESHOLD)
					yaw = YAW_RIGHT;
				else if(angle_sp - angle_cv > ANGLE_THRESHOLD)
					yaw = YAW_LEFT;
				else
					yaw = YAW_NEUTRAL;

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
		}

		SendToMultiWii(roll, pitch, yaw, height, 0, 0);
	}
}

#define MSP_IDENT                100   //out message         multitype + multiwii version + protocol version + capability variable
#define MSP_STATUS               101   //out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RAW_IMU              102   //out message         9 DOF
#define MSP_SERVO                103   //out message         8 servos
#define MSP_MOTOR                104   //out message         8 motors
#define MSP_RC                   105   //out message         8 rc chan
#define MSP_RAW_GPS              106   //out message         fix, numsat, lat, lon, alt, speed, ground course
#define MSP_COMP_GPS             107   //out message         distance home, direction home
#define MSP_ATTITUDE             108   //out message         2 angles 1 heading
#define MSP_ALTITUDE             109   //out message         altitude, variometer
#define MSP_BAT                  110   //out message         vbat, powermetersum
#define MSP_RC_TUNING            111   //out message         rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_PID                  112   //out message         up to 16 P I D (8 are used)
#define MSP_BOX                  113   //out message         up to 16 checkbox (11 are used)
#define MSP_MISC                 114   //out message         powermeter trig + 8 free for future use
#define MSP_MOTOR_PINS           115   //out message         which pins are in use for motors & servos, for GUI
#define MSP_BOXNAMES             116   //out message         the aux switch names
#define MSP_PIDNAMES             117   //out message         the PID names
#define MSP_WP                   118   //out message         get a WP, WP# is in the payload, returns (WP#, lat, lon, alt, flags) WP#0-home, WP#16-poshold

#define MSP_SET_RAW_RC           200   //in message          8 rc chan
#define MSP_SET_RAW_GPS          201   //in message          fix, numsat, lat, lon, alt, speed
#define MSP_SET_PID              202   //in message          up to 16 P I D (8 are used)
#define MSP_SET_BOX              203   //in message          up to 16 checkbox (11 are used)
#define MSP_SET_RC_TUNING        204   //in message          rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_ACC_CALIBRATION      205   //in message          no param
#define MSP_MAG_CALIBRATION      206   //in message          no param
#define MSP_SET_MISC             207   //in message          powermeter trig + 8 free for future use
#define MSP_RESET_CONF           208   //in message          no param
#define MSP_SET_WP               209   //in message          sets a given WP (WP#,lat, lon, alt, flags)
#define MSP_SELECT_SETTING       210   //in message          Select Setting Number (0-2)

#define MSP_SPEK_BIND            240   //in message          no param

#define MSP_EEPROM_WRITE         250   //in message          no param

#define MSP_DEBUGMSG             253   //out message         debug string buffer
#define MSP_DEBUG                254   //out message         debug1,debug2,debug3,debug4

void SendToMultiWii(int roll, int pitch, int yaw, int height, int aux1, int aux2)
{
	int data_length = 16;
	char c = 0;
	unsigned char send_buf[data_length + 6];
	send_buf[0] = (unsigned char)'S';
	send_buf[1] = (unsigned char)'M';
	send_buf[2] = (unsigned char)'<';
	send_buf[3] = MSP_SET_RAW_RC;
	send_buf[4] = data_length; // data length

	BufSet16(&send_buf[5], roll);
	BufSet16(&send_buf[7], pitch);
	BufSet16(&send_buf[9], yaw);
	BufSet16(&send_buf[11], 1000); // throttle should be controlled by multiwii
	BufSet16(&send_buf[13], aux1);
	BufSet16(&send_buf[15], aux2);
	BufSet16(&send_buf[17], height);
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





























