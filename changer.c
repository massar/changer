/**********************************************************************************************************************
  /dev/changer by Jeroen Massar aka Fuzzel/Un(f)iX <jmassar@unfix.org>               http://unfix.org/projects/changer
***********************************************************************************************************************
 TAB=3

 Copyright (C) 1999, Jeroen Massar

 Introduction
	Normally it isn't allowed to mount all the discs of a changer at once, that is because
	when you mount one slot you can't change to another anymore, because the cdrom gets locked.
	And ofcourse you'll have to unmount it and then you'll have to change discs by hand.
	Ok you could do this with a script... but what if you want to share the drives with NFS or
	Samba and you don't want people to have to hassle with the scripts... let alone when multiple
	people try...

 Description
	This little wrapper solves this problem. When it is loaded by the kernel it seeks in the
	Unified CdRom driver list and checks for changers (capacity>1&changable). It adds these
	devices to an changer-internal list, opens them and adds them to the changer-devices minor
	list. The devices changer:minor can then be mounted like any other normal cdrom device, as
	it acts like a single cdrom device... The other minors can also be mounted at the same time
	as the changer-wrapper will change to the appropriate slot when it needs access to it.

 Unsolved Problems:
 	- A changer can't actually use two slots at the same time... this is a technical problem
 	  so when one user uses a slot and another user uses another the changer will keep changing
 	  between them... The only way I see to solve this is to patch the kernel that when a file
 	  on a mounted filesystem of a changer device is accessed that the changer is locked. Then
 	  at least the time while the file is transfered there will be no changes.

 Features
	See the History part for when what was added.
	- Allows mounting of all discs in a changer at once.
	- It's free.
	- 42 : the solution to anything (at least according to the Hitchhikers Guide to the Galaxy...)

 Future Features (things on my mind)
	- Samba Config Editing (can create cd-entries from the names of the cd's).
	  Should be done in userspace with some kind of callback.
	- Request Queueing, so that subsequent requests are done at once instead of changing discs in between.
	  See "lkmpg/Scheduling Tasks"
	- Precaching? Or should this be set somewhere else??
	- mail jmassar@unfix.org if you got any ideas :)

 Parameters
 	- BOOL	noauto		- Disable auto-changer-detection.
 	- STRING	exclude		- Exclude the devicenames from auto-changer-detection.

 Revisions
	A version number is simply the date of the driver. So 19780225 = 25 February 1978.
	The newest version will always be available from the Un(f)iX Website at :
		http://unfix.org/projects/changer

 Legal Mumbo Jumbo
	Well first thing first its my idea, so I want my name to be kept with it too.
	For the rest... it's for Linux - The Peoples Operating System, so copy it whenever
	you can/want etc. see all the GPL info etc. If you got some additions etc. tell me about
	them and I'll be glad to include them. See the GPL added to the end of this header.

 Acknowledgements
	A lot of info on how to actually implement it came from md (the multiple-disk driver),
	nbd (the network block driver) and the loop device, all of which use sub/child-devices.
	I'd also like to thank the following persons:
		Linus Torvalds
			- for making Linux in the first place :)
		Marc Zyngier
			- for making the md driver which I learned this code from
			  and as the md driver is also being used for my soft-RAID-0 /archive :)
		Theodore Ts'o
			- for the loop device, which I had a good look at for the request part.
		David A. van Leeuwen, Jens Axboe, Scott Snyder and Erik Andersen
			- for the standardization of the cdrom code and for implementing
			  the selection of disc-slots.
		Paradigit Computer Store
			- for having a Teac CD-C68E priced at 200 guilders.
		Andrew Tridgell & The Samba Team
			- for Samba, allowing me to share the changer to my NT Xterminal.
		Emiel "Clone" Lensink, Pieter "Suf-Duck" de Boer, Jean-Paul "Corwin" Mars,
			Jeroen "Spook" Akershoek, Vera "Fear" Mafait
			- for being Un(f)iX...
		Beer/Neworder & Andabata
			- The Webhosting of unfix.org.
		#linux.nl
			- for the general laughter...

 History (child=underlying lowleveldriver)
	20 November 1998
		- I bought a Teac CD-C68E 6-cd changer, though ATAPI for 200 guilders I couldn't
		  go wrong and I didn't, it's great, I've simply put all my so-much-needed cd's
		  in and voila, but when I wanted to access it I had to manually umount and mount
		  it <grmbl>, that is not what I want todo each time so I needed a solution and
		  I thought up /dev/changer, which should release me of the unmounts and make any
		  changer look like it is are multiple seperate cd's. I hope many people can use
		  this... and that this small token will be noted as one of my contributions to Linux.
		- My NT Xterminal broke... for no apparent reason what so ever...
	22 November 1998
		- Ok got my NT Xterminal to work again... let's start this little project.
		- Looked into the kernel source and found out that I'd best base this on cdrom.c
		  and one of the "low-level" drivers. I've made the step to 2.1.123 so I can access
		  my changer with more options as 2.1 supports the CDROM_SELECT_SLOT call which I need.
		  And also the reporting of slots etc. which could come in handy.
		- Hmmm I've read the cdrom documentation with xdvi... cool program :)
		- Read in the cdrom-docs about setcd for Debian... did a dselect and hey... there's
		  my way to read volume labels, allowing volume names to be used.
	24 November 1998
		- The module loads and doesn't crash the system :)
		- Know probably know fully how I'm gonna solve this problem.
		- Added an IOCTL called CHANGER_SET which will allow the set of a major:minor to
		  be a device. When the IOCTL is called it registers the cdrom. It can also be used
		  to unset the device.
		- I only have to pass the IOCTL's to the sub-device and it should work...
	25 November 1998
		- Ok the thingy should work now...
		- I'll have to write a userspace proggy first though...
	26 November 1998
		- Implementing scanning of the cdi list... for auto adding changers.
		  This will only add cdroms with a capacity>1&changable named chg0, chg1 etc...
		  The number is the minor. This lets me skip the userspace proggy... for now...
		- Will add an 'exclude' parameter to allow exclusion of changers.
		- Wh00pie... it works.. at least the cdrom.c wrapping... the request not (yet).
		- Settings are now adapted from the child driver.
		- The mask is set according to the child's inabilities...
	28 November 1998
		- Put only an end_request(0) into request, and when I mount a disc from slot 0
		  it all goes well, altough it stops due to the failed request. When I remove the
		  disc it senses it ok, also for other slots. So the IOCTL wrapper works. Now I'll
		  have to work out the request.
		- Fixed releasing of the child when I'd have no slots in use of the child.
	16 March 1998 (RELEASED (bz2+tgz) - 1st public alpha)
		- Coded the request part using a good look at the loop-driver.
		- It starts to work much better now... as it actually reads data, at least
		  I hear and see the cd change and then see a second or two of data transfer.
		- 01:21 I can mount cd's! One problem... as the buffer's aren't invalidated all cd's seem
		  the same to the system... I'll have to find out a way to invalidate them.
	17 March 1998 (RELEASED (bz2+tgz) - 2nd public alpha - announced on c.o.l.a.)
		- Implemented a spinlock for each child device. This should hold it from changing discs
		  while a operation is in progress and then when another task gets scheduled.
		- Added some documentation to clear up some of the weird things I did.
		- 18:42 - IT WORKS!!!!! well except for slot 0 at my setup... hmmm... :)
		  /me happy... it also works by listing to cd's at once eg: 'ls -la -R /cdrom/4' and list another
		  on another tty and they will change every so often...
	18 March 1998 (RELEASED (bz2+tgz) - 3rd public alpha)
		- Implemented Debug levels.
		- I now re-set the blk_size[major][minor] in changer_change_slot().
		- And voila, accessing files work too, well it did before but then it had to be in the first
		  1024 sectors of the cd, now it can be anywhere...
		- Redesigned website to conform to new unfix.org style.
		- Included it in such a way that it is kernel-tree ready, and people can compile and test it.

Notes:
	drive_status & media_changed also pass along a slot number... does it have to be changed first then
	Andd eehmmm it doesn't lock the drive on mount... ("lock the changer.... didn't lock the changer").

Todo
	- *Global* spinlock/mutex for the global lists (making it 100% multi-threaded(-kernel) safe).
 	- User space program for non-autodetected cd-roms.
	- CHANGER_GET IOCTL for checking which changer:minor & name is connected to what device?
	- CHANGER_SET_LEVEL IOCTL for runtime setting output-levels.
	** CLEANUP THE CODE... call subs childs etc...
	- Add more paranoia checkings (NODEV+minor<MAX_MINOR checking etc..) NON-NULL conditions in structs etc...
	- optimize where possible.
	- Parameter handling.
	- SysCtl Interface (cdrom_sysctl_register() and friends)
		- Shows mappings eg:
			8<--------------------------------------
			Changer Information
			<device>	<minors>
			hdc		0,1,2,3,4,5
			scd0		6,7,8,9,10,11,12,13,14
			-------------------------------------->8

 Bugs:
	- Deadlocks on very heavy use... maybe it's something else but I'll have to look into it more deeply...
	  Maybe it's the "Plugging without Deadlocking"? from the drivers/block/ll_rw_block.c ??
	- I should make a kernel diff...
	- Not conforming to the linux kernel codingstyles...

 Notes:
	- This driver obviously doesn't support the CDC_SELECT_DISC capability... <grin>

*** Personal Notes ****************************************************************************************************
#define you the_one_reading_this()
#define me	"Jeroen Massar"
if (you==me)
{
 Changes to files (for nosy people: these are currently embedded in this file, see at "FILE:" below...)
	- include/linux/blk.h				- Added device entry.									- see CHANGER_MAJOR
	- include/linux/major.h				- Added CHANGER_MAJOR = 42								- see CHANGER_MAJOR
	- init/main.c							- Added changer_setup()									- see CONFIG_CHANGER
												- Added device number.									- see CONFIG_CHANGER
	- cdrom/Makefile						- Added objects.											- see changer.o
	- cdrom/Config.in						- Added compilation stuff.								- see CONFIG_CHANGER
	- block/ll_rw_blk.c					- Added changer_init()									- see CONFIG_CHANGER
												- Added 'Plugging without Deadlocking' support	- see CHANGER_MAJOR
	- Documentation/Configure.help	- Added help text.										- see CONFIG_CHANGER
									
 Extras:
	/proc/sys/cdrom/info 	- For all the cdroms in a system (using cdrom-wrapper).
	/proc/modules				- Modules info (see also mod* cmd's).
}

***********************************************************************************************************************
 GNU Public License
***********************************************************************************************************************
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**********************************************************************************************************************/

/**********************************************************************************************************************
  Global Static Definitions
**********************************************************************************************************************/
#include "changer.h"

const char c_full[]	= CHANGER_FULL;
const char c_short[]	= CHANGER_SHORT;
const char c_date[]	= CHANGER_DATE;
const char c_by[]		= CHANGER_AUTHOR;
const char c_www[]	= CHANGER_WWW;

/* We'll use 42 as a testing major number (see major.h and blk.h). */
#define MAJOR_NR CHANGER_MAJOR

/**********************************************************************************************************************
  Export Prototypes
**********************************************************************************************************************/
static int changer_init(void);
static void changer_cleanup(void);
static struct cdrom_device_info *changer_get_cdi(struct cdrom_device_info *cdi);
static int changer_change_slot(int minor);
static int changer_set(struct cdrom_device_info *cdi,struct changer_set *set);

/* wraps */
static int changer_wrap_open(struct cdrom_device_info *cdi);
static void changer_wrap_release(struct cdrom_device_info *cdi);

/* blkdev */
static void do_changer_request(void);

/* for cdrom.c... */
static int changer_open(struct cdrom_device_info *cdi,int purpose);
static void changer_release(struct cdrom_device_info *cdi);
static int changer_drive_status(struct cdrom_device_info *cdi,int slot_nr);
static int changer_media_changed(struct cdrom_device_info *cdi,int disc_nr);
static int changer_tray_move(struct cdrom_device_info *cdi,int position);
static int changer_lock_door(struct cdrom_device_info *cdi,int lock);
static int changer_select_speed(struct cdrom_device_info *cdi,int speed);
static int changer_get_last_session(struct cdrom_device_info *cdi,struct cdrom_multisession *ms_info);
static int changer_get_mcn(struct cdrom_device_info *cdi,struct cdrom_mcn *mcn);
static int changer_reset(struct cdrom_device_info *cdi);
static int changer_audio_ioctl(struct cdrom_device_info *cdi,unsigned int cmd,void *arg);
static int changer_dev_ioctl(struct cdrom_device_info *cdi,unsigned int cmd,unsigned long arg);

/**********************************************************************************************************************
  Stuff that actually has to belong in other files.
  NOTE: When this driver actually gets included into the linux kernel source,
		  these are the files (FILE is relative to kernel-root) to be changed.
		  This way it only allows for modulized builds, but it's alpha code
		  so you don't want it in the kernel yet.
		  (mail me and I'll make a nice diff for the kernel-source)
**********************************************************************************************************************/
/* FILE: include/linux/major.h */
#define CHANGER_MAJOR	42

/* FILE: init/main.c */
/* Somewhere at the end, or at least behind all other cdrom drivers. */
/*extern void changer_setup(char *str,int *ints);*/

/* FILE: include/linux/blk.h (NOTE: Add it as one of the last drivers). */
/* #ifdef CONFIG_CDROM_CHANGER */
#define DEVICE_NAME "changer"
#define DEVICE_REQUEST do_changer_request
#define DEVICE_NR(device) (MINOR(device))
#define DEVICE_ON(device)
#define DEVICE_OFF(device)
/* ... */

/* FILE: drivers/block/ll_rw_block.c
#ifdef CONFIG_CDROM_CHANGER
	changer_init();
#endif
*/

/* FILE: CREDITS
N: Jeroen Massar
E: jmassar@unfix.org
W: http://unfix.org
W: http://unfix.org/members/jmassar
W: http://unfix.org/projects/changer
D: /dev/changer, unfix.org BOFH.
S: Jeroen Massar
S: Gortmolenerf 46
S: 2807 EJ, Gouda
S: The Netherlands
*/

/* FILE: MAINTAINERS
CHANGER: /dev/changer
P: Jeroen Massar
M: jmassar@unfix.org
W: http://unfix.org/projects/changer
S: Maintained
*/

/* FILE: Documentation/Configure.help
/dev/changer
CONFIG_CDROM_CHANGER
  This driver allows you to mount all the slots of a changer at once.
  For updates, documentation and a small userland-tool check out:
	  http://unfix.org/projects/changer

  If you are not sure, say N; unless you've got a changer then say
  Y ofcourse... you'll like it.

*/
/**********************************************************************************************************************
  Includes
**********************************************************************************************************************/
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdrom.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/blk.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define LEVEL_NONE	0x0
#define LEVEL_DEBUG	0x1
#define LEVEL_FUNC	0x2
#define LEVEL_XS		0x4
#define LEVEL_INFO	0x8
#define LEVEL_ERR		0x16
#define LEVEL_WARN	0x32
#define LEVEL_ALL		-1

/* Enable Debug Ability 1=On, else Off. */
#define ENABLE_DEBUG_ABILITY		1

/**********************************************************************************************************************
  Macros
**********************************************************************************************************************/
#define INFO(lev,fmt,args...)		if (changer_output_level&lev) printk(KERN_INFO CHANGER_SHORT ": " fmt, ## args)
#if (ENABLE_DEBUG_ABILITY==1)
#define DEBUG(lev,fmt,args...)	if ((changer_output_level&LEVEL_DEBUG)&&(changer_output_level&lev)) printk(KERN_DEBUG CHANGER_SHORT ": " fmt, ## args)
#else
#define DEBUG(lev,fmt,args...)
#endif

#define CHG								changer_devs[minor]
#define CAP(func,cap)				if (o_cdi->ops->func==NULL) CHG.cdi.mask&=~(cap)
#define LOCK_DEVICE()				spin_lock(changer_majors[MAJOR(CHG.dev)].spinlock);
#define UNLOCK_DEVICE()				spin_unlock(changer_majors[MAJOR(CHG.dev)].spinlock);

/**********************************************************************************************************************
  Module Stuff
**********************************************************************************************************************/
MODULE_AUTHOR(CHANGER_AUTHOR);
MODULE_DESCRIPTION(CHANGER_FULL);
MODULE_SUPPORTED_DEVICE("cdrom");

/**********************************************************************************************************************
  Structs and other globals.
**********************************************************************************************************************/
static struct changer_dev changer_devs[CHANGER_MAX_MINOR];				/* Minor->Device mapping. */
static int changer_sizes[CHANGER_MAX_MINOR];									/* Sizes. */
static int changer_blksizes[CHANGER_MAX_MINOR];								/* Block Sizes. */
static struct changer_maj changer_majors[MAX_BLKDEV];						/* Which slot is which of my minors?
																							   For dup-checking. */
int changer_output_level=LEVEL_INFO|LEVEL_ERR;								/* Output level. */

/* capabilities (all except CDC_SELECT_DISC) */
#define CHANGER_CAPABILITIES	CDC_CLOSE_TRAY|CDC_OPEN_TRAY|CDC_LOCK|CDC_SELECT_SPEED| \
										CDC_MULTI_SESSION|CDC_MCN|CDC_MEDIA_CHANGED|CDC_PLAY_AUDIO| \
										CDC_RESET|CDC_IOCTLS|CDC_DRIVE_STATUS

static struct cdrom_device_ops changer_dops =
{
	changer_open,																		/* open */
	changer_release,																	/* release */
	changer_drive_status,															/* drive status */
	changer_media_changed,															/* media changed */
	changer_tray_move,																/* tray move */
	changer_lock_door,																/* lock door */
	changer_select_speed,															/* select speed */
	NULL,																					/* select disc */
	changer_get_last_session,														/* get last session */
	changer_get_mcn,																	/* get universal product code */
	changer_reset,																		/* hard reset */
	changer_audio_ioctl,																/* audio ioctl */
	changer_dev_ioctl,																/* device-specific ioctl */
	CHANGER_CAPABILITIES,															/* capabilities */
	0,																						/* number of active minor devices */
};

/**********************************************************************************************************************
  Internal Functions
**********************************************************************************************************************/
static struct cdrom_device_info *changer_get_cdi(struct cdrom_device_info *cdi)
{
	int		minor=MINOR(cdi->dev);
	kdev_t	dev=CHG.dev;
	while ((cdi!=NULL)&&(cdi->dev!=dev)) cdi=cdi->next;
	return(cdi);
}

/* Change the disk.
	should be used in the following sequence:
	8<-------------------------------------
		LOCK_DEVICE();
		changer_change_slot(minor);
		<operation: read/write/info/...>
		UNLOCK_DEVICE();
	------------------------------------->8
*/
static int changer_change_slot(minor)
{
	int result=0;
	DEBUG(LEVEL_FUNC,"changer_change_slot(%02x)\n",minor);
	blk_size[MAJOR_NR][minor]=blk_size[MAJOR(CHG.dev)][MINOR(CHG.dev)];
	result=changer_majors[MINOR(CHG.dev)].cdi->ops->select_disc(changer_majors[MINOR(CHG.dev)].cdi,CHG.slot);
	return(result);
}

static int changer_set(struct cdrom_device_info *cdi,struct changer_set *set)
{
	int minor=MINOR(cdi->dev),result=0,ret,maj,min;
	struct cdrom_device_info *o_cdi=NULL;

	// if set is supplied it must be my version, no set is ok too...
	if (((set)&&(set->version==CHANGER_VERSION))||(set==NULL))
	{
		/* Was it used before? */
		if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
		{
			maj=MAJOR(CHG.dev);
			min=MINOR(CHG.dev);
			INFO(LEVEL_INFO,"changer_set(): UN-Setting minor=%02x device major:minor %02x:%02x slot %d...\n",minor,maj,min,CHG.slot);
			/* Close/Release the device when opencount reaches 0. */
			if (changer_majors[maj].opencount==0)
			{
				DEBUG(LEVEL_INFO,"changer_set(): Closing the device...(opencount==0)\n");
				changer_wrap_release(changer_majors[MINOR(CHG.dev)].cdi);
			}
			/* ?? Un-mount/lock/... ?? */
			/* Unregister MAJOR_NR,minor as cdrom. */
			unregister_cdrom(&CHG.cdi);
			changer_majors[maj].minor[min]=NODEV;
			changer_majors[maj].opencount--;
		}
		if (set)
		{
			maj=MAJOR(set->dev);
			if (maj!=MAJOR_NR)
			{
				min=MINOR(set->dev);
				/* Copy attributes / Adapt Settings. */
				CHG.dev=set->dev;
				CHG.opencount=0;
				CHG.slot=set->slot;
	
				if ((o_cdi=changer_get_cdi(cdi))&&(o_cdi->ops))
				{
					/* Cache the cdi. */
					changer_majors[MINOR(CHG.dev)].cdi=o_cdi;
	
					/* Fill in specific dops. */
					CHG.cdi.ops=&changer_dops;
					CHG.cdi.handle=NULL;
					CHG.cdi.dev = MKDEV(MAJOR_NR,minor);
					CHG.cdi.mask=CHANGER_CAPABILITIES;
					CHG.cdi.speed=o_cdi->speed;
					CHG.cdi.capacity=1;
					CHG.cdi.options=o_cdi->options;
					CHG.cdi.mc_flags=0;
					CHG.cdi.use_count=0;
	
					/* Disable capabilities the drive doesn't have functions for. */
					CAP(drive_status,CDC_DRIVE_STATUS);
					CAP(media_changed,CDC_MEDIA_CHANGED);
					CAP(tray_move,CDC_CLOSE_TRAY+CDC_CLOSE_TRAY);
					CAP(lock_door,CDC_LOCK);
					CAP(select_speed,CDC_SELECT_SPEED);
					CAP(select_disc,CDC_SELECT_DISC);
					CAP(get_last_session,CDC_MULTI_SESSION);
					CAP(get_mcn,CDC_MCN);
					CAP(reset,CDC_RESET);
					CAP(audio_ioctl,CDC_PLAY_AUDIO);
					strncpy(CHG.cdi.name,set->name,(20-1));
					INFO(LEVEL_INFO,"changer_set(): Setting minor=%02x device major:minor %02x:%02x slot %d...\n",minor,maj,min,set->slot);
					if (((changer_majors[maj].opencount)>0)||
							(((changer_majors[maj].opencount)==0)&&(result=changer_wrap_open(o_cdi)==0)))
					{
						DEBUG(LEVEL_INFO,"changer_set(): Initializing...\n");
						changer_majors[maj].opencount++;
						changer_majors[maj].minor[min]=minor;
			
						/* Register MAJOR_NR,minor as cdrom. */
						DEBUG(LEVEL_INFO,"changer_set(): Registering cdrom...\n");
						if ((ret=register_cdrom(&CHG.cdi))==0)
						{
							result=0;
							blk_size[MAJOR_NR][minor]=blk_size[MAJOR(CHG.dev)][MINOR(CHG.dev)];
						}
						else
						{
							INFO(LEVEL_ERR,"Cannot become a registered cdrom on major:minor %02x:%02x! (register_cdrom() returned %d)\n",MAJOR_NR,minor,ret);
							unregister_cdrom(&CHG.cdi);
							CHG.dev=NODEV;
							result=-EIO;
						}
					}
					else
					{
						CHG.dev=NODEV;
						result=-EINVAL;
					}
				}
				else
				{
					INFO(LEVEL_ERR,"changer_set(): Couldn't get cdi or cdi->ops is empty...\n");
					CHG.dev=NODEV;
					result=-EINVAL;
				}
			}
			else
			{
				INFO(LEVEL_ERR,"Hmmmm eehmmm someone/thing tried to add myself to myself... wellps... that's a no go...\n");
				result=-EINVAL;
			}
		}
		else result=-EINVAL;
	}
	else
	{
		INFO(LEVEL_ERR,"Hi I speak version %d, NOT version %d, which you passed me, update your usertools.\n",CHANGER_VERSION,set->version);
		result=-EINVAL;
	}
	return(result);
}

/**********************************************************************************************************************
  The actual wrapper functions.
***********************************************************************************************************************
  Make from cdi the struct inode *ip and struct file *fp for the IOCTL call.
**********************************************************************************************************************/
/* SHOULD WORK: Simply gets the kdev_t from the child-table and sets the flag. */
static int changer_wrap_open(struct cdrom_device_info *cdi)
{
	int				minor=MINOR(cdi->dev);
	struct inode	ip;
	struct file		fp;

	blk_size[MAJOR_NR][minor]=blk_size[MAJOR(CHG.dev)][MINOR(CHG.dev)];
	ip.i_rdev=CHG.dev;
	fp.f_flags=O_NONBLOCK;
	return(cdrom_fops.open(&ip,&fp));
}

/* SHOULD WORK: Simply gets the kdev_t from the child-table and sets the flag. */
static void changer_wrap_release(struct cdrom_device_info *cdi)
{
	int				minor=MINOR(cdi->dev);
	struct inode	ip;
	struct file		fp;

	ip.i_rdev=CHG.dev;
	fp.f_flags=O_NONBLOCK;
	fp.f_mode=0;
	cdrom_fops.release(&ip,&fp);
}

/* This needs a lot of testing! */
static void do_changer_request(void)
{
	struct request			*req;
	int						minor,ok,blksize,block,offset,len,size;
	char						*dest;
	struct buffer_head	*bh;

	while (CURRENT)
	{
		ok=1;

		/* Get current request off the list. */
		req=CURRENT;
		CURRENT=CURRENT->next;

		/* It should be ours. */
		if (MAJOR(req->rq_dev)==MAJOR_NR)
		{
			minor=MINOR(req->rq_dev);
			/* Is it a used minor? */
			if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
			{
				DEBUG(LEVEL_FUNC,"changer_request(): The request is for minor %02x child %02x:%02x\n",minor,MAJOR(CHG.dev),MINOR(CHG.dev));

				/* Calculate block sizes. */
				blksize=BLOCK_SIZE;
				if (blksize_size[MAJOR(CHG.dev)])
				{
					blksize=blksize_size[MAJOR(CHG.dev)][MINOR(CHG.dev)];
					if (!blksize) blksize=BLOCK_SIZE;
				}
				dest=req->buffer;
				if (blksize<512)
				{
					block=req->sector*(512/blksize);
					offset=0;
				}
				else
				{
					block=req->sector/(blksize>>9);
					offset=(req->sector%(blksize>>9))<<9;
				}
				if (offset>blksize)
				{
					block++;
					offset-=blksize;
				}
				len=req->current_nr_sectors<<9;
				DEBUG(LEVEL_XS,"Determined sector=%d, len=%d, offset=%d, block=%d, blksize=%d\n",(int)req->sector,len,offset,block,blksize);

				/* Unlock the request so others can procede. */
				spin_unlock_irq(&io_request_lock);
				/* Lock the device, and change to the correct disc. */
				LOCK_DEVICE();
				changer_change_slot(minor);
				invalidate_buffers(CHG.dev);
				/* Collect all the pieces. */
				while (len>0)
				{
					size=blksize-offset;
					if (size>len) size=len;
					if ((bh=getblk(CHG.dev,block,blksize)))
					{
						DEBUG(LEVEL_XS,"changer_request(): getblk() OK\n");
						if (!buffer_uptodate(bh)&&((req->cmd==READ)||(offset||(len<blksize))))
						{
							DEBUG(LEVEL_XS,"changer_request(): Calling ll_rw_block().\n");
							ll_rw_block(READ,1,&bh);
							wait_on_buffer(bh);
							if (!buffer_uptodate(bh))
							{
								INFO(LEVEL_ERR,"changer_request(): Unable to update buffers...\n");
								ok=0;
							}
						}
						else DEBUG(LEVEL_INFO,"changer_request(): Buffers were up-to-date.\n");
						/* copy the read data to the buffer */
						if (ok) memcpy(dest,bh->b_data+offset,BLOCK_SIZE);
						/* Release the buffer head */
						brelse(bh);
					}
					else
					{
						INFO(LEVEL_ERR,"changer_request(): Device %s: getblk(-, %d, %d) returned NULL\n",kdevname(CHG.dev),block,blksize);
						ok=0;
					}
					dest+=size;
					len-=size;
					offset=0;
					block++;
				}
				/* All done so unlock the disc. */
				UNLOCK_DEVICE();
				spin_lock_irq(&io_request_lock);
			}
			else
			{
				INFO(LEVEL_ERR,"changer_request(): Child %02x is unused...\n",minor);
				ok=0;
			}
		}
		else
		{
			INFO(LEVEL_ERR,"changer_request(): Got a request for %02x:%02x which is NOT one of me...\n",MAJOR(req->rq_dev),MINOR(req->rq_dev));
			ok=0;
		}
		/* Put it back into the list. */
		req->next=CURRENT;
		CURRENT=req;
		DEBUG(LEVEL_FUNC,"Ending the request with %d (1=ok,0=eeks)\n",ok);
		end_request(ok);
	}
}

/**********************************************************************************************************************
  Initialization and other relevant wrappers.
**********************************************************************************************************************/
__initfunc(static void changer_setup(char *str, int *ints))
{
	DEBUG(LEVEL_FUNC,"changer_setup()\n");
}

__initfunc(static void changer_cleanup(void))
{
	int minor;
	struct cdrom_device_info cdi;

	DEBUG(LEVEL_FUNC,"changer_cleanup()\n");
	for (minor=0;minor<CHANGER_MAX_MINOR;minor++)
	{
		cdi.dev=MKDEV(MAJOR_NR,minor);
		changer_set(&cdi,NULL);
	}
	unregister_blkdev(MAJOR_NR,c_short);
}

/* Test if all OK (Called at boot/module-init time). */
__initfunc(static int changer_init(void))
{
	int minor=0,result=0,ret,slot,i;
	struct cdrom_device_info fake_cdi = {&changer_dops,0,0,0,0,0,0,0,0,0,"ChangerFake"},*cdi;
	struct changer_set set = {CHANGER_VERSION,0,0,""};

	DEBUG(LEVEL_FUNC,"changer_init()\n");
	INFO(LEVEL_ALL,"%s (%s)\n",c_full,c_date);
	INFO(LEVEL_ALL,"by %s\n",c_by);
	INFO(LEVEL_ALL,"%s\n",c_www);

	/* Initialize data parts. */
	memset(&changer_devs,0,sizeof(changer_devs));
	memset(&changer_sizes,0,sizeof(changer_sizes));
	memset(&changer_blksizes,0,sizeof(changer_blksizes));
	memset(&changer_majors,0,sizeof(changer_majors));

	/* Initialize all the device spinlocks. */
	for (i=0;i<CHANGER_MAX_MINOR;i++) changer_majors[i].spinlock=SPIN_LOCK_UNLOCKED;

	if ((ret=register_blkdev(MAJOR_NR,c_short,&cdrom_fops))==0)
	{
		blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;
		blk_dev[MAJOR_NR].current_request=NULL;
		blk_size[MAJOR_NR]=changer_sizes;
		blksize_size[MAJOR_NR] = changer_blksizes;
		read_ahead[MAJOR_NR] = INT_MAX;
		
		/* Trick Time */
		/* Temporary add a fake cdrom drive. */
		/* Simply to get the cdi list entries. */
		fake_cdi.dev=MKDEV(MAJOR_NR,CHANGER_MAX_MINOR-1);
		if ((ret=register_cdrom(&fake_cdi))==0)
		{
			cdi=fake_cdi.next;
			/* Travel list and add devices. */
			while (cdi!=NULL)
			{
				if (cdi->capacity>1)
				{
					set.dev=cdi->dev;
					for (slot=0;slot<cdi->capacity;slot++)
					{
						set.slot=slot;
						/* WARNING: Length limits!!! */
						sprintf(set.name,CHANGER_DEVNAME"%d",minor);
						fake_cdi.dev=MKDEV(MAJOR_NR,minor);
						/* Add cdrom device. */
						INFO(LEVEL_ALL,"Adding minor=%02x device major:minor %02x:%02x slot %d...\n",minor,MAJOR(set.dev),MINOR(set.dev),slot);
						if (changer_set(&fake_cdi,&set)==0) minor++;
						/* else failed... */
					}
				}
				/* Next... */
				cdi=cdi->next;
			}
		}
		else
		{
			/* Wellps.. it ain't that important... there is always the userland proggy... */
			INFO(LEVEL_ERR,"Hmmm couldn't add a temporary fake device, use the userland program to add changers (register_cdrom() returned %d)\n",ret);
		}
		/* And unregister the fake device... */
		fake_cdi.dev=MKDEV(MAJOR_NR,CHANGER_MAX_MINOR-1);
		unregister_cdrom(&fake_cdi);
	}
	else
	{
		INFO(LEVEL_ERR,"Unable to register major %02x! (register_blkdev() returned %d)\n",MAJOR_NR,ret);
		result=-EIO;
	}
	if (result==0)	INFO(LEVEL_INFO,"Initialized OK...\n");
	else changer_cleanup();
	return(result);
}

/**********************************************************************************************************************
  The CdRom Wrapper Functions
***********************************************************************************************************************
  Most of the parts of these functions are based on the fact that they "ask" the real driver to perform the function.
**********************************************************************************************************************/
/* Open() the device special file. */
static int changer_open(struct cdrom_device_info *cdi,int purpose)
{
	int minor=MINOR(cdi->dev),result=0;
	DEBUG(LEVEL_FUNC,"changer_open(%02x:purpose=%d)\n",minor,purpose);
	/* Open actual driver -> This is done on CHANGER_SET(). */
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		CHG.opencount++;
		MOD_INC_USE_COUNT;
	}
	else result=-EINVAL;
	return(result);				/* 0 = Success, <0 = Error */
}

/* Release() - Close device. */
static void changer_release(struct cdrom_device_info *cdi)
{
	int minor=MINOR(cdi->dev);
	DEBUG(LEVEL_FUNC,"changer_release(%02x)\n",minor);
	/* Close actual driver -> This is done on CHANGER_SET(NODEV). */
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		MOD_DEC_USE_COUNT;
		CHG.opencount--;
	}
}

/* Status() - Get drive status. */
static int changer_drive_status(struct cdrom_device_info *cdi,int slot_nr)
{
	int minor=MINOR(cdi->dev),result=-EIO;
	DEBUG(LEVEL_FUNC,"changer_drive_status(%02x:slot=%d)\n",minor,slot_nr);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->drive_status(changer_majors[MINOR(CHG.dev)].cdi,CHG.slot);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* CDS_NO_INFO | CDS_NO_DISC | CDS_TRAY_OPEN | CDS_DRIVE_NOT_READY | CDS_DISC_OK */
}

/* Media_Changed() - Check Media Changed Status */
static int changer_media_changed(struct cdrom_device_info *cdi,int disc_nr)
{
   int minor=MINOR(cdi->dev),result=0;
	DEBUG(LEVEL_FUNC,"changer_media_changed(%02x,disc=%d)\n",minor,disc_nr);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->media_changed(changer_majors[MINOR(CHG.dev)].cdi,CHG.slot);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
   return(result);		/* 0 = NOT Changed, 1 = Changed */
}

/* MoveTray() - Move the tray: open (position=0), or close (position=1). */
static int changer_tray_move(struct cdrom_device_info *cdi,int position)
{
	int minor=MINOR(cdi->dev),result=-EINVAL;
	DEBUG(LEVEL_FUNC,"changer_tray_move(%02x:position=%d)\n",minor,position);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->tray_move(changer_majors[MINOR(CHG.dev)].cdi,position);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* 0 = Ok its there now, 1 = Fail, it won't. */
}

/* LockDoor() - Lock the door: unlock (lock=0), or locked (lock=1). */
static int changer_lock_door(struct cdrom_device_info *cdi,int lock)
{
	int minor=MINOR(cdi->dev),result=-EINVAL;
	DEBUG(LEVEL_FUNC,"changer_lock_door(%02x:lock=%d)\n",minor,lock);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->lock_door(changer_majors[MINOR(CHG.dev)].cdi,lock);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* 0 = Ok it is, 1 = Fail, it won't. */
}

/* SelectSpeed() - Select the speed. */
static int changer_select_speed(struct cdrom_device_info *cdi,int speed)
{
	int minor=MINOR(cdi->dev),result=-EINVAL;
	DEBUG(LEVEL_FUNC,"changer_select_speed(%02x:speed=%d)\n",minor,speed);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->select_speed(changer_majors[MINOR(CHG.dev)].cdi,speed);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* >=0 = Ok, <0 = Failed. */
}

/* GetLastSession() - Get last session. */
static int changer_get_last_session(struct cdrom_device_info *cdi,struct cdrom_multisession *ms_info)
{
	int minor=MINOR(cdi->dev),result=-EINVAL;
	DEBUG(LEVEL_FUNC,"changer_get_last_session(%02x)\n",minor);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->get_last_session(changer_majors[MINOR(CHG.dev)].cdi,ms_info);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* 0 = Success, ??Else = Error?? <-- Not documented!! */
}

/* GetMCN() - Get Media Catalog Number (MCN) / Universal Product Code (UPN). */
static int changer_get_mcn(struct cdrom_device_info *cdi,struct cdrom_mcn *mcn)
{
	int minor=MINOR(cdi->dev),result=0;
	DEBUG(LEVEL_FUNC,"changer_get_mcn(%02x)\n",minor);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->get_mcn(changer_majors[MINOR(CHG.dev)].cdi,mcn);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);			/* ?? 0 = Fail, char *var[14]="MCNMCNMCNMCNM\0"; ?? <-- Not documented!!  */
}

/* Reset() - Reset. */
static int changer_reset(struct cdrom_device_info *cdi)
{
	int minor=MINOR(cdi->dev),result=-EINVAL;
	DEBUG(LEVEL_FUNC,"changer_reset(%02x)\n",minor);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->reset(changer_majors[MINOR(CHG.dev)].cdi);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* ?? <-- Not documented!! ?? */
}

/* AudioIOCTL() - Audio Calls. */
static int changer_audio_ioctl(struct cdrom_device_info *cdi,unsigned int cmd,void *arg)
{
	int minor=MINOR(cdi->dev),result=ENOSYS;
	DEBUG(LEVEL_FUNC,"changer_audio_ioctl(%02x:cmd=%x)\n",minor,cmd);
	if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
	{
		LOCK_DEVICE();
		changer_change_slot(minor);
		result=changer_majors[MINOR(CHG.dev)].cdi->ops->audio_ioctl(changer_majors[MINOR(CHG.dev)].cdi,cmd,arg);
		UNLOCK_DEVICE();
	}
	else result=-EINVAL;
	return(result);		/* 0 = Success, ENOSYS = Not implemented, other = error. */
}

/* DevIOCTL() - Device Calls. */
static int changer_dev_ioctl(struct cdrom_device_info *cdi,unsigned int cmd,unsigned long arg)
{
	int minor=MINOR(cdi->dev),result=-EINVAL;
	struct changer_set set;
	DEBUG(LEVEL_FUNC,"changer_dev_ioctl(%02x:cmd=%x)\n",minor,cmd);

	switch (cmd)
	{
		case CHANGER_SET:
								/* Let only root (or equivalent) do this function. */
								if (capable(CAP_SYS_ADMIN))
								{
									if (minor<CHANGER_MAX_MINOR)
									{
										if (copy_from_user(&set,(struct changer_set *)arg,sizeof(set))) result=-EFAULT;
										else result=changer_set(cdi,&set);
									}
									else result=-EINVAL;
								}
								else result=-EACCES;
								break;
		default:				if ((minor<CHANGER_MAX_MINOR)&&(CHG.dev!=NODEV))
								{
									LOCK_DEVICE();
									changer_change_slot(minor);
									result=changer_majors[MINOR(CHG.dev)].cdi->ops->dev_ioctl(changer_majors[MINOR(CHG.dev)].cdi,cmd,arg);
									UNLOCK_DEVICE();
								}
								else result=-EINVAL;
	}
	return(result);
}

/**********************************************************************************************************************
  Modules Stuff
**********************************************************************************************************************/
#ifdef MODULE
int init_module(void)
{
	DEBUG(LEVEL_FUNC,"init_module()\n");
	return(changer_init());
}

void cleanup_module(void)
{
	DEBUG(LEVEL_FUNC,"cleanup_module()\n");
	changer_cleanup();
	INFO(LEVEL_INFO,"Module freed... Have a pleasant day.\n");
}
#endif MODULE

