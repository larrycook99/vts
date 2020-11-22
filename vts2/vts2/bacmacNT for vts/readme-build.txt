to rebuild BACMACNT you will need to install the NT40DDK
then place files in these folders:

xxxx\nt40ddk\src\network\bacmacnt\driver
	bacmacnt.c
	bacmacnt.h
	bacmacnt.rc
	openclos.c
	read.c
	write.c
	bacoid.h
	debug.h
	polarver.h
	resource.h
	sources.dsp
	sources
	makefile

xxxx\nt40ddk\src\network\bacmacnt\driver\obj
	_objects.mac

xxxx\nt40ddk\src\network\bacmacnt\inc
	ntddpack.h	get this from nt40ddk
	packet32.h

