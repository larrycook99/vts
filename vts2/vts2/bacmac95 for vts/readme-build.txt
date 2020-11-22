to rebuild BACMAC95 you will need to install the WIN95DDK
then place files in these folders:

xxxx\win95ddk\src\net\bacmac95\VXD
	bacmac95.c
	bacmac95.def
	bacmac95.h
	bacmac95.inf
	bacmac95.rc
	lock.c
	ndisdev.asm
	ndispkt.c
	openclos.c
	read.c
	request.c
	write.c
	bacoid.h
	debug.h
	ndis.mk
	build.bat
	makefile

xxxx\win95ddk\src\net\bacmac95\inc
	ntddpack.h	get this from win95ddk
	ntddndis.h	get this from win95ddk
	devioctl.h	get this from win95ddk
	packet32.h
	packon.h
	packoff.h

