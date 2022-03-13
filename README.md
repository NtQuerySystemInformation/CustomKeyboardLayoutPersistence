# Keyboard Layout Persistence PoC:
	![imagen](https://user-images.githubusercontent.com/87275762/158073012-e3de0a6a-7b33-4e23-8c42-87148a2a71b9.png)
	
	Achieve execution using a custom keyboard layout, tested in Windows 11 Home version 21H2 

# How to use:
The dropper requires VS2022 for compilation and the keyboard driver VS2019 because of WDK (until VS2022 supports this.)

	If you want to compile your own keyboard driver:
		1.-Compile the keyboard driver and the dropper (KeyboardLayoutPersistence) as Release x64.
		2.-Replace the keyboard driver dll inside KeyboardLayoutPersistence folder. 
		3.-Execute the dropper and then reboot. 
	
	If you just want to compile and test (precompiled dll)
		1.-Compile the dropper as Release x64 (KeyboardLayoutPersistence).
		2.-Execute and then reboot.
    
   Once the infected user logs in, you should see a cmd.exe process that is NTAUTHORITY/SYSTEM.
   
# Warnings:
  It is recommended to run the binary in a VM, since there is no code related to the desinstallation process in the PoC.
  For more details refer to the paper.
  
# Dependencies:
   Requires Microsoft Visual C++ Redistributable (Visual Studio 2015, 2017, 2019 y 2022)

Dedicated to all vx-underground members.

