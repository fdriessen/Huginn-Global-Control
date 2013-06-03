/*
 * common.h
 *
 *  Created on: 30 mei 2013
 *      Author: Floris
 */

#ifndef COMMON_H_
#define COMMON_H_

#define	true				1
#define	false				!true
#define null				NULL

#define	MODE_LINE_FOLLOW	1
#define	MODE_LINE_HOVER		2
#define	MODE_LINE_ROTATE	3

#define LINE_DETECT_FIFO	"LDFIFO"
#define ANTI_DRIFT_FIFO		"ADFIFO"

typedef struct _ld_information
{
	int mode;
	union
	{
		ld_follow_info follow_info;
		ld_hover_info hover_info;
		ld_rotate_info rotate_info;
	} data;
} ld_information;

typedef struct _ld_follow_info
{
	float	angle_sp;
	float	angle_cv;
	int		x_sp;
	int		x_cv;
} ld_follow_info;

typedef struct _ld_hover_info
{
	float	angle_sp;
	float	angle_cv;
	int		x_sp;
	int		x_cv;
	int		y_sp;
	int		y_cv;
} ld_hover_info;

typedef struct _ld_rotate_info
{
	int		x_sp;
	int		x_cv;
	int		y_sp;
	int		y_cv;
} ld_rotate_info;


#endif /* COMMON_H_ */
