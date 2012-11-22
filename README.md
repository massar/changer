/dev/changer
============

Hi,

Yet another version, fixing some patching problems with Linux Kernel 2.2.14, currently diffed against 2.3.38
Memset is still a pain in the special dark place... but it works...
See the source code and the website for the rest of the documentation available.
For comments etc, send them to: jeroen@massar.ch or check the website at http://unfix.org/projects/changer

Greets,
  Jeroen Massar

PS-for-nosy-people:
 My linux development&server&router box is called heaven (FQDN:heaven.unfix.org).
 My NT X-Workstation (running only ReflectionX most of the time) is called hell (FQDN:hell.unfix.org).
 The names have been chosen for the obvious reasons.

Legal Mumbo Jumbo
=================
The code is GPL'ed as attached into the source code.

Description
===========
/dev/changer will allow you to mount all the cd's in your IDE changer(s) at once.
thus avoiding the hassle with scripts or other custom software.

Status
======
The code for /dev/changer can now be considered to be at a stable level.

Currently we only have the following problems:
* It is a changer so you technically CAN'T access two slots at the same time.
  Thus whenever you spool two movies, one from slot 0 and the other from slot 3
  the /dev/changer code will keep swapping between them every request. It'll
  get very slow because of this.

  I'll work on something like a timer or alike (see the source-code) which will
  fix this by simply only allowing a slotchange every n seconds and thus effectivly
  allowing datatransfer from/to that slot for that time.

* CD's aren't locked whenever I mount them.

See the source code and the website for the rest of the documentation available.

Using/Installing
================
You'll have to apply the patch to your kernel, then enable either the module or include the driver
into the kernel. eg:

    root@heaven:/usr/src/local/linux-2.2.4# patch -p1 <changer_19990429.diff
    root@heaven:/usr/src/local/linux-2.2.4# make xconfig
    root@heaven:/usr/src/local/linux-2.2.4# make-kpkg --revision=42:heaven.2.2.6 binary
    root@heaven:/usr/src/local/linux-2.2.4# cd ..
    root@heaven:/usr/src/local/# dpkg -i *.deb

    line 1: patch the kernel
    line 2: configure the kernel (select either CHANGER=module or CHANGER=yes)
    line 3: make debian kernel packages, non-debian users could use:

            root@machine:/usr/src/local/linux-2.2.4# make dep bzLilo modules modules_install
    line 5: install the new kernel

And don't forget to modify your lilo where required...
then reboot the newly installed kernel, see your favourite kernel HOWTO...

If you've created a module you'll have to insmod it
eg:
    alias block-major-42 changer

then you should create enough devices for the changer with:

    root@heaven:/dev# for i in `seq 255`; do mknod /dev/changer$i b 42 $i; mkdir /cdrom/$i; chown root:cdrom /dev/changer$i /cdrom/$i; done

after you've done that you'll have to mount the slots, just like an ordinary cdrom, eg:

    root@heaven:# mount /dev/changer4 /cdrom/4 -t iso9660

you can ofcourse put these into the /etc/fstab.

If you still got any questions do a: `vi question; cat question | mail jeroen@massar.ch`

