//Author Stefano Sbarra
//April 2020
//FREE FOR ANY NON COMMERCIAL PROJECT
//Original source: https://www.instructables.com/Space-Mouse-With-Arduino-Micro-Fully-Printable/

//THIS SKETCH MAKES ARDUINO LEONARDO (OR MICRO) WORK AS A SPACE 3D MOUSE FOR INVENTOR AND FUSION 360
//NORMAL JOYSTICK MOVE ACT AS VIEW ROTATION USING F4 KEY
//PRESS AND MOVE ACT AS VIEW ZOOM USING F3 KEY
//QUICK PRESS OF THE JOYSTICK GOES TO HOME VIEW USING F6
//ADDITIONAL FUNCTIONS AND SWITCHES ARE EASY TO IMPLEMENT (EXAMPLE SLICE VIEW)
#include <Arduino.h>
#include <Mouse.h>
#include <Keyboard.h>

typedef enum {ROTATE, ZOOM, NORMAL} spacemouse_mode_t;

//#define DEBUG
#define INVERT_MOUSE 1           // Set to -1 to invert
#define LONG_PRESS_INTERVAL 500  //interval to establish if quick or long press
#define MOVE_SLOW_DOWN       5   //this slows down the moves
const int sensitivity [3] = {/*ROTATE*/ 150, /*ZOOM*/150, /*NORMAL*/75}; //Higher sensitivity value = slower mouse, should be <= about 500
#define MOVE_THRESHOLD 2         // Use of 2 instead of 0 cancels the small fluctuation between joystick position readings


//moved and esc key helps to go back from view mode to design

int horzPin = A0;  // Analog output of horizontal joystick pin
int vertPin = A1;  // Analog output of vertical joystick pin
int selPin  = 10;  // select button pin of joystick
int b1Pin   =  7;  // button 1
int b2Pin   =  8;  // button 2
int b3Pin   =  9;  // button 3

spacemouse_mode_t default_mode = NORMAL; //selects mouse mode: normal, zoom or rotate by default
spacemouse_mode_t mode = NORMAL;
int moved = 0;                  //checks if the joystick has moved
int pressed = 0;                //checks if the joystick has been pressed

int vertZero, horzZero;         //Stores the initial value of each axis, usually around 512
int vertValue, horzValue;       //Stores current analog output of each axis

int mouseClickFlag = 0;         //another click check
int b1flag = 0;                 //click check
int b2flag = 0;                 //click check
int b3flag = 0;                 //click check
int used = 0;                   //check if const are shown
int lastused = 0;               //check if const are shown

unsigned long releasetime = 0;  //used to register the releasing time (to select home or zoom)
unsigned long presstime = 0;    //as above
int registertime = 0;           //used to selective enter the release mode
unsigned long diff = 0;         //difference between time checks to read quick or long press


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

  Mouse.begin();                   //Init mouse emulation
  Keyboard.begin();                //Init keyboard emulation
#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop()
{
  unsigned long currentMillis = millis();
  int invertMouse;
  vertValue = analogRead(vertPin) - vertZero;  // read vertical offset
  horzValue = analogRead(horzPin) - horzZero;  // read horizontal offset

#ifdef DEBUG
  Serial.print("B1: ");
  Serial.println(digitalRead(b1Pin));
  Serial.print("B2: ");
  Serial.println(digitalRead(b2Pin));
  Serial.print("B3: ");
  Serial.println(digitalRead(b3Pin));
#endif

//BUTTON 1

  if ((digitalRead(b1Pin) == 0) && (!b1flag))  // if button 1 is pressed
  {
    b1flag = 1;
    used = 1;

    if (used != lastused) {
      default_mode = ROTATE;
    } else {
      default_mode = NORMAL;
      used = 0;
    }
    mode = default_mode;

  }
  else if (digitalRead(b1Pin) && (b1flag))
  {
    b1flag = 0;
  }
  lastused = used;


//BUTTON 2

  if ((digitalRead(b2Pin) == 0) && (!b2flag))  // if button 2 is pressed
  {
    b2flag = 1;
    Keyboard.write(KEY_HOME); // fit zoom
  }
  else if (digitalRead(b1Pin) && (b2flag))
  {
    b2flag = 0;
  }


//BUTTON 3

  if ((digitalRead(b3Pin) == 0) && (!b3flag))  // if button 3 is pressed
  {
    b3flag = 1;
    Keyboard.write(KEY_F7); //slice graphics (INVENTOR ONLY)
  }
  else if (digitalRead(b3Pin) && (b3flag))
  {
    b3flag = 0;
  }


//JOYSTICK CLICK

  if ((digitalRead(selPin) == 0) && (!mouseClickFlag))  // if the joystick button is pressed
  {
    presstime = currentMillis;
    registertime = 1;
    mouseClickFlag = 1;
    pressed = 1;
    mode = ZOOM;
  }
  else if ((digitalRead(selPin)) && (mouseClickFlag) && (registertime)) // if the joystick button is released
  {
    releasetime = currentMillis;
    registertime = 0;
    mouseClickFlag = 0;
    pressed = 0;
    mode = default_mode;

    diff = (releasetime - presstime);  //check if the press was quick or long

    if(diff < LONG_PRESS_INTERVAL)     //if quick press: call home view
    {
      Keyboard.write(KEY_F6); // home view
    }
  }
#ifdef DEBUG
  Serial.print("Default mode: ");
  Serial.println(default_mode);
  Serial.print("Mode: ");
  Serial.println(mode);
  Serial.println(" ");
#endif

  switch(mode) {   //rotate or zoom

    case ROTATE:

      invertMouse = -(INVERT_MOUSE); //restore mouse orientation

      if ( (vertValue <= MOVE_THRESHOLD)&&(vertValue >= (-MOVE_THRESHOLD)) )
        vertValue = 0;
      if ( (horzValue <= MOVE_THRESHOLD)&&(horzValue >= (-MOVE_THRESHOLD)) )
        horzValue = 0;
      if ( (horzValue != 0) || (vertValue != 0)) {
        Keyboard.press(KEY_F4); //rotate view (inventor mode)
        Mouse.press(MOUSE_LEFT);
        Mouse.move((invertMouse * (horzValue / sensitivity[NORMAL])), (invertMouse * (vertValue / sensitivity[NORMAL])), 0); // move mouse with f4 key pressed to rotate view
        moved = 1;
      }
      else {  // joystick is in neutral position: release all and stroke ESC to help go back to design view
        if (moved == 1) {
          Keyboard.releaseAll();
          Mouse.release(MOUSE_LEFT);
          Keyboard.write(KEY_ESC);
          moved = 0;
        }
      }
      break;

    case ZOOM:

      invertMouse = (INVERT_MOUSE);  //mouse inverted to push for zoom in and pull for zoom out

      if ( (vertValue > MOVE_THRESHOLD)||(vertValue < (-MOVE_THRESHOLD)) ){
        Keyboard.press(KEY_F3); // zoom view
        Mouse.press(MOUSE_LEFT);
        Mouse.move(0, (invertMouse * (vertValue / sensitivity[ZOOM])), 0); // move mouse on y axis with f3 key pressed to zoom view
        moved=1;
      }

      if ( (vertValue <= MOVE_THRESHOLD)&&(vertValue >= (-MOVE_THRESHOLD))  &&  (horzValue <= MOVE_THRESHOLD)&&(horzValue >= (-MOVE_THRESHOLD))  ){
          if (moved == 1) {
            Keyboard.releaseAll();
            Mouse.release(MOUSE_LEFT);
            Keyboard.write(KEY_ESC);
            moved = 0;
          }
      }
      break;

    case NORMAL:

      invertMouse = -(INVERT_MOUSE); //restore mouse orientation

      if ( (vertValue <= MOVE_THRESHOLD)&&(vertValue >= (-MOVE_THRESHOLD)) )
        vertValue = 0;
      if ( (horzValue <= MOVE_THRESHOLD)&&(horzValue >= (-MOVE_THRESHOLD)) )
        horzValue = 0;
      if ( (horzValue != 0) || (vertValue != 0))
        Mouse.move((invertMouse * (horzValue / sensitivity[NORMAL])), (invertMouse * (vertValue / sensitivity[NORMAL])), 0); // move mouse

      break;
    }

  delay(MOVE_SLOW_DOWN); //this slows down the moves

}