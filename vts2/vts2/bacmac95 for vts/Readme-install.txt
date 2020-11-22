Installation Procedure for BACMAC95
===================================
March 6, 2000

This procedure results in the installation of BACMAC95.VXD. BACMAC95 provides
the MAC packet driver and NDIS interface for NALSERV32 which in turn provides
BACnet protocol Client/Server capabilities up through the BACnet Network Layer
for VTS. The procedure also applies to Windows 98.

The installation procedure is:

1.  Make sure you have installed all the network adapters that you require
    (such as an NE5500) through the standard Windows 95 network adapter
    installation procedure. 
2.  Make sure you have installed the VTS components for Windows 95.
3.  From the Windows 95 Explorer or My Computer, select the Control Panel, or
    use Start|Settings|Control Panel, then in the "Control Panel" Window, 
    select Network.

    or right-click Network Neighborhood and click Properties.
4.  In the Configuration tab of the "Network" Window, click Add..
5.  Select "Protocol" in the "Select Network Component Type" Window and
    then click Add... 
    (Note: Your Adapter(s) should be listed in this window. If not, click 
    Cancel and return to step 1.)
6.  Click Have Disk ... in the "Select Network Protocol" Window.
7.  Browse the VTS Installation disk, or the folder on your hard drive
    where VTS was installed, until you find ...Drivers\BACMAC95\BACMAC95.INF
    Click OK in the "Install from Disk" Window.
8.  "BACdor95 NDIS MAC Driver" should now be selected in the 
    "Select Network Protocol" Window. 
    Click OK.
9.  You may get an error: The file bacndi.dll on (Unknown) could not be 
    found. If you do, simply click OK.
10. "BACdor95 NDIS MAC Driver -> <nothing>" should have been added to the 
    protocol list in the "Select Network Protocol" Window. You should also see 
    a "BACMAC95 Installer" Window with a list of possible network adapters for 
    you to bind BACMAC95 to. Select one and click Bind. You will get 
    confirmation or rejection in the BACMAC95 Installer Window. The Bind button 
    will disappear if the binding is successful.
11. Click OK in the "Network" Window. The appropriate files will be copied 
    to your hard disk and the Windows 95 registry will be modified. You may be 
    asked to insert a Windows 95 CD-ROM. You MUST follow ALL directions with 
    regard to the CD-ROM to insure proper installation.
12. You will be prompted to restart your PC after installation is complete. 
    You must do so in order to complete the installation and for BACMAC95 to be 
    recognized by Windows.

The removal procedure is:

1. From the Windows 95 Explorer or My Computer, select the Control Panel.
2. In the "Control Panel" Window, select Network.
3. In the Configuration tab of the "Network" window select "BACdor95 NDIS 
   MAC Driver -> <..>" and click Remove.
4. BACMAC95 will be removed from your system and the registry. Follow all 
   instructions from the Windows 95 Network Installer.

