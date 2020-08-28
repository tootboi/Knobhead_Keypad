#include <Arduino.h>
#include <HID-Project.h>
#include <Keypad.h>

//variable for capsLock led
const int capsLed = A0;

//variable for layers
int layer = 0;
const int bit1 = A1;
const int bit0 = A2;

//variables for rotary encoder
#define outputA 2
#define outputB 3
int counter = 0;
int aState;
int prevAState;

#define encoderBtn 16
int btnState = 1;
unsigned int pressFreq = 0;
bool held = false;
unsigned int timeElapsed;
unsigned int pressTime;
unsigned int prevPressTime;
int clickCount = 0;
unsigned int currTime;
unsigned int doubleClickTiming = 300;    //change this value to adjust the timing of double clicks.
const int debounce = 20;
unsigned long lastPress;    //needs to be long not int as int will overflow (rolls over) and cause problems.

//variables for keypad matrix
const int  ROW_NUM = 3;
const int COLUMN_NUM = 3;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};
byte row_pins[ROW_NUM] = {5, 6, 7};
byte column_pins[COLUMN_NUM] = {8, 9, 10};
Keypad keypad = Keypad(makeKeymap(keys), row_pins, column_pins, ROW_NUM, COLUMN_NUM);

void setup() {
  Serial.begin(9600);

  //setup for capsLock
  pinMode(capsLed, OUTPUT);

  //setup for layer LEDs
  pinMode(bit0, OUTPUT);
  pinMode(bit1, OUTPUT);

  //setup for rotarty encoder
  pinMode(outputA,INPUT_PULLUP);
  pinMode(outputB,INPUT_PULLUP);
  prevAState = digitalRead(outputA);
  pinMode(encoderBtn, INPUT_PULLUP);

  Consumer.begin(); //For writing media keys.
  Keyboard.begin(); //For writing normal keys.
  BootKeyboard.begin(); //For keyboard leds.
}

void loop() {
  //code for capsLock led
  if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    digitalWrite(capsLed, HIGH);
  } else {
    digitalWrite(capsLed, LOW);
  }

  //code for rotary encoder
  aState = digitalRead(outputA);
  if(aState != prevAState) {
    if(digitalRead(outputB) != aState) {
      counter ++;
      if((counter % 2) == 0) {        //needed as my rotary encoder sends two pulse per detent
        Consumer.write(MEDIA_VOLUME_UP);    //change this line to modify functions.
      }
    } else {
      counter --;
      if((counter % 2) == 0) {        //needed as my rotary encoder sends two pulse per detent
        Consumer.write(MEDIA_VOLUME_DOWN);    //change this line to modify functions.
      }
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }
  prevAState = aState;

    //code for encoder btn
  btnState = digitalRead(encoderBtn);
  if(btnState == LOW) {
    if((millis() - lastPress) > debounce) {
        //for debugging
      Serial.print("Time now: ");
      Serial.print(millis());
      Serial.print(" | Lastpress: ");
      Serial.print(lastPress);
      Serial.print(" | time between: ");
      Serial.println(millis() - lastPress);
      //update lastPress
      lastPress = millis();
      if(held == false) {
          //pressFreq logic does not have problems with int overflow (roll over).
          //as pressTime and prevPressTime are same data types and their negation gets assigned
          //to same data type (pressFreq).
        pressTime = millis();
        if(clickCount > 0) {
          pressFreq = pressTime - prevPressTime;
        }
        prevPressTime = pressTime;
        clickCount++;
        }
      held = true;
    }
  } else {
    held = false;
  }

  currTime = millis();
  timeElapsed = currTime - pressTime;   //time elapsed since last btn press

  if(clickCount == 2 && pressFreq < doubleClickTiming && !held) {    //for double click.
    Serial.print(clickCount);
    Serial.print(" counts | Layer: ");
    clickCount = 0;   //reset counter
    //for changing layer
    if(layer < 3) {
      layer++;
    } else {
      layer = 0;
    }
      //for debugging
    Serial.print(layer);
    Serial.println(" double clicked");
  } else if(clickCount == 1 && timeElapsed >= doubleClickTiming && !held) {    //for single click
    Serial.print(clickCount);
    Serial.print(" counts | timeElapsed: ");
    clickCount = 0;   //reset counter
    Consumer.write(MEDIA_VOLUME_MUTE);    //change this line to modify functions.
      //for debugging
    Serial.print(timeElapsed);
    Serial.println(" single clicked");
  }

  //code for keypad
  char key = keypad.getKey();
  switch (layer) {
    //layer 0
    case 0:
      //code for layer LEDs
      digitalWrite(bit1, LOW);
      digitalWrite(bit0, LOW);
      //code for keypad
      if(key){
        Serial.println(key);
        switch (key) {
        case '1':
          Keyboard.print("layer 0 ");    //change this line to modify functions.
          break;
        case '2':
          Consumer.write(CONSUMER_CALCULATOR);    //change this line to modify functions.
          break;
        case '3':
          Keyboard.press(KEY_PAGE_UP);    //change this line to modify functions.
          break;
        case '4':
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('z');    //change this line to modify functions.
          break;
        case '5':
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('y');    //change this line to modify functions.
          break;
        case '6':
          Keyboard.press(KEY_PAGE_DOWN);    //change this line to modify functions.
          break;
        case '7':
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('x');    //change this line to modify functions.
          break;
        case '8':
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('c');    //change this line to modify functions.
          break;
        case '9':
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('v');    //change this line to modify functions.
          break;

        default:
          break;
        }
        Keyboard.releaseAll();    //release all keys
      }
      break;
      
    //layer 1
    case 1:
      //code for layer LEDs
      digitalWrite(bit1, LOW);
      digitalWrite(bit0, HIGH);
      //code for keypad
      if(key){
        Serial.println(key);
        switch (key) {
        case '1':
          Keyboard.print("layer 1 ");    //change this line to modify functions.
          break;
        case '2':
          Keyboard.press(KEY_LEFT_SHIFT);    //change this line to modify functions.
          Keyboard.press(KEY_LEFT_ALT);    //change this line to modify functions.
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('z');    //change this line to modify functions.
          break;
        case '3':
          //code    //change this line to modify functions.
          break;
        case '4':
          //code    //change this line to modify functions.
          break;
        case '5':
          //code    //change this line to modify functions.
          break;
        case '6':
          //code    //change this line to modify functions.
          break;
        case '7':
          //code    //change this line to modify functions.
          break;
        case '8':
          //code    //change this line to modify functions.
          break;
        case '9':
          //code    //change this line to modify functions.
          break;

        default:
          break;
        }
        Keyboard.releaseAll();    //release all keys
      }
      break;

    //layer 2
    case 2:
      //code for layer LEDs
      digitalWrite(bit1, HIGH);
      digitalWrite(bit0, LOW);
      //code for keypad
      if(key){
        Serial.println(key);
        switch (key) {
        case '1':
          Keyboard.print("layer 2 ");    //change this line to modify functions.
          break;
        case '2':
          //code    //change this line to modify functions.
          break;
        case '3':
          //code    //change this line to modify functions.
          break;
        case '4':
          //code    //change this line to modify functions.
          break;
        case '5':
          //code    //change this line to modify functions.
          break;
        case '6':
          //code    //change this line to modify functions.
          break;
        case '7':
          //code    //change this line to modify functions.
          break;
        case '8':
          //code    //change this line to modify functions.
          break;
        case '9':
          //code    //change this line to modify functions.
          break;

        default:
          break;
        }
        Keyboard.releaseAll();    //release all keys
      }
      break;

    //layer 3
    case 3:
      //code for layer LEDs
      digitalWrite(bit1, HIGH);
      digitalWrite(bit0, HIGH);
      //code for keypad
      if(key){
        Serial.println(key);
        switch (key) {
        case '1':
          Keyboard.print("layer 3 ");    //change this line to modify functions.
          break;
        case '2':
          //code    //change this line to modify functions.
          break;
        case '3':
          //code    //change this line to modify functions.
          break;
        case '4':
          //code    //change this line to modify functions.
          break;
        case '5':
          //code    //change this line to modify functions.
          break;
        case '6':
          //code    //change this line to modify functions.
          break;
        case '7':
          //code    //change this line to modify functions.
          break;
        case '8':
          //code    //change this line to modify functions.
          break;
        case '9':
          //code    //change this line to modify functions.
          break;

        default:
          break;
        }
        Keyboard.releaseAll();    //release all keys
      }
      break;

    default:
      break;
  }
}