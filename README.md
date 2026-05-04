# kindle-input 
## A Tool to use your Kindle Scribe as a graphics tablet! (for now Linux only)
This tool is based on 2 app, the desktop server and the kindle client, and you need both to run this.
### Installation guide
1. Download the zip from [release](https://github.com/Ghibbi64/kindle-input/releases).
2. Extract the folder "kindle-input-desktop" wherever you want.
3. Open the storage of your kindle and copy the folder "extensions" in the root (the real folder path is /mnt/us).
Done
### How to use it
**! Be sure to not have any other serial-tty device connected to your pc !**<br>
First take a good look at the config file of the desktop app.<br>
Then open the app on the kindle trough KUAL, and select "Enable tablet mode", now you can connect the usb to your pc and open the desktop app in your terminal.<br>
After finishing use CTRL+C to close the app.
#### Little troubleshooting - known problems
- If the desktop app doesn't see the kindle, just press again on "Enable tablet mode".
- If you disconnect the USB while the program is running, you need to press again "Enable tablet mode" to turn on the data stream again, if you want to continue using it. 
