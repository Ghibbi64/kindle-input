# kindle-input 
## super-ultra-ALPHA (only CLI for now, more like a proof of concept, but i'm making a ui for both the kindle and pc)
A tool that lets you use your Kindle Scribe as a graphics tablet.<br>
**For now only tested on Scribe 2022.**

### Little guide
For now this only works on linux, sorry windows and mac users (i will add support for them in the future)<br>
Make sure to have [usbnetLite](github.com/notmarek/kindle-usbnetlite) installed on your kindle, and i suggest using the usb ssh instead of wifi because of latency and stutter.
Once connected the kindle to the computer you can edit the config file to your likings and then just run the program!

#### Warning for now
Even if you are on wifi you should edit the "ip_address_usb" in the config to match the wifi ip address (this is just temporary i'm focusing more on the kindle app right now).

#### Little troubleshooting
For some reason the virtual ethernet made by usbnetLite gives my a lot of routing error, if this is the case for you in the repo you can find a script to fix this (hopefully) in /src/platform/linux/networkFix that you need to run while the kindle is connected (remove any other usb ethernet device if there are).
