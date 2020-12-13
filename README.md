# llss
LLSS, or 'Link Layer Stable Switcher' is a networking tool for linux-enabled devices designed for stability and security. llss is a MTD (Moving Target Defense) that utilizes Hardware Address (MAC) randomization to provide a moving target. This greatly increases the difficulty of sniffing, and ARP cache poisoning. At the moment, this project is **optimized to work in an ad-hoc, wireless environment**

### Examples
**You are in a compromised area with people trying to intercept all traffic**
* The shuffling of the MAC addresses makes it considerably more difficult to create a man in the middle attack.
* Tracking data for a specific MAC address is useless.
* Filtering with wireshark is considerably more difficult, tracking a session between users will take considerable computational effort.
* If session tracking is completed, the data is still confidential (encrypted by AES).

## Install Instructions
Currently, llss requires the following
* gcc
* make
* libssl-dev
* libexplain-dev

Here is a helpful shell script to run all of those things
```
#!/bin/sh
sudo apt install gcc
sudo apt install make
sudo apt-get install libssl-dev
sudo apt-get install libexplain-dev #This will be unneccesary in the near future.
```
llss operates optimally in an ad-hoc environment, but connecting to an ad-hoc network varies depending on device. Please refer to [my ad-hoc notes](/notes/ad-hoc.md)

## Future Work
llss is not complete, and work on it will continue
#### Features Coming in future llss updates
:white_check_mark: IP address shuffling\
:white_check_mark: Port shuffling\
:white_check_mark: Instant install files\
