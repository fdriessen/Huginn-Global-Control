#ifndef SERIAL_H_
#define SERIAL_H_

#include <termios.h>
#include <errno.h>
#include <unistd.h>

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

#endif /* SERIAL_H_ */
