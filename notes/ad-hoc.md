# Creating an Ad-Hoc Network On Raspberry pi (raspbian)
I will refine this later so you can have a virtual network, that way you dont need the ugly ethernet setup.

## Requirements
You need the following to do this If you want to have the pi's connected to the internet. You can use SSH over the ad-hoc network (which was suprising).
1. An ethernet cable for every raspberry pi you have
2. An n + 2 port ethernet switch (for n pi's, 1 to your laptop, 1 to the router/internet)
3. A laptop/desktop hub station for control (You can also use one of your Pi's for the control node)

## Setup
Follow these steps
1. Edit `/etc/network/interfaces` as root
2. Add the following into interfaces at the bottom of the file
	```
	auto lo
	iface lo inet loopback

	iface eth0 inet manual

	auto wlan0
	iface wlan0 inet static
		address 192.168.X.X #Replace these X's with whatever you want
		netmask 255.255.255.0
		wireless-channel 1
		wireless-essid 439AdHoc
		wireless-mode ad-hoc
	```
3. restart your device. `sudo shutdown -r now` works just fine.
This will disable eth0 (for some really weird reason).
You are now on an ad-hoc network. But as mentioned above, the device is now acting strange on startup. The next two commands fix that
4. run `sudo crontab -e`, add the command `ip link set <eth0> up`, replace `eth0` with whatever your ethernet port
5. edit `/etc/rc.local`, insert `dhclient -v eth0`

This will re-enable eth0 so you can connect via eth

If you are using a git repo and want to pull, you have two options.

1. Pull from the internet (Requires some ethernet connection)
2. Host a local gitserver (I havent gotten this to work yet, but google `git serve`)
