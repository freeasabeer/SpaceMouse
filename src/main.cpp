//Author Olivier Perron
//March 2021
//FREE FOR ANY NON COMMERCIAL PROJECT
//Original source: https://www.instructables.com/Space-Mouse-With-Arduino-Micro-Fully-Printable/ from Stefano Sbarra, April 2020

//THIS SKETCH MAKES ARDUINO LEONARDO (OR MICRO) BASED ON ATmega32U4 WORK AS A SPACE 3D MOUSE FOR FUSION 360
//NORMAL MODE : JOYSTICK MOVE ACT AS ... A NORMAL MOUSE :)
//PAN MODE VIEW : PAN BY HOLDING MIDDLE MOUSE BUTTON
//ROTATE MODE VIEW : ROTATION BY HOLDING SHIFT KEY + MIDDLE MOUSE BUTTON
//PRESS AND MOVE ACT AS VIEW ZOOM BY ROLLING MIDDLE MOUSE BUTTON (WHATEVER THE CURRENT MODE IS)
//QUICK PRESS OF THE JOYSTICK ZOOMS TO FIT USING F6
//ADDITIONAL FUNCTIONS AND SWITCHES ARE EASY TO IMPLEMENT
// BUTTON1 ACTS AS MOUSE LEFT BUTTON
// BUTTON2 TOGGLES SPACE MOUSE MODE (NORMAL, PAN, ROTATE)
// BUTTON3 SENDS ESC_KEY
#include <Arduino.h>
#include <Mouse.h>
#include <Keyboard.h>


// HARDWARE SETUP
#define horzPin  A0                // Analog output of horizontal joystick pin
#define vertPin  A1                // Analog output of vertical joystick pin
#define selPin   10                // select button pin of joystick
#define b1Pin     7                // button 1
#define b2Pin     8                // button 2
#define b3Pin     9                // button 3

// SOFTWARE SETUP
typedef enum {NORMAL, PAN, ROTATE, ZOOM} spacemouse_mode_t;
//#define DEBUG                    // Uncomment to activate debug output to serial console
#define START_MODE       NORMAL    // Space Mouse mode to use at start-up
#define INVERT_MOUSE          0    // 0: Do not invert mouse, 1: Invert mouse
#define LONG_PRESS_INTERVAL 500    // interval to define if short or long press
#define MOVE_THRESHOLD        1    // Use of 1 instead of 0 cancels the small fluctuation between joystick position readings. If you experiment instabilities, try to increase to 2
const int sensitivity [4]    = {/*NORMAL*/ 75, /*PAN*/ 75, /*ROTATE*/150, /*ZOOM*/150}; //Higher sensitivity value = slower mouse, should be <= about 500
const int move_slow_down [4] = {/*NORMAL*/  5, /*PAN*/  5, /*ROTATE*/ 10, /*ZOOM*/ 40}; //this slows down the moves

// GLOBALS
int vertZero, horzZero;            // Stores the initial value of each axis, usually around 512


void setup()
{
  pinMode(horzPin, INPUT);         // Set both analog pins as inputs
  pinMode(vertPin, INPUT);
  pinMode(selPin, INPUT);          // set button select pin as input
  pinMode(b1Pin, INPUT);           // set button 1 pin as input
  pinMode(b2Pin, INPUT);           // set button 2 pin as input
  pinMode(b3Pin, INPUT);           // set button 3 pin as input
  digitalWrite(selPin, HIGH);      // Pull button select pin high
  digitalWrite(b1Pin, HIGH);       // Pull button 1 pin high
  digitalWrite(b2Pin, HIGH);       // Pull button 2 pin high
  digitalWrite(b3Pin, HIGH);       // Pull button 3 pin high
  delay(1000);                     // short delay to let outputs settle
  vertZero = analogRead(vertPin);  // get the initial values
  horzZero = analogRead(horzPin);  // Joystick should be in neutral position when reading these

  Mouse.begin();                   // Init mouse emulation
  Keyboard.begin();                // Init keyboard emulation
#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop()
{
  static spacemouse_mode_t default_mode = START_MODE; // selects mouse default mode: normal, pan, zoom or rotate
  static spacemouse_mode_t mode         = START_MODE;
  static int moved = 0;            // checks if the joystick has moved
  static int mouseClickFlag = 0;   // another click check
  static int b1flag = 0;           // click check
  static int b2flag = 0;           // click check
  static int b3flag = 0;           // click check
  static int b2toggle = 0;         // b2 toggle count
  static unsigned long presstime;  // used to register the press time
  unsigned long releasetime;       // used to register the release time (to select home or zoom)
  unsigned long diff;              // difference between time checks to read short or long press
  int vertValue, horzValue;        // Stores current analog output of each axis
  int invertMouse;
  unsigned long currentMillis = millis();


  vertValue = analogRead(vertPin) - vertZero;  // read vertical offset
  horzValue = analogRead(horzPin) - horzZero;  // read horizontal offset
  if ( (vertValue <= MOVE_THRESHOLD)&&(vertValue >= (-MOVE_THRESHOLD)) )
    vertValue = 0;
  if ( (horzValue <= MOVE_THRESHOLD)&&(horzValue >= (-MOVE_THRESHOLD)) )
    horzValue = 0;

#ifdef DEBUG
  Serial.print("B1: ");
  Serial.println(digitalRead(b1Pin));
  Serial.print("B2: ");
  Serial.println(digitalRead(b2Pin));
  Serial.print("B3: ");
  Serial.println(digitalRead(b3Pin));
#endif

//BUTTON 1

  if ((digitalRead(b1Pin) == 0) && (!b1flag)) // if button 1 is pressed
  {
    b1flag = 1;
    Mouse.press(MOUSE_LEFT);
  }
  else if (digitalRead(b1Pin) && (b1flag))    // if button 1 is released
  {
    b1flag = 0;
    Mouse.release(MOUSE_LEFT);
  }


//BUTTON 2

  if ((digitalRead(b2Pin) == 0) && (!b2flag)) // if button 2 is pressed
  {
    b2flag = 1;
    b2toggle++;
    b2toggle%=3;
    default_mode = (spacemouse_mode_t)b2toggle;
    mode = default_mode;
  }
  else if (digitalRead(b2Pin) && (b2flag))    // if button 2 is released
  {
    b2flag = 0;
  }


//BUTTON 3

  if ((digitalRead(b3Pin) == 0) && (!b3flag)) // if button 3 is pressed
  {
    b3flag = 1;
    Keyboard.write(KEY_ESC); // sends ESC key
  }
  else if (digitalRead(b3Pin) && (b3flag))    // if button 3 is released
  {
    b3flag = 0;
  }


//JOYSTICK CLICK

  if ( (digitalRead(selPin) == 0) && (!mouseClickFlag) )  // if the joystick button is pressed
  {
    presstime = currentMillis;
    mouseClickFlag = 1;
    mode = ZOOM;
  }
  else if ( (digitalRead(selPin)) && (mouseClickFlag) ) // if the joystick button is released
  {
    releasetime = currentMillis;
    mouseClickFlag = 0;
    mode = default_mode;

    diff = (releasetime - presstime);  // check if the press was short or long

    if(diff < LONG_PRESS_INTERVAL)     // if short press
    {
      Keyboard.write(KEY_F6); // Fusion 360: zoom to fit
    }
  }
#ifdef DEBUG
  Serial.print("Default mode: ");
  Serial.println(default_mode);
  Serial.print("Mode: ");
  Serial.println(mode);
  Serial.println(" ");
#endif

  switch(mode) {   // normal, pan, rotate or zoom

    case NORMAL:

      if ( (horzValue != 0) || (vertValue != 0) ) {
        invertMouse = -(1 - 2 * INVERT_MOUSE); // restore mouse orientation
        Mouse.move((invertMouse * (horzValue / sensitivity[NORMAL])), (invertMouse * (vertValue / sensitivity[NORMAL])), 0); // move mouse
      }
      break;

    case PAN:

      if ( (horzValue != 0) || (vertValue != 0) ) {
        invertMouse = -(1 - 2 * INVERT_MOUSE); // restore mouse orientation
        Mouse.press(MOUSE_MIDDLE);
        Mouse.move((invertMouse * (horzValue / sensitivity[PAN])), (invertMouse * (vertValue / sensitivity[PAN])), 0); // move mouse with middle button pressed to pan view
        moved = 1;
      }
      else {  // joystick is in neutral position: release all and stroke ESC to help go back to design view
        if (moved == 1) {
          Mouse.release(MOUSE_MIDDLE);
          //Keyboard.write(KEY_ESC);
          moved = 0;
        }
      }
      break;

    case ROTATE:

      if ( (horzValue != 0) || (vertValue != 0) ) {
        invertMouse = -(1 - 2 * INVERT_MOUSE); //restore mouse orientation
        Keyboard.press(KEY_LEFT_SHIFT); //rotate view (fusion 360 mode)
        Mouse.press(MOUSE_MIDDLE);
        Mouse.move((invertMouse * (horzValue / sensitivity[NORMAL])), (invertMouse * (vertValue / sensitivity[NORMAL])), 0); // move mouse with f4 key pressed to rotate view
        moved = 1;
      }
      else {  // joystick is in neutral position: release all and stroke ESC to help go back to design view
        if (moved == 1) {
          Keyboard.releaseAll();
          Mouse.release(MOUSE_MIDDLE);
          //Keyboard.write(KEY_ESC);
          moved = 0;
        }
      }
      break;

    case ZOOM:

      if ( vertValue != 0 ) {
        invertMouse = (1 - 2 * INVERT_MOUSE);  //mouse inverted to push for zoom in and pull for zoom out
        Mouse.move(0, 0, (invertMouse * (vertValue / sensitivity[ZOOM]))); // move mouse on y axis with scroll wheel to zoom view
        moved=1;
      }

      if ( (horzValue == 0) && (vertValue == 0) ) { // joystick is in neutral position
          if (moved == 1) {
            //Nothing to do
            moved = 0;
          }
      }
      break;

    }

  delay(move_slow_down[mode]); //this slows down the moves
}