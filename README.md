XD-Input
=========

Created by [Nicholas Stephen](http://www.nickstephen.com/).
Based on work by [Scarlet.Crush](http://forums.pcsx2.net/Thread-XInput-Wrapper-for-DS3-and-Play-com-USB-Dual-DS2-Controller).

Contents (use find with the <?> bit):
--------------------
0. <0>LINKS
1. <1>INSTALLATION
2. <2>OPERATION
3. <3>CONFIG
4. <4>SUPPORT


<0>LINKS
------------
My site: [www.nickstephen.com](http://www.nickstephen.com/)
This project page: [www.nickstephen.com/software/xdinput/](http://www.nickstephen.com/software/xdinput/)
Contact me: [www.nickstephen.com/contact.php](http://www.nickstephen.com/contact.php)
Source code: [GitHub](https://github.com/saltisgood/xd-input)


<1>INSTALLATION:
----------------
Scarlet.Crush's Driver Package is required for XD-Input to work. This can be obtained from [here](http://forums.pcsx2.net/Thread-XInput-Wrapper-for-DS3-and-Play-com-USB-Dual-DS2-Controller).
[VC 2013 x86 Redistributable](http://www.microsoft.com/en-au/download/details.aspx?id=40784) is also required.
XD-Input itself does not require installation.


<2>OPERATION:
-----------------
1. Plug in your USB controller.
2. Start XD-Input.
3. Go through the list of Human Interface Devices (HIDs) by pressing 'n' or 'y' to check which is your controller.
4. By default, XD-Input will then automatically go into translation mode. Converting your controller's inputs into the virtual device's inputs. Press CTRL + HOME to re-read button mappings from keys.config (see below). Press CTRL + END to stop translation and quit the program.
	
If there are a list of processes in the PROC_LIST setting (see below) then this will happen:
4.	XD-Input will wait until one of the processes listed in PROC_LIST starts up. Pressing CTRL + SHIFT + END stops waiting and exits.
5.	When a process has been detected as starting, XD-Input reads in the button mappings for that process from (ExampleProcess.exe.config) and
	starts translating.
6.	When the process has been detected as ending, XD-Input will quit translation.

If option QUIT_ON_GAME_CLOSE is true:
7.	XD-Input will then exit.

Otherwise:
7.	XD-Input will go back to step 4.


<3>CONFIG
-------------
Configuration of XD-Input happens through the *.config files in the program's working directory (i.e. where it is). So any file in there that ends in .config can be edited. They're all just plain-text files, so don't worry about encoding or whatever. The formats are as follows:

	settings.config:
FAVDEV_VENDORID=121
FAVDEV_PRODUCTID=6
PROCESS_LIST=DarkSoulsII.exe;
QUIT_ON_GAME_CLOSE=TRUE

FAVDEV_VENDORID is the vendor ID of the saved favourite device (i.e. your controller). Same for product ID. PROCESS_LIST is a semi-colon delimited list of process names (including file extensions) for the program to wait for and hook up with (platonically). If you want XD-Input to just continue translating your controller without respect to individual game processes, just remove the process names and the semi-colon. QUIT_ON_GAME_CLOSE chooses whether the quit the program when the game process ends. If there is no process list, this setting has no effect.

	keys.config, *.exe.config:
BUTTON_Y=BUTTON_A
BUTTON_B=BUTTON_B
BUTTON_A=BUTTON_X
BUTTON_X=BUTTON_Y
BUTTON_LB=BUTTON_LB
BUTTON_RB=BUTTON_RB
BUTTON_LT=BUTTON_LT
BUTTON_RT=BUTTON_RT
BUTTON_BACK=BUTTON_BACK
BUTTON_START=BUTTON_START
BUTTON_LS=BUTTON_LS
BUTTON_RS=BUTTON_RS

This is the list of supported buttons on the controller using the XBox 360 button nomenclature. Just change the position of the buttons on the right hand side to change the button mappings from your controller. Having buttons that map to multiple other buttons is undefined, so I suggest not doing that. The keys.config file is the generic button mapping. This is used when not using a process list. When using process lists, the config files get generated for each process, so that file is named Process.exe.config. 


<4>LINKS
------------------
If there are any problems/questions/suggestions hit me up at [my contact page](www.nickstephen.com/contact.php) but have a go at reading [the online guide](www.nickstephen.com/software/xdinput/) first.

Cheerio!