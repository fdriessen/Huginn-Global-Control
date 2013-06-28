#ifndef CONFIG_H
#define CONFIG_H

// debugging?
#define	DEBUG
// debug level
// 0 = all
// 1 = only important stuff and errors
// 2 = only errors
#ifdef	DEBUG
#define	DEBUG_LEVEL	0
#endif

// Platform
#define PLATFORM_BB		0 // beagle board
#define PLATFORM_x86	1 // linux x86 compatible
#define	PLATFORM		PLATFORM_x86

#if PLATFORM == PLATFORM_x86	
#define SHOW_WINDOW
#endif

#if PLATFORM == PLATFORM_BB
#define WEBCAM_RESIZE
#endif

#endif