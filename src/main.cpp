#include <Arduino.h>
#include <HID-Project.h>

//variables for keypad matrix
const int matrixDebounce = 10;
unsigned long prevPressTimeKeypad;

int pressCount;
int lastPressCount;

const int rowNum = 3;
const int colNum = 3;

byte rowPins[rowNum] = {5, 6, 7};
byte colPins[colNum] = {8, 9, 10};

int getKey();

//    btn matrix layout:
//    |----------------|
//    |[1]    [2]   [3]|
//    |[4]    [5]   [6]|
//    |[7]    [8]   [9]|
//    |----------------|

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
unsigned int longPressTiming = 500;   //change this value to adjust long press timing.

void setup() {
  Serial.begin(9600);

  //setup for keypad matrix
  for(byte r=0; r<rowNum; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }

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
  } else if(clickCount == 1 && timeElapsed >= longPressTiming && held) {    //for long press
    Serial.println("Long press");
    clickCount = 0;
    //change layer
    if(layer == 0) {
      layer = 3;
    } else {
      layer--;
    }
  }

  //code for keypad matrix
  int key = getKey();
  switch (layer) {
    //layer 0
    case 0:
      //code for layer LEDs
      digitalWrite(bit1, LOW);
      digitalWrite(bit0, LOW);
      //code for keypad
      if(key){
        Serial.print("key: ");
        Serial.println(key);
        switch (key) {
        case 1:
          Keyboard.print("layer 0 ");    //change this line to modify functions.
          break;
        case 2:
          Consumer.write(CONSUMER_CALCULATOR);    //change this line to modify functions.
          break;
        case 3:
          Keyboard.press(KEY_PAGE_UP);    //change this line to modify functions.
          break;
        case 4:
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('z');    //change this line to modify functions.
          break;
        case 5:
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('y');    //change this line to modify functions.
          break;
        case 6:
          Keyboard.press(KEY_PAGE_DOWN);    //change this line to modify functions.
          break;
        case 7:
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('x');    //change this line to modify functions.
          break;
        case 8:
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press('c');    //change this line to modify functions.
          break;
        case 9:
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
        Serial.print("key: ");
        Serial.println(key);
        switch (key) {
        case 1:
          Keyboard.print("layer 1 ");    //change this line to modify functions.
          break;
        case 2:
          Keyboard.press(KEY_LEFT_SHIFT);    //change this line to modify functions.
          Keyboard.press(KEY_LEFT_ALT);    //change this line to modify functions.
          Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
          Keyboard.press(KEY_F1);    //change this line to modify functions.
          break;
        case 3:
          //code    //change this line to modify functions.
          break;
        case 4:
          //code    //change this line to modify functions.
          break;
        case 5:
          //code    //change this line to modify functions.
          break;
        case 6:
          //code    //change this line to modify functions.
          break;
        case 7:
          //code    //change this line to modify functions.
          break;
        case 8:
          //code    //change this line to modify functions.
          break;
        case 9:
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
        Serial.print("key: ");
        Serial.println(key);
        switch (key) {
        case 1:
          Keyboard.print("layer 2 ");    //change this line to modify functions.
          break;
        case 2:
          //code    //change this line to modify functions.
          break;
        case 3:
          //code    //change this line to modify functions.
          break;
        case 4:
          //code    //change this line to modify functions.
          break;
        case 5:
          //code    //change this line to modify functions.
          break;
        case 6:
          //code    //change this line to modify functions.
          break;
        case 7:
          //code    //change this line to modify functions.
          break;
        case 8:
          //code    //change this line to modify functions.
          break;
        case 9:
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
        Serial.print("key: ");
        Serial.println(key);
        switch (key) {
        case 1:
          Keyboard.print("layer 3 ");    //change this line to modify functions.
          break;
        case 2:
          //code    //change this line to modify functions.
          break;
        case 3:
          //code    //change this line to modify functions.
          break;
        case 4:
          //code    //change this line to modify functions.
          break;
        case 5:
          //code    //change this line to modify functions.
          break;
        case 6:
          //code    //change this line to modify functions.
          break;
        case 7:
          //code    //change this line to modify functions.
          break;
        case 8:
          //code    //change this line to modify functions.
          break;
        case 9:
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

int getKey() {
  int currKey=0;
  pressCount = 0;
  if((millis() - prevPressTimeKeypad) > matrixDebounce) {
    prevPressTimeKeypad = millis();
    for(byte c=0; c<colNum; c++) {
      //pulse col.
      pinMode(colPins[c], OUTPUT);
      digitalWrite(colPins[c], LOW);
      for(byte r=0; r<rowNum; r++) {
        //check if btn is pressed.
        if(digitalRead(rowPins[r]) == 0) {
          pressCount++;
          //pressCount==1 limits concurrent btn press to 1.
          //lastPressCount==0 ensures that all btns were released.
          if(pressCount == 1 && lastPressCount == 0) {
            currKey = c+colNum*r+1;
            break;
          }
        }
      }
      //end col pulse.
      digitalWrite(colPins[c], HIGH);
      pinMode(colPins[c], INPUT);
    }
    //save current pressCount.
    lastPressCount = pressCount;
  }
  return(currKey);
}