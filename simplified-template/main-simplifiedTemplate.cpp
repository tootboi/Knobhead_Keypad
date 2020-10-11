#include <Arduino.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

#define ledStripPin A3
#define ledCount 3
Adafruit_NeoPixel strip(ledCount, ledStripPin, NEO_GRB + NEO_KHZ800);

const int matrixDebounce = 10;
unsigned long prevPressTimeKeypad;
int pressCount;
int lastPressCount;
const int rowNum = 3;
const int colNum = 3;
byte rowPins[rowNum] = {10, 9, 7};
byte colPins[colNum] = {8, 14, 16};
int getKey();

int layer = 0;
bool subLayer = false;
void layerChange(byte sign);
void checkBit(byte layer, byte bit, byte led, uint32_t rgb);

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
unsigned int doubleClickTiming = 300;
const int debounce = 20;
unsigned long lastPress;
unsigned int longPressTiming = 500;
bool released = false;
bool rotated = false;
byte encoderClickType = 0;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();
  strip.setBrightness(15);

  for(byte r=0; r<rowNum; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }

  pinMode(outputA,INPUT_PULLUP);
  pinMode(outputB,INPUT_PULLUP);
  prevAState = digitalRead(outputA);
  pinMode(encoderBtn, INPUT_PULLUP);

  Consumer.begin();
  Keyboard.begin();
  BootKeyboard.begin();
  Mouse.begin();
}

void loop() {
  btnState = digitalRead(encoderBtn);
  if(btnState == LOW) {
    if((millis() - lastPress) > debounce) {
      lastPress = millis();
      if(held == false) {
        if(rotated) {
          rotated = false;
          clickCount = 0;
        }
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
  timeElapsed = currTime - pressTime;

  if(clickCount == 2 && pressFreq < doubleClickTiming && !held && !rotated) {    //for double click.
    clickCount = 0;
    encoderClickType = 2;
  } else if(clickCount == 1 && timeElapsed >= longPressTiming && released && !rotated) {    //for long press
    clickCount = 0;
    encoderClickType = 3;
  } else if(clickCount == 1 && timeElapsed >= doubleClickTiming && !held && !rotated) {    //for single click
    clickCount = 0;
    encoderClickType = 1;
  }

  switch(layer) {
    //layer 0
    case 0:
      if(subLayer) {
        strip.clear();
        //sublayer 0 led
      } else {
        strip.clear();
        //main layer 0 led
      }
      break;
    //layer 1
    case 1:
      if(subLayer){
        strip.clear();
        //sublayer 1 led
      } else {
        strip.clear();
        //main layer 1 led
      }
      break;
    //layer 2
    case 2:
      if(subLayer){
        strip.clear();
        //sublayer 2 led
      } else {
        strip.clear();
        //main layer 2 led
      }
      break;
    //layer 3
    case 3:
      if(subLayer){
        strip.clear();
        //sublayer 3 led
      } else {
        strip.clear();
        //main layer 3 led
      }
      break;
      
    default:
      break;
  }
  strip.show();
  
  if(millis() - lastPulse > pulseDebounce) {
    lastPulse = millis();
    int direction = getEncoderDirection();
    int key = getKey();
    switch (layer) {
      //layer 0
      case 0:
        //code for key
        if(key){
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer 0 key 1
              } else {
                //main layer 0 key 1
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer 0 key 2
              } else {
                //main layer 0 key 2
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer 0 key 3
              } else {
                //main layer 0 key 3
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer 0 key 4
              } else {
                //main layer 0 key 4
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer 0 key 5
              } else {
                //main layer 0 key 5
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer 0 key 6
              } else {
                //main layer 0 key 6
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer 0 key 7
              } else {
                //main layer 0 key 7
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer 0 key 8
              } else {
                //main layer 0 key 8
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer 0 key 9
              } else {
                //main layer 0 key 9
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();
        }
        
        //code for knob
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                //sublayer 0 single
              } else {
                //main layer 0 single
              }
              break;
            case 2: //double
              if(subLayer) {
                //sublayer 0 double
              } else {
                //main layer 0 double
              }
              break;
            case 3: //long
              if(subLayer) {
                //sublayer 0 long
              } else {
                //main layer 0 long
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
                //sublayer 0 clkwise
              } else {
                //main layer 0 clkwise
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                //sublayer 0 anticlk
              } else {
                //main layer 0 anticlk
              }
              break;

            default:
              break;
          }
        }
        Keyboard.releaseAll();
        break;
        
      //layer 1
      case 1:
        //code for key
        if(key){
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer 1 key 1
              } else {
                //main layer 1 key 1
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer 1 key 2
              } else {
                //main layer 1 key 2
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer 1 key 3
              } else {
                //main layer 1 key 3
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer 1 key 4
              } else {
                //main layer 1 key 4
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer 1 key 5
              } else {
                //main layer 1 key 5
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer 1 key 6
              } else {
                //main layer 1 key 6
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer 1 key 7
              } else {
                //main layer key 7
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer 1 key 8
              } else {
                //main layer 1 key 8
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer 1 key 9
              } else {
                //main layer 1 key 9
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();
        }
        
        //code for knob
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                //sublayer 1 single
              } else {
                //main layer 1 single
              }
              break;
            case 2: //double
              if(subLayer) {
                //sublayer 1 double
              } else {
                //main layer 1 double
              }
              break;
            case 3: //long
              if(subLayer) {
                //sublayer 1 long
              } else {
                //main layer 1 long
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
                //sublayer 1 clkwise
              } else {
                //main layer 1 clkwise
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                //sublayer 1 anticlk
              } else {
                //main layer 1 anticlk
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
        //code for key
        if(key){
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer 2 key 1
              } else {
                //main layer 2 key 1
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer 2 key 2
              } else {
                //main layer 2 key 2
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer 2 key 3
              } else {
                //main layer 2 key 3
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer 2 key 4
              } else {
                //main layer 2 key 4
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer 2 key 5
              } else {
                //main layer 2 key 5
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer 2 key 6
              } else {
                //main layer 2 key 6
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer 2 key 7
              } else {
                //main layer 2 key 7
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer 2 key 8
              } else {
                //main layer 2 key 8
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer 2 key 9
              } else {
                //main layer 2 key 9
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();
        }
        
        //code for knob
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                //sublayer 2 single
              } else {
                //main layer 2 single
              }              
              break;
            case 2: //double
              if(subLayer) {
                //sublayer 2 double
              } else {
                //main layer 2 double
              }              
              break;
            case 3: //long
              if(subLayer) {
                //sublayer 2 long
              } else {
                //main layer 2 long
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
                //sublayer 2 clkwise
              } else {
                //main layer 2 clkwise
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                //sublayer 2 anticlk
              } else {
                //main layer 2 anticlk
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
        //code for key
        if(key){
          switch (key) {
            case 1:
              if(subLayer) {
                //sublayer 3 key 1
              } else {
                //main layer 3 key 1
              }
              break;
            case 2:
              if(subLayer) {
                //sublayer 3 key 2
              } else {
                //main layer 3 key 2
              }
              break;
            case 3:
              if(subLayer) {
                //sublayer 3 key 3
              } else {
                //main layer 3 key 3
              }
              break;
            case 4:
              if(subLayer) {
                //sublayer 3 key 4
              } else {
                //main layer 3 key 4
              }
              break;
            case 5:
              if(subLayer) {
                //sublayer 3 key 5
              } else {
                //main layer 3 key 5
              }
              break;
            case 6:
              if(subLayer) {
                //sublayer 3 key 6
              } else {
                //main layer 3 key 6
              }
              break;
            case 7:
              if(subLayer) {
                //sublayer 3 key 7
              } else {
                //main layer 3 key 7
              }
              break;
            case 8:
              if(subLayer) {
                //sublayer 3 key 8
              } else {
                //main layer 3 key 8
              }
              break;
            case 9:
              if(subLayer) {
                //sublayer 3 key 9
              } else {
                //main layer 3 key 9
              }
              break;

            default:
              break;
          }
          Keyboard.releaseAll();
        }
        
        //code for knob
        if(encoderClickType) {
          switch(encoderClickType) {
            case 1: //single
              if(subLayer) {
                //sublayer 3 single
              } else {
                //main layer 3 single
              }
              break;
            case 2: //double
              if(subLayer) {
                //sublayer 3 double
              } else {
                //main layer 3 double
              }              
              break;
            case 3: //long
              if(subLayer) {
                //sublayer 3 long
              } else {
                //main layer 3 long
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
                //sublayer 3 clkwise
              } else {
                //main layer 3 clkwise
              }
              break;
            case 2:     //anti-clockwise
              if(subLayer) {
                //sublayer 3 anticlk
              } else {
                //main layer 3 anticlk
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
      pinMode(colPins[c], OUTPUT);
      digitalWrite(colPins[c], LOW);
      for(byte r=0; r<rowNum; r++) {
        if(digitalRead(rowPins[r]) == 0) {
          pressCount++;
          if(pressCount == 1 && lastPressCount == 0) {
            currKey = c+colNum*r+1;
            break;
          }
        }
      }
      digitalWrite(colPins[c], HIGH);
      pinMode(colPins[c], INPUT);
    }
    lastPressCount = pressCount;
  }
  return(currKey);
}

int getEncoderDirection() {
  int direction = 0;
  aState = digitalRead(outputA);
  if(aState != prevAState) {
    if(digitalRead(outputB) != aState) {
      counter ++;
      if((counter % 2) == 0) {
        direction = 1;
      }
    } else {
      counter --;
      if((counter % 2) == 0) {
        direction = 2;
      }
    }
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

void checkBit(byte layer, byte bit, byte led, uint32_t rgb) {
  switch(layer) {
    case 0:
      break;
    case 1:
      if(bit==0) {
        strip.setPixelColor(led, rgb);   //bit0
      }
      break;
    case 2:
      if(bit==1) {
        strip.setPixelColor(led, rgb);   //bit1
      }
      break;
    case 3:
      strip.setPixelColor(led, rgb);   //bit0 and bit1
      break;
    
    default:
      break;
  }
}