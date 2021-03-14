//Author Stefano Sbarra
//April 2020
//FREE FOR ANY NON COMMERCIAL PROJECT

//THIS SKETCH MAKES ARDUINO LEONARDO (OR MICRO) WORK AS A SPACE 3D MOUSE FOR INVENTOR AND FUSION 360
//NORMAL JOYSTICK MOVE ACT AS VIEW ROTATION USING F4 KEY
//PRESS AND MOVE ACT AS VIEW ZOOM USING F3 KEY
//QUICK PRESS OF THE JOYSTICK GOES TO HOME VIEW USING F6
//ADDITIONAL FUNCTIONS AND SWITCHES ARE EASY TO IMPLEMENT (EXAMPLE SLICE VIEW)
#include <Arduino.h>
#include <Mouse.h>
#include <Keyboard.h>

//moved and esc key helps to go back from view mode to design

int horzPin = A1;  // Analog output of horizontal joystick pin
int vertPin = A0;  // Analog output of vertical joystick pin
int selPin = 10;   // select button pin of joystick
int b1Pin = 7;     // button 1
int b2Pin = 8;     // button 2
int b3Pin = 9;     // button 3

char f3key = KEY_F3;      //zoom view
char f4key = KEY_F4;      //rotate view
char f6key = KEY_F6;      //home view
char f7key = KEY_F7;      //slice graphics (INVENTOR ONLY)
char f8key = KEY_F8;      //show constraints (INVENTOR ONLY)
char f9key = KEY_F9;      //hide constraints (INVENTOR ONLY)
char keyhome = KEY_HOME;  //fit zoom
char esc = KEY_ESC;       //ESC key

int moved = 0;    //checks if the joystick has moved
int mode = 0;     //selects zoom or rotate
int pressed = 0;  //checks if the joystick has been pressed

int vertZero, horzZero;       //Stores the initial value of each axis, usually around 512
int vertValue, horzValue;     //Stores current analog output of each axis
const int sensitivity = 200;  //Higher sensitivity value = slower mouse, should be <= about 500
int mouseClickFlag = 0;       //another click check
int b1flag = 0;               //click check
int b2flag = 0;               //click check
int b3flag = 0;               //click check
int used = 0;                 //check if const are shown
int lastused = 0;             //check if const are shown

//int invertMouse = 1;        //Invert joystick based on orientation
int invertMouse = -1;         //Noninverted joystick based on orientation

unsigned long releasetime = 0;  //used to register the releasing time (to select home or zoom)
unsigned long presstime = 0;    //as above
int registertime = 0;           //used to selective enter the release mode
unsigned long diff = 0;         //difference between time checks to read quick or long press

const long interval = 500;      //interval to establish if quick or long

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

  Mouse.begin();      //Init mouse emulation
  Keyboard.begin();   //Init keyboard emulation
//  Serial.begin(9600);

}

void loop()
{
  unsigned long currentMillis = millis();

  vertValue = analogRead(vertPin) - vertZero;  // read vertical offset
  horzValue = analogRead(horzPin) - horzZero;  // read horizontal offset

//  Serial.print("B1: ");
//  Serial.println(digitalRead(b1Pin));
//  Serial.print("B2: ");
//  Serial.println(digitalRead(b2Pin));
//  Serial.print("B3: ");
//  Serial.println(digitalRead(b3Pin));
//  Serial.println(" ");


//BUTTON 1

  if ((digitalRead(b1Pin) == 0) && (!b1flag))  // if button 1 is pressed
  {
    b1flag = 1;
    used = 1;

    if (used != lastused) {
      Keyboard.write(f8key);
    } else {
      Keyboard.write(f9key);
      used = 0;
    }

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
    Keyboard.write(keyhome);
  }
  else if (digitalRead(b1Pin) && (b2flag))
  {
    b2flag = 0;
  }


//BUTTON 3

  if ((digitalRead(b3Pin) == 0) && (!b3flag))  // if button 3 is pressed
  {
    b3flag = 1;
    Keyboard.write(f7key);
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
    mode = 1;
  }
  else if ((digitalRead(selPin)) && (mouseClickFlag) && (registertime)) // if the joystick button is not pressed
  {
    releasetime = currentMillis;
    registertime = 0;
    mouseClickFlag = 0;
    pressed = 0;
    mode = 0;

    diff = (releasetime - presstime);  //check if the click was quick or long

    if(diff < interval)                //if quick call home view
    {
      Keyboard.write(f6key);
    }
  }


  switch(mode) {   //rotate or zoom

    case 0:

      invertMouse = -1; //restore mouse orientation

      if ( (vertValue > 2)||(vertValue < (-2)) ){  // position two instead of 0 cancel the small fluctuation between readings
        Keyboard.press(f4key);
        Mouse.press(MOUSE_LEFT);
        Mouse.move(0, (invertMouse * (vertValue / sensitivity)), 0); // move mouse on y axis with f4 key pressed to rotate view
        moved=1;
      }

      if ( (horzValue > 2)||(horzValue < (-2)) ){
        Keyboard.press(f4key);
        Mouse.press(MOUSE_LEFT);
        Mouse.move((invertMouse * (horzValue / sensitivity)), 0, 0); // move mouse on x axis with f4 key pressed to rotate view
        moved=1;
      }

      if ( (vertValue <= 2)&&(vertValue >= (-2))  &&  (horzValue <= 2)&&(horzValue >= (-2))  ){   //if joystick is in neutral position release all and stroke ESC to help go back to design view
          Keyboard.releaseAll();
          Mouse.release(MOUSE_LEFT);
          if(moved==1){
            Keyboard.write(esc);
            moved=0;
          }
      }
      break;

    case 1:

      invertMouse = 1;  //mouse inverted to push for zoom in and pull for zoom out

      if ( (vertValue > 2)||(vertValue < (-2)) ){
        Keyboard.press(f3key);
        Mouse.press(MOUSE_LEFT);
        Mouse.move(0, (invertMouse * (vertValue / sensitivity)), 0); // move mouse on y axis with f3 key pressed to zoom view
        moved=1;
      }

      if ( (vertValue <= 2)&&(vertValue >= (-2))  &&  (horzValue <= 2)&&(horzValue >= (-2))  ){
          Keyboard.releaseAll();
          Mouse.release(MOUSE_LEFT);
          if(moved==1){
            Keyboard.write(esc);
            moved=0;
          }
      }
      break;

    }

  delay(10); //this slows down the moves

}