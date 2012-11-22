/**********************************************************************************************************************
  /dev/changer by Jeroen Massar aka Fuzzel/Un(f)iX <jmassar@unfix.org>               http://unfix.org/projects/changer
***********************************************************************************************************************
 TAB=3
 See changer.c for more info.
**********************************************************************************************************************/
#ifndef _CHANGER_H
#define _CHANGER_H

#include <linux/major.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/blkdev.h>
#include <linux/cdrom.h>

#define CHANGER_SHORT	"changer"
#define CHANGER_AUTHOR	"Jeroen Massar <jmassar@unfix.org>"
#define CHANGER_FULL		"Changer CdRom Wrapper Device"
#define CHANGER_DATE		"17 March 1999"
#define CHANGER_VERSION	19990317
#define CHANGER_WWW		"http://unfix.org/projects/changer"
#define CHANGER_DEVNAME	"changer"

/* IOCTL */
#define CHANGER_SET	 		_IO(CHANGER_MAJOR,42)

/* Maximum of supported minor devices, thus the number of wrapped cd-slots. */
#define CHANGER_MAX_MINOR	256
#define CHANGER_NAME_LEN	20			/* Buffers can be overflowed so watch it. */
#define CHANGER_MAX_MINORS	256		/* Max minors */

/* Struct for CHANGER_SET IOCTL. */
struct changer_set
{
	int		version;						/* Version. */
	kdev_t	dev;							/* The device (major:minor). */
	int		slot;							/* Slot on the device. */
	char		name[CHANGER_NAME_LEN];	/* The name the thing gets in /proc//sys/dev/cdrom/info. */
												/* eg. "/dev/changer5". */
};

/* Stuff that only the kernel needs to know. */
#ifdef __KERNEL__

#include <linux/mm.h>
#include <linux/fs.h>
#include <asm/semaphore.h>

/* Struct for devices in the changer array. */
struct changer_dev
{
	kdev_t							dev;								/* Device (child's maj:min). */
	int								opencount;						/* Use count (opens of the changer device). */
	struct cdrom_device_info	cdi;								/* Cdrom Device Info. */
	int								slot;								/* Slot of the device. (which slot this is on the device). */
};

/* Struct for keeping count of how many opens a specific device has.
	This is for open/closes etc. As I only want and can open a device once. */
struct changer_maj
{
	int								opencount;						/* How many slots I've opened on this device. */
	int								minor[CHANGER_MAX_MINORS];	/* Which slot is which of my minors? (for changer_devs[.?.]. */
	spinlock_t						spinlock;						/* Spinlock holding off changing of a device. */
	struct cdrom_device_info	*cdi;								/* Cached pointer to the cdi. */
};

#endif __KERNEL__
#endif _CHANGER_H

