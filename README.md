# SpaceMouse

This is an alternate software to run on the Space Mouse With Arduino Micro as described here:
https://www.instructables.com/Space-Mouse-With-Arduino-Micro-Fully-Printable/ by Stefano Sbarra.

In this Instructable project, Stefano's original software was mainly targeting Inventor and partially Fusion 360 if set in Inventor mode.
Besides this I also wanted some more features compared to what Stefano was proposing, hence I started to modify its original source code to fully fit with Fusion 360 and **my personal needs**.

**With this software the Space Mouse has the following modes available:**
* normal: moves the mouse pointer as with a normal mouse
* pan: Fusion 360 pan mode (moves by simulating holding the middle mouse button)
* rotate: Fusion 360 rotate mode (rotates part by simulating holding SHIFT key + middle mouse button)
* zoom: Fusion 360 zoom mode (zooms in/out by simulating rolling the middle mouse button)

**The 3 space mouse buttons have the following functions:**
* button 1 (Left): simulates a left mouse button
* button 2 (Middle): toggles between default, pan and rotate modes
* button 3 (Right): simulates the ESC key

**Joystick switch functions:**
- short press: simulates the F6 key (fits to zoom)
- long press: activates the zoom mode.  
  To zoom in: while keeping the joystick pressed, push the joystick.  
  To zoom out: while keeping the joystick pressed, pull the joystick.  
  When the joystick switch is released, the space mouse switches back to the previous mode that was active (normal, pan, rotate).
 
Enjoy and feel free to mofify this software to suit **your** needs !

## Note:
The files in this repository are meant to be used with Platformio.
For those using just the regular Arduino IDE, you just have to download the file main.cpp in the src folder, to rename it let say to `spacemouse.ino` and open `spacemouse.ino` with the Arduino IDE.
Make sure then that you select the `Arduino pro micro` board and that the Arduino `Mouse` and `Keyboard` libraries are installed in the Arduino IDE.
