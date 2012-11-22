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
#define CHANGER_DATE		"16 March 1999"
#define CHANGER_WWW		"http://unfix.org/projects/changer"
#define CHANGER_DEVNAME	"changer"

/* IOCTL */
#define CHANGER_SET	 		_IO(CHANGER_MAJOR,42)

/* Maximum of supported minor devices, thus the number of wrapped cd-slots. */
#define CHANGER_MAX_MINOR	256
#define CHANGER_NAME_LEN	20

/* Struct for CHANGER_SET IOCTL. */
struct changer_set
{
	kdev_t	dev;							/* The device. */
	int		slot;							/* Slot on the device. */
	char		name[CHANGER_NAME_LEN];	/* /dev/changer/<name>. */
};

/* Stuff that only the kernel needs to know. */
#ifdef __KERNEL__

#include <linux/mm.h>
#include <linux/fs.h>
#include <asm/semaphore.h>

/* Struct for devices in the changer array. */
struct changer_dev
{
	kdev_t							dev;			/* Device */
	int								opencount;	/* Use count */
	struct cdrom_device_info	cdi;			/* Cdrom Device Info. */
	int								slot;			/* Slot on the device. */
};

/* Struct for keeping count of how many opens a specific device has.
	This is for open/closes etc. As I only want and can open a device once. */
struct changer_maj
{
	int opencount;			/* How many slots I've got open. */
	int minor[255];		/* Which slot is which of my minors? For dup-checking. */
};

#endif __KERNEL__
#endif _CHANGER_H

