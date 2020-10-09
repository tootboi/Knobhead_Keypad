#include <Arduino.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

//variables for NeoPixel
#define ledStripPin A3
#define ledCount 3

Adafruit_NeoPixel strip(ledCount, ledStripPin, NEO_GRB + NEO_KHZ800);

//variables for keypad matrix
const int matrixDebounce = 10;
unsigned long prevPressTimeKeypad;

int pressCount;
int lastPressCount;

const int rowNum = 3;
const int colNum = 3;

byte rowPins[rowNum] = {10, 9, 7};
byte colPins[colNum] = {8, 14, 16};

int getKey();

//    btn matrix layout:
//    |----------------|
//    |[1]    [2]   [3]|
//    |[4]    [5]   [6]|
//    |[7]    [8]   [9]|
//    |----------------|

//variable for layers
int layer = 0;
bool subLayer = false;

void layerChange(byte sign);

//variables for rotary encoder
#define outputA A0
#define outputB A2
int counter = 0;
int aState;
int prevAState;
const int pulseDebounce = 1;
unsigned long lastPulse = 0;

int getEncoderDirection();

#define encoderBtn 15
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
byte encoderClickType = 0;

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
  if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
    strip.setPixelColor(0, strip.Color(187, 0, 255)); //for changing led0 color
    strip.show();
  } else {
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
    encoderClickType = 2;
      //for debugging
    Serial.print("Double click | ");
    Serial.print("Layer: ");
    Serial.println(layer);
  } else if(clickCount == 1 && timeElapsed >= longPressTiming && released && !rotated) {    //for long press
    clickCount = 0;
    encoderClickType = 3;
      //for debugging
    Serial.print("Long press | ");
    Serial.print("Layer: ");
    Serial.println(layer);
  } else if(clickCount == 1 && timeElapsed >= doubleClickTiming && !held && !rotated) {    //for single click
      //for debugging
    Serial.print(clickCount);
    Serial.print(" counts | timeElapsed: ");
    clickCount = 0;   //reset counter
    encoderClickType = 1;
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
  }

  //code for layer leds
  switch(layer) {
    //layer 0
    case 0:
      strip.clear();
      strip.show();
      break;
    //layer 1
    case 1:
      strip.clear();
      if(subLayer){
        //sublayer
        strip.setPixelColor(2, strip.Color(0, 0, 255));   //bit0
      } else {
        //main
        strip.setPixelColor(2, strip.Color(0, 255, 0));   //bit0
      }
      strip.show();
      break;
    //layer 2
    case 2:
      strip.clear();
      if(subLayer){
        strip.setPixelColor(1, strip.Color(0, 0, 255));   //bit1
      } else {
        strip.setPixelColor(1, strip.Color(0, 255, 0));   //bit1
      }
      strip.show();
      break;
    //layer 3
    case 3:
      if(subLayer){
        strip.setPixelColor(1, strip.Color(0, 0, 255));   //bit1
        strip.setPixelColor(2, strip.Color(0, 0, 255));   //bit0
      } else {
        strip.setPixelColor(1, strip.Color(0, 255, 0));   //bit1
        strip.setPixelColor(2, strip.Color(0, 255, 0));   //bit0
      }
      strip.show();
      break;
      
    default:
      break;
  }
  
  if(millis() - lastPulse > pulseDebounce) {
    lastPulse = millis();
    int direction = getEncoderDirection();
    int key = getKey();
    switch (layer) {
      //layer 0
      case 0:
        //code for keypad
        if(key){
          Serial.print("key: ");
          Serial.println(key);
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.print("layer 0 ");    //change this line to modify functions.
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Consumer.write(CONSUMER_CALCULATOR);    //change this line to modify functions.
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_PAGE_UP);    //change this line to modify functions.
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('z');    //change this line to modify functions.
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('y');    //change this line to modify functions.
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_PAGE_DOWN);    //change this line to modify functions.
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('x');    //change this line to modify functions.
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('c');    //change this line to modify functions.
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('v');    //change this line to modify functions.
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();    //release all keys
        }
        
        //code for rotary encoder
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                //sublayer
                subLayer = !subLayer;
              } else {
                Consumer.write(MEDIA_VOLUME_MUTE);
              }
              break;
            case 2: //double
              if(subLayer) {
                //sublayer
                layerChange(0);
              } else {
                layerChange(0);
              }
              break;
            case 3: //long
              if(subLayer) {
                //sublayer
                layerChange(1);
              } else {
                layerChange(1);
              }
              break;

            default:
              break;
          }
          encoderClickType = 0;
        }
        if(direction) {
          switch (direction) {
            case 1:     //clockwise
              if(subLayer) {
                Consumer.write(MEDIA_VOLUME_UP);
              } else {
                Consumer.write(MEDIA_VOLUME_UP);
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                Consumer.write(MEDIA_VOLUME_DOWN);
              } else {
                Consumer.write(MEDIA_VOLUME_DOWN);
              }
              break;

            default:
              break;
          }
        }
        Keyboard.releaseAll();    //release all keys
        break;
        
      //layer 1
      case 1:
        //code for keypad
        if(key){
          Serial.print("key: ");
          Serial.println(key);
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.print("layer 1 ");    //change this line to modify functions.
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEY_LEFT_SHIFT);    //change this line to modify functions.
                Keyboard.press(KEY_LEFT_ALT);    //change this line to modify functions.
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press(KEY_F1);    //change this line to modify functions.
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();    //release all keys
        }
        
        //code for rotary encoder
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                subLayer = !subLayer;
              } else {
                Consumer.write(MEDIA_VOLUME_MUTE);
              }
              break;
            case 2: //double
              if(subLayer) {
                layerChange(0);
              } else {
                layerChange(0);
              }
              break;
            case 3: //long
              if(subLayer) {
                layerChange(1);
              } else {
                layerChange(1);
              }
              break;

            default:
              break;
          }
          encoderClickType = 0;
        }
        if(direction) {
          switch (direction) {
            case 1:     //clockwise
              if(subLayer) {
                Consumer.write(MEDIA_VOLUME_UP);
              } else {
                Consumer.write(MEDIA_VOLUME_UP);
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                Consumer.write(MEDIA_VOLUME_DOWN);
              } else {
                Consumer.write(MEDIA_VOLUME_DOWN);
              }              
              break;

            default:
              break;
          }
        }
        Keyboard.releaseAll();  
        break;

      //layer 2
      case 2:
        //code for keypad
        if(key){
          Serial.print("key: ");
          Serial.println(key);
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer
                Keyboard.press(KEY_NUM_LOCK);    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_1);    //change this line to modify functions.
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_2);    //change this line to modify functions.
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_3);    //change this line to modify functions.
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_4);    //change this line to modify functions.
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_5);    //change this line to modify functions.
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_6);    //change this line to modify functions.
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer
                Keyboard.press(KEYPAD_0);    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_7);    //change this line to modify functions.
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_8);    //change this line to modify functions.
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.press(KEYPAD_9);    //change this line to modify functions.
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();    //release all keys
        }
        
        //code for rotary encoder
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                subLayer = !subLayer;
              } else {
                subLayer = !subLayer;
              }              
              break;
            case 2: //double
              if(subLayer) {
                layerChange(0);
              } else {
                layerChange(0);
              }              
              break;
            case 3: //long
              if(subLayer) {
                layerChange(1);
              } else {
                layerChange(1);
              }              
              break;

            default:
              break;
          }
          encoderClickType = 0;
        }
        if(direction) {
          switch (direction) {
            case 1:     //clockwise
              if(subLayer) {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('+');    //change this line to modify functions.
              } else {
                Consumer.write(MEDIA_VOLUME_UP);
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                Keyboard.press(KEY_LEFT_CTRL);    //change this line to modify functions.
                Keyboard.press('-');    //change this line to modify functions.
              } else {
                Consumer.write(MEDIA_VOLUME_DOWN);
              }
              break;

            default:
              break;
          }
        }
        Keyboard.releaseAll(); 
        break;

      //layer 3
      case 3:
        //code for keypad
        if(key){
          Serial.print("key: ");
          Serial.println(key);
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                Keyboard.print("layer 3 ");    //change this line to modify functions.
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer
                //code    //change this line to modify functions.
              } else {
                //code    //change this line to modify functions.
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();    //release all keys
        }
        
        //code for rotary encoder
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                subLayer = !subLayer;
              } else {
                Consumer.write(MEDIA_VOLUME_MUTE);
              }
              break;
            case 2: //double
              if(subLayer) {
                layerChange(0);
              } else {
                layerChange(0);
              }              
              break;
            case 3: //long
              if(subLayer) {
                layerChange(1);
              } else {
                layerChange(1);
              }              
              break;

            default:
              break;
          }
          encoderClickType = 0;
        }
        if(direction) {
          switch (direction) {
            case 1:     //clockwise
              if(subLayer) {
                Consumer.write(MEDIA_VOLUME_UP);
              } else {
                Mouse.move(0, 0, -1);     //change this line to modify functions.
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                Consumer.write(MEDIA_VOLUME_DOWN);
              } else {
                Mouse.move(0, 0, 1);     //change this line to modify functions.
              }
              break;

            default:
              break;
          }
        }
        Keyboard.releaseAll();  
        break;

      default:
        break;
    }
  }
}

///custom functions
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

void layerChange(byte sign) {
  switch(sign) {
    case 0: //increase layer
      if(layer < 3) {
        layer++;
      } else {
        layer = 0;
      }
      break;
    case 1: //decrease layer
      if(layer == 0) {
        layer = 3;
      } else {
        layer--;
      }
      break;
    
    default:
      break;
  }
}