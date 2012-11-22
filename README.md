changer
=======

/dev/changer - Linux Kernel CD Disk Changer support16 March 1999

Hi,

Well this is the very first, almost working beta alpha gamma of /dev/changer.
It shouldn't even compile for you... cause you're missing some stuff in the
kernel includes which manually editted but haven't included patches for yet.

See the source code for all the rest of the documentation available.

What:
/dev/changer will allow you to mount all the cd's in your changer(s) at once.
thus avoiding the hassle with scripts etc. And effectivly allowing a cheap way
to expensive jukeboxes along with their software...

A few things you should know:
root@heaven.unfix.org # for i in `seq 255`; do mknod /dev/changer$i b 42 $i; mkdir -p /cdrom/$i done
and then:
root@heaven.unfix.org # mount /dev/changer1 /cdrom/1 -t iso9660

For any comments etc, send it to: jmassar@unfix.org or check the website at http://unfix.org/projects/changer

Greets,
 Jeroen Massar

http://unfix.org - jmassar@unfix.org

