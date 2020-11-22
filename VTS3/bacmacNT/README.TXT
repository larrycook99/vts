Installation Procedure for BACMACNT
===================================
March 6, 2000

This procedure results in the installation of BACMACNT.SYS. BACMACNT provides
the MAC packet driver and NDIS interface for NALSERV32 which in turn provides
BACnet protocol Client/Server capabilities up through the BACnet Network Layer
for VTS.

The installation procedure is:

1.  Make sure you have installed all the network adapters that you require (such as an NE5500)
    through the standard Windows 95 network adapter installation procedure. 
2.  Make sure you have installed the VTS components for Windows NT.
3.  Copy BACMACNT.SYS to your c:\WinNT\System32\Drivers folder.
4.  Execute the program BACREGNT.EXE using Start|Run or by double-clcking from
    Explorer or My Computer. Select all adapters that you wish to bind BACMACNT to
    then click OK.
5.  You must reboot your PC for the bindings to take effect.

The removal procedure is:

1.  Execute the program BACREGNT.EXE using Start|Run or by double-clcking from
    Explorer or My Computer.
 
2.  When the BACMAC Registration Dialog appears, select the adapter you wish 
    to unbind, click the "Unbind" button and then click "Exit".

Note: BACMACNT and its bindings will NOT appear in the normal NT Network Control Panel.