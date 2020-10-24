# llss
LLSS, or 'Link Layer Stable Switcher' is a networking service for linux-enabled devices designed for stability and security. It is designed to maintain connection and security between devices during the process of link layer switches. Additionally, LLSS has builtin address randomization at the packet level.This can work at both the link layer, and the IP layer (Both MAC and IP addresses can be shuffled). While it may functionally work in a wired environment. It is designed to operate best in a wireless environment with the losses and issues that come with it. At the moment, this project is **optimized to work in an ad-hoc environment**

### Examples
**You are in a compromised area with people trying to intercept all traffic**
* The shuffling of the MAC addresses makes it considerably more difficult to create a man in the middle attack.
* Tracking data for a specific MAC address is useless.
* Filtering with wireshark is considerably more difficult, tracking a session between users will take considerable computational effort.
* If session tracking is completed, the data is still confidential (encrypted by AES).
