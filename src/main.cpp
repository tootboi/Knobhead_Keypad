#include <Arduino.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

//variables for NeoPixel
#define ledStripPin 15
#define ledCount 3

Adafruit_NeoPixel strip(ledCount, ledStripPin, NEO_GRB + NEO_KHZ800);

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

//variable for layers
int layer = 0;

//variables for rotary encoder
#define outputA 2
#define outputB 3
int counter = 0;
int aState;
int prevAState;
const int pulseDebounce = 5;
unsigned long lastPulse = 0;

int getEncoderDirection();

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
bool released = false;
bool rotated = false;

void setup() {
  Serial.begin(9600);

  //setup for NeoPixel
  strip.begin();
  strip.show();
  strip.setBrightness(15);

  //setup for keypad matrix
  for(byte r=0; r<rowNum; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }

  //setup for rotarty encoder
  pinMode(outputA,INPUT_PULLUP);
  pinMode(outputB,INPUT_PULLUP);
  prevAState = digitalRead(outputA);
  pinMode(encoderBtn, INPUT_PULLUP);

  Consumer.begin(); //For writing media keys.
  Keyboard.begin(); //For writing normal keys.
  BootKeyboard.begin(); //For keyboard leds.
  Mouse.begin();
}

void loop() {
  //code for NeoPixel

  //code for capsLock led
  if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    //digitalWrite(capsLed, HIGH);
    strip.setPixelColor(0, strip.Color(255, 255, 0));
    strip.show();
  } else {
    //digitalWrite(capsLed, LOW);
    strip.clear();
    strip.show();
  }

    //code for encoder btn
  btnState = digitalRead(encoderBtn);
  if(btnState == LOW) {
    if((millis() - lastPress) > debounce) {
        //for debugging
      // Serial.print("Time now: ");
      // Serial.print(millis());
      // Serial.print(" | Lastpress: ");
      // Serial.print(lastPress);
      // Serial.print(" | time between: ");
      // Serial.println(millis() - lastPress);
      //update lastPress
      lastPress = millis();
      if(held == false) {
        if(rotated) {
          rotated = false;
          clickCount = 0;
        }
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
    if(held) {
      released = true;
    } else {
      released = false;
    }
    held = false;
  }

  currTime = millis();
  timeElapsed = currTime - pressTime;   //time elapsed since last btn press

  if(clickCount == 2 && pressFreq < doubleClickTiming && !held && !rotated) {    //for double click.
      //for debugging
    // Serial.print(clickCount);
    clickCount = 0;   //reset counter
    //for changing layer
    if(layer < 3) {
      layer++;
    } else {
      layer = 0;
    }
      //for debugging
    Serial.print("Double click | ");
    Serial.print("Layer: ");
    Serial.println(layer);
  } else if(clickCount == 1 && timeElapsed >= longPressTiming && released && !rotated) {    //for long press
    clickCount = 0;
    //change layer
    if(layer == 0) {
      layer = 3;
    } else {
      layer--;
    }
      //for debugging
    Serial.print("Long press | ");
    Serial.print("Layer: ");
    Serial.println(layer);
  } else if(clickCount == 1 && timeElapsed >= doubleClickTiming && !held && !rotated) {    //for single click
      //for debugging
    Serial.print(clickCount);
    Serial.print(" counts | timeElapsed: ");
    clickCount = 0;   //reset counter
    Consumer.write(MEDIA_VOLUME_MUTE);    //change this line to modify functions.
      //for debugging
    Serial.print(timeElapsed);
    Serial.println(" single clicked");
  } else if(held) {
    if(millis() - lastPulse > pulseDebounce) {
      lastPulse = millis();
      int direction = getEncoderDirection();
      if(direction) {
        rotated = true;
      }
      switch(direction) {
        case 1:
          Serial.println("held clockwise");
          break;
        case 2:
          Serial.println("held anti-clockwise");
          break;

        default:
          break;
      }
    }
    // Serial.print("rotated: ");
    // Serial.println(rotated);
  }

  // //code for rotary encoder
  if(millis() - lastPulse > pulseDebounce) {
    lastPulse = millis();
    int direction = getEncoderDirection();
    switch(layer) {
      //layer0
      case 0:
        switch (direction) {
          case 1:     //clockwise
            Consumer.write(MEDIA_VOLUME_UP);
            break;
          case 2:     //anti-clockwise
            Consumer.write(MEDIA_VOLUME_DOWN);
            break;

          default:
            break;
        }
        Keyboard.releaseAll();    //release all keys
        break;
      //layer1
      case 1:
        switch (direction) {
          case 1:     //clockwise
            Consumer.write(MEDIA_VOLUME_UP);
            break;
          case 2:     //anti-clockwise
            Consumer.write(MEDIA_VOLUME_DOWN);
            break;

          default:
            break;
        }
        Keyboard.releaseAll();    //release all keys
        break;
      //layer2
      case 2:
        switch (direction) {
          case 1:     //clockwise
            Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
            Keyboard.press('+');    //change this line to modify functions.
            break;
          case 2:     //anti-clockwise
            Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
            Keyboard.press('-');    //change this line to modify functions.
            break;

          default:
            break;
        }
        Keyboard.releaseAll();    //release all keys
        break;
      //layer3
      case 3:
        switch (direction) {
          case 1:     //clockwise
            Mouse.move(0, 0, -1);     //change this line to modify functions.
            break;
          case 2:     //anti-clockwise
            Mouse.move(0, 0, 1);     //change this line to modify functions.
            break;

          default:
            break;
        }
        Keyboard.releaseAll();    //release all keys
        break;
      
      default:
        break;
    }
  }
  

  //code for keypad matrix
  int key = getKey();
  switch (layer) {
    //layer 0
    case 0:
      //code for layer LEDs
      strip.setPixelColor(1, strip.Color(0, 0, 0));   //bit1
      strip.show();
      strip.setPixelColor(2, strip.Color(0, 0, 0));   //bit0
      strip.show();
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
      strip.setPixelColor(1, strip.Color(0, 0, 0));   //bit1
      strip.show();
      strip.setPixelColor(2, strip.Color(0, 255, 0));   //bit0
      strip.show();
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
      strip.setPixelColor(1, strip.Color(0, 255, 0));   //bit1
      strip.show();
      strip.setPixelColor(2, strip.Color(0, 0, 0));   //bit0
      strip.show();
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
      strip.setPixelColor(1, strip.Color(0, 255, 0));   //bit1
      strip.show();
      strip.setPixelColor(2, strip.Color(0, 255, 0));   //bit0
      strip.show();
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

int getEncoderDirection() {
  int direction = 0;
  aState = digitalRead(outputA);
  if(aState != prevAState) {
      //for debugging
    //Serial.println("pulse");
    if(digitalRead(outputB) != aState) {
      counter ++;
      if((counter % 2) == 0) {        //needed as my rotary encoder sends two pulse per detent
        direction = 1;
      }
    } else {
      counter --;
      if((counter % 2) == 0) {        //needed as my rotary encoder sends two pulse per detent
        direction = 2;
      }
    }
      //for debugging
    //Serial.println(direction);
  }
  prevAState = aState;
  return(direction);
}