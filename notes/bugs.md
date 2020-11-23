# Bugs
I have run into some huge bugs during this process, I would like to note them as I run into them.

## DHCP
DHCP was the frist really huge issue, the second I got mac address shuffling working, **it 100% broke everything.**.
You see, the thing I didnt realize *at the time* is that DHCP is costly and very *timely*. The changes would propogate, but stuff would happen.
1. I would end up with a new IP. This is bad because I am trying to maintain the IP, but change the underlying address.
    Changing the IP would actually break the communication and I didnt want to be that funky. 
    I could maybe maybe do it later but thats not for now. Gives me a good idea for a app name. "Chaos Agent" which just shuffles EVERYTHING but maintains communication somehow.
2. The communication would break between the two hosts
3. *SSH would break*
