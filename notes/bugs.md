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
Ssh breaking was bad, because my workflow was heavily optimized and I was using SSH to control all three devices at once.

## Mac Address setting + Random
TLDR (Fill in tomorrow)
Tried everything, link forum posts
SC's to discord post at 3am
Fuzzing to figure it out at the end
Infinite loop, fine, forbidden
What the hell is errno 52
solutions I tried
    asymettic mac use/generation.
    strace
    reading system kernel
    shutting down fd's when not in use
    https://forum.pine64.org/showthread.php?tid=10301
    removing modules
    restarting arp

## random effing me over (The big error)
    fuzzing to solve this
    was very strange
    eric shoutout
    https://serverfault.com/questions/40712/what-range-of-mac-addresses-can-i-safely-use-for-my-virtual-machines

## port error
1. recvfrom changing the fonky
2. Sending back causes a port mismatch
3. ICMP port unreachable.

## Debug error
1. tried commenting everything out
2. Commented line by line
3. Found isolated source of error
4. Was memory corruption
5. Zeroed out the ifreq struct to fix. I had already done this but idk why it didnt work.
