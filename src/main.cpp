#include <Arduino.h>
#include <HID-Project.h>
#include <Keypad.h>

//variable for capsLock led
const int capsLed = A0;

//variables for rotary encoder
#define outputA 2
#define outputB 3
int counter = 0;
int aState;
int prevAState;

//variables for keypad matrix
const int  ROW_NUM = 4;
const int COLUMN_NUM = 3;
char keys[ROW_NUM][COLUMN_NUM] = {
  {' ', '0', ' '},
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};
byte row_pins[ROW_NUM] = {4, 5, 6, 7};
byte column_pins[COLUMN_NUM] = {8, 9, 10};
Keypad keypad = Keypad(makeKeymap(keys), row_pins, column_pins, ROW_NUM, COLUMN_NUM);

void setup() {
  Serial.begin(9600);

  //setup for capsLock
  pinMode(capsLed, OUTPUT);

  //setup for rotarty encoder
  pinMode(outputA,INPUT_PULLUP);
  pinMode(outputB,INPUT_PULLUP);
  prevAState = digitalRead(outputA);

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
        Consumer.write(MEDIA_VOLUME_UP);
      }
    } else {
      counter --;
      if((counter % 2) == 0) {        //needed as my rotary encoder sends two pulse per detent
        Consumer.write(MEDIA_VOLUME_DOWN);
      }
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }
  prevAState = aState;

  //code for keypad
  char key = keypad.getKey();
  if(key){
    Serial.println(key);
    switch (key)
    {
    case '0':
      Consumer.write(MEDIA_VOLUME_MUTE);
      break;
    case '1':
      Consumer.write(CONSUMER_CALCULATOR);
      break;
    case '2':
      //code
      break;
    case '3':
      Keyboard.press(KEY_PAGE_UP);
      break;
    case '4':
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('z');
      break;
    case '5':
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('y');
      break;
    case '6':
      Keyboard.press(KEY_PAGE_DOWN);
      break;
    case '7':
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('x');
      break;
    case '8':
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('c');
      break;
    case '9':
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('v');
      break;

    default:
      break;
    }
    Keyboard.releaseAll();    //release all keys
  }
}