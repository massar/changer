changer
=======

Friday, 19 March 1999

Hi,

Well this is one of the very first, almost working beta alpha's of /dev/changer.
It works but don't stress it to much.
I tried to play an mp3 from one cd and cat another cd and my filesystems simply
deadlocked... so notez bien: you're really playing with alpha software...
I'll try and fix this asap, seeing that I've only worked on this for about 9 nights
which is approx. 9*6 hours it seems to work pretty well already...

See the source code and the website for the rest of the documentation available.

Description
~~~~~~~~~~~
/dev/changer will allow you to mount all the cd's in your changer(s) at once.
thus avoiding the hassle with scripts or other custom software. 

Compiling & Using
~~~~~~~~~~~~~~~~~
jeroen@heaven.unfix.org:/raid/chg$ make clean
jeroen@heaven.unfix.org:/raid/chg$ make
root@heaven.unfix.org:/raid/chg# for i in `seq 255`; do mknod /dev/changer$i b 42 $i; mkdir -p /cdrom/$i done
root@heaven.unfix.org:/raid/chg# mount /dev/changer1 /cdrom/1 -t iso9660

For comments etc, send them to: jeroen@massar.ch or check the website at http://unfix.org/projects/changer

Greets,
 Jeroen Massar
 jeroen@massar.ch

