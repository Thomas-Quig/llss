# llss
LLSS, or 'Link Layer Stable Switcher' is a networking service for linux-enabled devices designed for stability and security. It is designed to maintain connection and security between devices during the process of link layer switches. Additionally, LLSS has builtin address randomization at the session level.This can work at both the link layer, and the IP layer (Both MAC and IP addresses can be shuffled). While it may functionally work in a wireless environment. It is designed to operate best in a wireless environment with the losses and issues that come with it.

### Examples
**Your device changes MAC addresses for some reason.**
* Your youtube video sees minimal downtime
* Your ssh connectoin doesnt time out
* Your webserver stays up at the same IP
* Any video game you are playing allows you to continue playing online with minimal interruption.

**You enter a starbucks and automatically connect to the wifi**
* Your hardware address changes every so often, making it more difficult for Starbucks to track you
* Passive sniffer tools will have a (slightly) harder time tracking you.
* An attacker trying to spoof your mac address for an MITM will lose that ability because your MAC is shuffled (its still possible, just more diffucult)