# Workflow
In these months, I have optimized the hell out of my workflow for this project. That has included the following
* Extensive bash profiles
* Tmux scripting
* Ssh configurating
* Hardware solutions
* Virtual desktops
* Screaming

## Extensive bash profiles
The first time I got the pi's, I loaded Ubuntu server 20.04 onto them (See hubris.md). I've been writing my own bash profiles since 2018 ever since I got into sigpwny and figured out how to use them. Since then I have gotten way better at it. I dont really organize that much (I used to, my CS225/233 workflow was really optimized), since then I really just add and alias as I need it. Back when I had the time, it was "hmm, I might need this in the future, I might as well add it now."

Some of my favorites are
```
alias c='clear'
#Clear the screen, I know cntrl+l exists I dont care

alias cm='clear; make'
#Clear and then make your code

alias cmd='clear; make debug;
#Clear and then make the debug version of my code (always have two seperate versions)

alias g='git add -u; git status; git commit -m "Automatic, see changes"; git push"
#Quick auto git

alias b='vim ~/.bashrc; source ~/.bashrc'
#Edit your bashrc, replace this with the title of your desired profile

alias whereami='ifconfig | grep "inet " | grep -v 127.0.0.1 | cut -d\ -f2'
#Get what your IP is

alias remake='make clean; make' 
#Quick Remake tool

alias fsv='whereami; python3 -m http.server 1234' #This is literally the best way to quickly share files between linux enabled devices IMO
```

Additionally, I have written a cheeky little define function for debugging in c, super convenient to just turn off all print statements and allow them to compile out given good optimization. I actually invented this during CS241 a whole year ago and have been using it in C since.
```
#define \_\_DEBUG\_\_ 0 (or 1)
#define dbprintf if(\_\_DEBUG\_\_)printf
```
**Workflow++ Rating:** 8/10

## Tmux scripting
In october I learned how to do some badass tmux scripting. From my macbook, I can open all the raspberry pi's that I care about via SSH. I wrote some cool commands to quickly type the exact same content into all 3 windows, I even wrote commands on top of those commands to do pretty much whatever I wanted. Want to pull from all 4 devices, the command is `439pull`. Want to clear all the screens, `439c`, it was really easy once i got the hang of it and improved my workflow alot. Definitely worth doing

**Workflow++ Rating:** 9/10

## Ssh configuration
I mean this is just so i didnt have to type the same password in over and over again, definitely worth doing to save you time. Btw all you have to do is `fsv ~/.ssh` and then `wget your.ip.goes.here:1234/id_rsa.pub >> ~/.ssh/authorized_keys` *I WROTE THAT OFF THE TOP OF MY HEAD DONT TRUST ME*. Definitely useful but not critical.

**Workflow++ Rating:** 6/10

## Hardware solutions (purchasing)
I spent a crazy amount of money on this. Like over $300. **I WANTED TO DO THAT ANYWAY**, I was looking (and had been saving up for) for an excuse to blow money on cool tech-y related stuff I needed/wanted (ethernet switches, kvm's, hdmi switches/splitters, raspberry pis, etc.), and this was finally a good excuse to get it all. I now am the proud owner of the following lol.
* An undisclosed number of Raspberry pis
* 6 Tiny (1.5 ft) ethernet cables (to connect to the switch)
* Raspberry pi on-off switches, and other stuff that came with the Raspi kit (it was honestly pretty cool).
* USB KVM
* HDMI Switches (1 for the 3 pis, and then 1 to switch between pi output and pc output.)
* Usb cables (many)
* 2 Amazon Mice
* 2 Nintendo themed Pi cases
* 1 clear raspi case

Canakit took a long time to get my stuff to me, but that was ok, Prof Kravets had a pi3 for me to use. Having all this though super duper improved my workflow, instead of having to plug in and unplug things, I just had to push 2 buttons and it was all up and running.

**Workflow++ Rating:** 10/10

## Virtual desktops
HOLY HECK FUN FACT WINDOWS HAS VIRTUAL DESKTOPS. You can get them by pressing WIN + Tab, and they are so helpful. I have 4 for 2 monitors for a total of 8 desktops. 1 for school, 1 for off-topic (gaming), 1 for work, and 1 for sigpwny. I find myself on the "off-topic" one alot, maybe having a dedicated off topic thing is a bad call, further research required... Regardless, this massively sped up my googling capabilities as I could throw distractions onto another, more relevant virtual desktop. Would reccomend

**Workflow++ Rating:** 10/10

## Screaming
Screaming into the void helps sometimes. I have my own personal discord server where I can send myself files & links etc. I have friends to bounce ideas off of.

The one thing I fear is that I am too intense. I know it will do me well to be a powerful presence, but I dont want to be annoying... anyway...


# Final thoughts
I'll update this as time goes, workflow is important to me, especially when it is good.