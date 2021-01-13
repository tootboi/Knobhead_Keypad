#include <Arduino.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

//variables for LED timeout
const unsigned long timeout = 60000;   //timeout in millisecond. change this value to adjust timeout
unsigned long idleTime;
bool idleStart = false;
bool idle = false;
bool rainbow = false;

//variables for NeoPixel
#define ledStripPin A3
#define ledCount 3

Adafruit_NeoPixel strip(ledCount, ledStripPin, NEO_GRB + NEO_KHZ800);

//variables for keypad matrix
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
void checkBit(byte layer, byte bit, byte led, uint32_t rgb);

//variables for rotary encoder
#define outputA A2
#define outputB A0
const int pulseDebounce = 0;
unsigned long lastPulse = 0;

uint8_t tableDecode();

#define encoderBtn 15

uint8_t getClick();

void setup() {
  Serial.begin(9600);

  //setup for NeoPixel
  strip.begin();
  strip.show();

  //setup for keypad matrix
  for(byte c=0; c<colNum; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  //setup for rotarty encoder
  pinMode(outputA, INPUT_PULLUP);
  pinMode(outputB, INPUT_PULLUP);
  pinMode(encoderBtn, INPUT_PULLUP);

  Consumer.begin(); //For writing media keys.
  Keyboard.begin(); //For writing normal keys.
  BootKeyboard.begin(); //For keyboard leds.
  Mouse.begin();
}

void loop() {
  //for timeout
  bool noInput = true;

  //code for layer leds
  if(!idle) strip.setBrightness(12);
  switch(layer) {
    //layer 0
    case 0:
      if(subLayer) {
        //sublayer
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 0, 255);
        uint32_t color2 = strip.Color(0, 0, 255);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      } else {
        //main
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 255, 0);
        uint32_t color2 = strip.Color(0, 255, 0);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      }
      break;
    //layer 1
    case 1:
      if(subLayer){
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 0, 255);
        uint32_t color2 = strip.Color(0, 0, 255);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      } else {
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 255, 0);
        uint32_t color2 = strip.Color(0, 255, 0);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      }
      break;
    //layer 2
    case 2:
      if(subLayer){
        strip.clear();
        uint32_t color0 = strip.Color(0, 113, 128);
        uint32_t color1 = strip.Color(0, 0, 255);
        uint32_t color2 = strip.Color(0, 0, 255);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_NUM_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      } else {
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 255, 0);
        uint32_t color2 = strip.Color(0, 255, 0);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      }
      break;
    //layer 3
    case 3:
      if(subLayer){
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 0, 255);
        uint32_t color2 = strip.Color(0, 0, 255);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      } else {
        strip.clear();
        uint32_t color0 = strip.Color(255, 255, 0);
        uint32_t color1 = strip.Color(0, 255, 0);
        uint32_t color2 = strip.Color(0, 255, 0);
        checkBit(layer, 1, 1, color1);
        checkBit(layer, 0, 2, color2);
        //code for led indicate
        if(BootKeyboard.getLeds() & LED_CAPS_LOCK) {  //for changing what led0 indicates
          strip.setPixelColor(0, color0); //for changing led0 color
        }
      }
      break;
      
    default:
      break;
  }
  
  uint8_t clickType = getClick();
  uint8_t direction = tableDecode();
  uint8_t key = getKey();
  //for timeout
  if(key || direction || clickType) noInput = false, idleStart = true, idle = false;

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
              Keyboard.print("z");    //change this line to modify functions.
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
      if(clickType) {
        switch(clickType) {
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
        clickType = 0;
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
              rainbow = !rainbow;    //change this line to modify functions.
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
      if(clickType) {
        switch(clickType) {
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
        clickType = 0;
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
      if(clickType) {
        switch(clickType) {
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
        clickType = 0;
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
              Keyboard.print("1");    //change this line to modify functions.
            }
            break;
          case 2:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("2");    //change this line to modify functions.
            }
            break;
          case 3:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("3");   //change this line to modify functions.
            }
            break;
          case 4:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("4");    //change this line to modify functions.
            }
            break;
          case 5:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("5");    //change this line to modify functions.
            }
            break;
          case 6:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("6");    //change this line to modify functions.
            }
            break;
          case 7:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("7");    //change this line to modify functions.
            }
            break;
          case 8:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("8");    //change this line to modify functions.
            }
            break;
          case 9:
            if(subLayer) {
              //sublayer
              //code    //change this line to modify functions.
            } else {
              Keyboard.print("9");    //change this line to modify functions.
            }
            break;

          default:
            break;
        }
        Keyboard.releaseAll();    //release all keys
      }
      
      //code for rotary encoder
      if(clickType) {
        switch(clickType) {
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
        clickType = 0;
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

  //for timeout
  if(idleStart) idleTime = millis(), idleStart = false;
  if(noInput && millis() - idleTime > timeout) {
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
      //dim LEDs
      strip.setBrightness(1);
    }
  }

  strip.show();
}

///custom functions
uint8_t getClick() {
  uint8_t clickType = 0;    //1==single, 2==double, 3==long
  static const int longPressTiming = 500;   //change this value to adjust long press timing.
  static const int doubleClickTiming = 300;    //change this value to adjust the timing of double clicks.
  static uint8_t clickState = HIGH;
  static uint8_t prevClickState = HIGH;
  static uint8_t clickCount = 0;
  static unsigned long lastPress;    //needs to be long not int, as int will overflow.
  static unsigned int clickTime;
  static unsigned int prevClickTime;
  static unsigned int pressFreq = 0;    //time between clicks
  static unsigned int timeElapsed;
  static const uint8_t clickDebounce = 20;

  clickState = digitalRead(encoderBtn);
  if(clickState == LOW) {
    if((millis() - lastPress) > clickDebounce) {
      lastPress = millis();
      //detect falling edge
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

  timeElapsed = millis() - clickTime;   //time elapsed since click. could overflow if click held for a long long time.

  if(clickCount==1 && timeElapsed < longPressTiming && timeElapsed >= doubleClickTiming && prevClickState) {    //for single click
    clickCount = 0;
    clickType = 1;
  } else if(clickCount==2 && pressFreq < doubleClickTiming && prevClickState) {    //for double click.
    clickCount = 0;
    clickType = 2;
  } else if(clickCount==1 && timeElapsed >= longPressTiming && prevClickState) {    //for long press
    clickCount = 0;
    clickType = 3;
  }
  return clickType;
}

int getKey() {
  static uint8_t lastKey;
  static uint8_t prevKeyStates[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  static uint16_t keyHoldBuffer = 0;
  static uint16_t bufferThreshold = 1300;   //change this value to adjust timing of repeating key
  static uint16_t repeatBuffer = 0;
  static uint16_t repeatThreshold = 80;    //change this value to adjust timing of repeat freq
  int currKey = 0;

  for(byte r=0; r<rowNum; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    for(byte c=0; c<colNum; c++) {
      if(digitalRead(colPins[c]) == 0) {
        //detects falling edge
        if(prevKeyStates[c+colNum*r] == 1) {
          prevKeyStates[c+colNum*r] = 0;
          currKey = c+colNum*r+1;
          lastKey = currKey;
          keyHoldBuffer = 0;
          delay(10);    //reduces the amount of ghost keys, i think
        }
        if(c+colNum*r+1 == lastKey) {   //check if currentKey is same as last held key
          //if key is held down, repeatedly send the key
          if(keyHoldBuffer >= bufferThreshold) {
            //this slows the frequency of repeated keys
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
  static uint8_t validation = 0;    //for validating if last two prevStates are valid
  static int8_t encoderLUT[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
  static int8_t counterTest =  0;

  prevStates <<= 2;
  if(digitalRead(outputA)) prevStates |= 0x01;
  if(digitalRead(outputB)) prevStates |= 0x02;
  prevStates &= 0xf;    //this removes bits beyond 4 bits, that were created by the bit shift

  int8_t decodedVal = encoderLUT[prevStates];
  if(decodedVal) {
    validation <<= 4;
    validation |= prevStates;
    if(validation==23) {    //BINARY 00010111
      Serial.print("CW  | ");
      Serial.print("counter: ");
      Serial.println(counterTest);
      counterTest++;
      return 1;
    } else if(validation==43) {   //BINARY 00101011
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