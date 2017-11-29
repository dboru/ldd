/*
* usbdev.h  - the header file with the ioctl definitions.
* This header file need to be known the usb driver (kernel module) and
* the userspace program calling ioctl.
*/

#ifndef USBDEV_H
#define USBDEV_H

#include <linux/ioctl.h>

/*
* The major device number 
*/
#define MINOR_NUM 192

/* 
 * Set the message of the device driver 
  */
  #define IOCTL_SET_MSG _IOR(MINOR_NUM, 0, char *)

/*
 * _IOR means that we're creating an ioctl command 
 * number for passing information from a user process
 * to the kernel module. 
 * The first arguments, MAJOR_NUM, is the major device 
 * number we're using.
 *
 * The second argument is the number of the command 
 * (there could be several with different meanings).
 *
 * The third argument is the type we want to get from 
 * the process to the kernel.
*/

/* 
 * Get the message of the device driver 
 */
 #define IOCTL_GET_MSG _IOR(MINOR_NUM, 1, char *)
/* 
 * This IOCTL is used for output, to get the message 
 * of the device driver. However, we still need the 
 * buffer to place the message in to be input, 
 * as it is allocated by the process.
*/

/* 
 * The name of the device file 
 */

 #define DEVICE_FILE_NAME "skel0"
 #endif
