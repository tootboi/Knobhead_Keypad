#include <Arduino.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

#define ledStripPin A3
#define ledCount 3
#define outputA A2
#define outputB A0
#define encoderBtn 15

const unsigned long timeout = 60000;
unsigned long idleTime;
bool idleStart = false;
bool idle = false;
bool rainbow = false;
bool timeoutEnabled = false;
const int rowNum = 3;
const int colNum = 3;
byte rowPins[rowNum] = {10, 9, 7};
byte colPins[colNum] = {8, 14, 16};
int layer = 0;
bool subLayer = false;
const int pulseDebounce = 0;
unsigned long lastPulse = 0;

Adafruit_NeoPixel strip(ledCount, ledStripPin, NEO_GRB + NEO_KHZ800);

int getKey();
void layerChange(byte sign);
void checkBit(byte layer, byte bit, byte led, uint32_t rgb);
uint8_t tableDecode();
uint8_t getClick();

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();
  for(byte c=0; c<colNum; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  pinMode(outputA, INPUT_PULLUP);
  pinMode(outputB, INPUT_PULLUP);
  pinMode(encoderBtn, INPUT_PULLUP);
  Consumer.begin();
  Keyboard.begin();
  BootKeyboard.begin();
  Mouse.begin();
}

void loop() {
  bool noInput = true;
  if(!idle) strip.setBrightness(12);

  switch(layer) {
    //layer 0
    case 0:
      if(subLayer) {
        //sublayer 0
        strip.clear();
      } else {
        //mainlayer 0
        strip.clear();
      }
      break;
    //layer 1
    case 1:
      if(subLayer){
        //sublayer 1
        strip.clear();
      } else {
        //mainlayer 1
        strip.clear();
      }
      break;
    //layer 2
    case 2:
      if(subLayer){
        //sublayer 2
        strip.clear();
      } else {
        //mainlayer 2
        strip.clear();
      }
      break;
    //layer 3
    case 3:
      if(subLayer){
        //sublayer 3
        strip.clear();
      } else {
        //mainlayer 3
        strip.clear();
      }
      break;
      
    default:
      break;
  }
  
  uint8_t clickType = getClick();
  uint8_t direction = tableDecode();
  uint8_t key = getKey();
  if(key || direction || clickType) noInput = false, idleStart = true, idle = false;

  switch (layer) {
    //layer 0
    case 0:
      if(key){
        switch (key) {
          case 1:
            if(subLayer) {
              //sublayer 0 key 1
            } else {
              //mainlayer 0 key 1
            }
            break;
          case 2:
            if(subLayer) {
              //sublayer 0 key 2
            } else {
              //mainlayer 0 key 2
            }
            break;
          case 3:
            if(subLayer) {
              //sublayer 0 key 3
            } else {
              //mainlayer 0 key 3
            }
            break;
          case 4:
            if(subLayer) {
              //sublayer 0 key 4
            } else {
              //mainlayer 0 key 4
            }
            break;
          case 5:
            if(subLayer) {
              //sublayer 0 key 5
            } else {
              //mainlayer 0 key 5
            }
            break;
          case 6:
            if(subLayer) {
              //sublayer 0 key 6
            } else {
              //mainlayer 0 key 6
            }
            break;
          case 7:
            if(subLayer) {
              //sublayer 0 key 7
            } else {
              //mainlayer 0 key 7
            }
            break;
          case 8:
            if(subLayer) {
              //sublayer 0 key 8
            } else {
              //mainlayer 0 key 8
            }
            break;
          case 9:
            if(subLayer) {
              //sublayer 0 key 9
            } else {
              //mainlayer 0 key 9
            }
            break;

          default:
            break;
        }
        Keyboard.releaseAll();
      }
      
      if(clickType) {
        switch(clickType) {
          case 1: //single
            if(subLayer) {
              //sublayer 0 single
            } else {
              //mainlayer 0 single
            }
            break;
          case 2: //double
            if(subLayer) {
              //sublayer 0 double
            } else {
              //mainlayer 0 double
            }
            break;
          case 3: //long
            if(subLayer) {
              //sublayer 0 long
            } else {
              //mainlayer 0 long
            }
            break;

          default:
            break;
        }
        clickType = 0;
      }
      if(direction) {
        switch (direction) {
          case 1:     //clockwise
            if(subLayer) {
              //sublayer 0 cw
            } else {
              //mainlayer 0 cw
            }
            break;
          case 2:     //anti-clockwise
            if(subLayer) {
              //sublayer 0 ccw
            } else {
              //mainlayer 0 ccw
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
      if(key){
        switch (key) {
          case 1:
            if(subLayer) {
              //sublayer 1 key 1
            } else {
              //mainlayer 1 key 1
            }
            break;
          case 2:
            if(subLayer) {
              //sublayer 1 key 2
            } else {
              //mainlayer 1 key 2
            }
            break;
          case 3:
            if(subLayer) {
              //sublayer 1 key 3
            } else {
              //mainlayer 1 key 3
            }
            break;
          case 4:
            if(subLayer) {
              //sublayer 1 key 4
            } else {
              //mainlayer 1 key 4
            }
            break;
          case 5:
            if(subLayer) {
              //sublayer 1 key 5
            } else {
              //mainlayer 1 key 5
            }
            break;
          case 6:
            if(subLayer) {
              //sublayer 1 key 6
            } else {
              //mainlayer 1 key 6
            }
            break;
          case 7:
            if(subLayer) {
              //sublayer 1 key 7
            } else {
              //mainlayer 1 key 7
            }
            break;
          case 8:
            if(subLayer) {
              //sublayer 1 key 8
            } else {
              //mainlayer 1 key 8
            }
            break;
          case 9:
            if(subLayer) {
              //sublayer 1 key 9
            } else {
              //mainlayer 1 key 9
            }
            break;

          default:
            break;
        }
        Keyboard.releaseAll();
      }
      
      if(clickType) {
        switch(clickType) {
          case 1: //single
            if(subLayer) {
              //sublayer 1 single
            } else {
              //mainlayer 1 single
            }
            break;
          case 2: //double
            if(subLayer) {
              //sublayer 1 double
            } else {
              //mainlayer 1 double
            }
            break;
          case 3: //long
            if(subLayer) {
              //sublayer 1 long
            } else {
              //mainlayer 1 long
            }
            break;

          default:
            break;
        }
        clickType = 0;
      }
      if(direction) {
        switch (direction) {
          case 1:     //clockwise
            if(subLayer) {
              //sublayer 1 cw
            } else {
              //mainlayer 1 cw
            }
            break;
          case 2:     //anti-clockwise
            if(subLayer) {
              //sublayer 1 ccw
            } else {
              //mainlayer 1 ccw
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
      if(key){
        switch (key) {
          case 1:
            if(subLayer) {
              //sublayer 2 key 1
            } else {
              //mainlayer 2 key 1
            }
            break;
          case 2:
            if(subLayer) {
              //sublayer 2 key 2
            } else {
              //mainlayer 2 key 2
            }
            break;
          case 3:
            if(subLayer) {
              //sublayer 2 key 3
            } else {
              //mainlayer 2 key 3
            }
            break;
          case 4:
            if(subLayer) {
              //sublayer 2 key 4
            } else {
              //mainlayer 2 key 4
            }
            break;
          case 5:
            if(subLayer) {
              //sublayer 2 key 5
            } else {
              //mainlayer 2 key 5
            }
            break;
          case 6:
            if(subLayer) {
              //sublayer 2 key 6
            } else {
              //mainlayer 2 key 6
            }
            break;
          case 7:
            if(subLayer) {
              //sublayer 2 key 7
            } else {
              //mainlayer 2 key 7
            }
            break;
          case 8:
            if(subLayer) {
              //sublayer 2 key 8
            } else {
              //mainlayer 2 key 8
            }
            break;
          case 9:
            if(subLayer) {
              //sublayer 2 key 9
            } else {
              //mainlayer 2 key 9
            }
            break;

          default:
            break;
        }
        Keyboard.releaseAll();
      }
      
      if(clickType) {
        switch(clickType) {
          case 1: //single
            if(subLayer) {
              //sublayer 2 single
            } else {
              //mainlayer 2 single
            }
            break;
          case 2: //double
            if(subLayer) {
              //sublayer 2 double
            } else {
              //mainlayer 2 double
            }
            break;
          case 3: //long
            if(subLayer) {
              //sublayer 2 long
            } else {
              //mainlayer 2 long
            }
            break;

          default:
            break;
        }
        clickType = 0;
      }
      if(direction) {
        switch (direction) {
          case 1:     //clockwise
            if(subLayer) {
              //sublayer 2 cw
            } else {
              //mainlayer 2 cw
            }
            break;
          case 2:     //anti-clockwise
            if(subLayer) {
              //sublayer 2 ccw
            } else {
              //mainlayer 2 ccw
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
      if(key){
        switch (key) {
          case 1:
            if(subLayer) {
              //sublayer 3 key 1
            } else {
              //mainlayer 3 key 1
            }
            break;
          case 2:
            if(subLayer) {
              //sublayer 3 key 2
            } else {
              //mainlayer 3 key 2
            }
            break;
          case 3:
            if(subLayer) {
              //sublayer 3 key 3
            } else {
              //mainlayer 3 key 3
            }
            break;
          case 4:
            if(subLayer) {
              //sublayer 3 key 4
            } else {
              //mainlayer 3 key 4
            }
            break;
          case 5:
            if(subLayer) {
              //sublayer 3 key 5
            } else {
              //mainlayer 3 key 5
            }
            break;
          case 6:
            if(subLayer) {
              //sublayer 3 key 6
            } else {
              //mainlayer 3 key 6
            }
            break;
          case 7:
            if(subLayer) {
              //sublayer 3 key 7
            } else {
              //mainlayer 3 key 7
            }
            break;
          case 8:
            if(subLayer) {
              //sublayer 3 key 8
            } else {
              //mainlayer 3 key 8
            }
            break;
          case 9:
            if(subLayer) {
              //sublayer 3 key 9
            } else {
              //mainlayer 3 key 9
            }
            break;

          default:
            break;
        }
        Keyboard.releaseAll();
      }
      
      if(clickType) {
        switch(clickType) {
          case 1: //single
            if(subLayer) {
              //sublayer 3 single
            } else {
              //mainlayer 3 single
            }
            break;
          case 2: //double
            if(subLayer) {
              //sublayer 3 double
            } else {
              //mainlayer 3 double
            }
            break;
          case 3: //long
            if(subLayer) {
              //sublayer 3 long
            } else {
              //mainlayer 3 long
            }
            break;

          default:
            break;
        }
        clickType = 0;
      }
      if(direction) {
        switch (direction) {
          case 1:     //clockwise
            if(subLayer) {
              //sublayer 3 cw
            } else {
              //mainlayer 3 cw
            }
            break;
          case 2:     //anti-clockwise
            if(subLayer) {
              //sublayer 3 ccw
            } else {
              //mainlayer 3 ccw
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

  if(idleStart) idleTime = millis(), idleStart = false;
  if(noInput && millis() - idleTime > timeout && timeoutEnabled) {
    idle = true;
    if(rainbow) {
      static long j = 0;
      if(j<65536) {
        strip.clear();
        strip.setBrightness(30);
        strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(j + (2-0) * 3000)));
        strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(j + (2-1) * 3000)));
        strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(j + (2-2) * 3000)));
        strip.show();
        j+=256;
        delay(10);
      } else {
        j = 0;
      }
    } else {
      strip.setBrightness(1);
    }
  }

  strip.show();
}

///custom functions
uint8_t getClick() {
  uint8_t clickType = 0;
  static const int longPressTiming = 500;
  static const int doubleClickTiming = 300;
  static uint8_t clickState = HIGH;
  static uint8_t prevClickState = HIGH;
  static uint8_t clickCount = 0;
  static unsigned long lastPress;
  static unsigned int clickTime;
  static unsigned int prevClickTime;
  static unsigned int pressFreq = 0;
  static unsigned int timeElapsed;
  static const uint8_t clickDebounce = 20;

  clickState = digitalRead(encoderBtn);
  if(clickState == LOW) {
    if((millis() - lastPress) > clickDebounce) {
      lastPress = millis();
      if(prevClickState == HIGH) {
        clickTime = millis();
        if(clickCount) {
          pressFreq = clickTime - prevClickTime;
        }
        prevClickTime = clickTime;
        clickCount++;
        }
      prevClickState = clickState;
    }
  } else {
    prevClickState = clickState;
  }

  timeElapsed = millis() - clickTime;

  if(clickCount==1 && timeElapsed < longPressTiming && timeElapsed >= doubleClickTiming && prevClickState) {
    clickCount = 0;
    clickType = 1;
  } else if(clickCount==2 && pressFreq < doubleClickTiming && prevClickState) {
    clickCount = 0;
    clickType = 2;
  } else if(clickCount==1 && timeElapsed >= longPressTiming && prevClickState) {
    clickCount = 0;
    clickType = 3;
  }
  return clickType;
}

int getKey() {
  static uint8_t lastKey;
  static uint8_t prevKeyStates[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  static uint16_t keyHoldBuffer = 0;
  static uint16_t bufferThreshold = 1300;
  static uint16_t repeatBuffer = 0;
  static uint16_t repeatThreshold = 80;
  int currKey = 0;

  for(byte r=0; r<rowNum; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    for(byte c=0; c<colNum; c++) {
      if(digitalRead(colPins[c]) == 0) {
        if(prevKeyStates[c+colNum*r] == 1) {
          prevKeyStates[c+colNum*r] = 0;
          currKey = c+colNum*r+1;
          lastKey = currKey;
          keyHoldBuffer = 0;
          delay(10);
        }
        if(c+colNum*r+1 == lastKey) {
          if(keyHoldBuffer >= bufferThreshold) {
            if(repeatBuffer >= repeatThreshold) currKey = c+colNum*r+1, repeatBuffer = 0;
            else repeatBuffer++;
          } else {
            keyHoldBuffer++;
          }
        }
      } else {
        prevKeyStates[c+colNum*r] = 1;
      }
    }
    digitalWrite(rowPins[r], HIGH);
    pinMode(rowPins[r], INPUT);
  }
  return(currKey);
}

uint8_t tableDecode() {
  static uint8_t prevStates = 0;
  static uint8_t validation = 0;
  static int8_t encoderLUT[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
  static int8_t counterTest =  0;

  prevStates <<= 2;
  if(digitalRead(outputA)) prevStates |= 0x01;
  if(digitalRead(outputB)) prevStates |= 0x02;
  prevStates &= 0xf;

  int8_t decodedVal = encoderLUT[prevStates];
  if(decodedVal) {
    validation <<= 4;
    validation |= prevStates;
    if(validation==23) {
      Serial.print("CW  | ");
      Serial.print("counter: ");
      Serial.println(counterTest);
      counterTest++;
      return 1;
    } else if(validation==43) {
      Serial.print("CCW | ");
      Serial.print("counter: ");
      Serial.println(counterTest);
      counterTest--;
      return 2;
    }
  }
  return 0;
}

void layerChange(byte sign) {
  switch(sign) {
    case 0:
      if(layer < 3) {
        layer++;
      } else {
        layer = 0;
      }
      break;
    case 1:
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
        strip.setPixelColor(led, rgb);
      }
      break;
    case 2:
      if(bit==1) {
        strip.setPixelColor(led, rgb);
      }
      break;
    case 3:
      strip.setPixelColor(led, rgb);
      break;
    
    default:
      break;
  }
}