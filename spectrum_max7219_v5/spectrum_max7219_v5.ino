#include <Arduino.h>
#include "ArdRTOS.h"
#include "setting.h"

void readSerial(void);
void appSerial();
void text_Screen();
void scrollText(const char *p);

#include "spectrum.h"
#include "DS1307_CFG.h"
#include "button_rotary_led_composite.h"

#define  DELAYTIME  100  // in milliseconds
uint8_t string[54] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};

void  setup() {
  // Set up button_rotary_led_composite
  setupBRLC();

  // ADC config
  setupADC();

  // Time config
  setTime();

  // LED matrix config
  mx.begin();               // initialize display
  intro();
  mx.clear();
  delay(10);

  OS.addTask(vScreenTask, 256);
  OS.addTask(vISR_RotaryEncoder, 128);
  OS.addTask(vButton_Task, 128);
  OS.addTask(vLED_Task, 128);
  OS.begin();
}// end setup

void loop()
{
}

//#####################################################################
void intro(void) {
  for (byte i = 0; i < xres; i++)
  {
    for (byte j = 1; j <= yres; j++)
    {
      displayvalue = MY_ARRAY[j];
      displaycolumn = xres - 1 - i;
      mx.setColumn(displaycolumn, displayvalue);              //  for left to right
      delay(10);
    }
  }
}

#define BUF_SIZE  60
uint8_t newMessage[BUF_SIZE];
bool newMessageAvailable = false;

void readSerial(void)
{
  static uint8_t  putIndex = 0;
  while (Serial.available())
  {
    newMessage[putIndex] = (char)Serial.read();
    if ((newMessage[putIndex] == '\n') || (putIndex >= BUF_SIZE - 3)) // end of message character or full buffer
    {
      newMessage[putIndex] = '\0';
      // restart the index for next filling spree and flag we have a message waiting
      putIndex = 0;
      newMessageAvailable = true;
    } else if (newMessage[putIndex] != '\r')
      // Just save the next char in next location
      putIndex++;

    OS.delay(1);
  }
}

void appSerial() {
  for (;;) {
    readSerial();
    if (newMessageAvailable)  // there is a new message waiting
    {
      Serial.println((char* )newMessage);
  
      uint8_t keyMess[5], valueMess[BUF_SIZE - 5];
      for (int i = 0; i < 4; i++)
        keyMess[i] = newMessage[i];
      keyMess[4] = '\0';
  
      int j = 0;
      for (int i = 5; i < sizeof(newMessage); i++)
      {
        valueMess[j] = newMessage[i];
        j++;
      }
      Serial.println((char* )keyMess);
      Serial.println((char* )valueMess);
  
      OS.delay(1);
      if (strcmp((char* )keyMess, "time") == 0) {          // if received time
        // data value: Jul 22 2022-12:34:56-
        int j = 0;
        uint8_t _time[15], _date[10];
        uint8_t count = 0, pos[2] = {0};
        for (int i = 0; i < sizeof(valueMess); i++) {
          if (count == 2) break;
          if (valueMess[i] == '-') {
            pos[count] = i;
            count++;
          }
        }
        for (int i = 0; i < pos[0]; i++) {
          _date[i] = valueMess[i];
          j++;
        }
        _date[j] = '\0';
        j = 0;
        for (int i = pos[0] + 1; i < pos[1]; i++) {
          _time[j] = valueMess[i];
          j++;
        }
        _time[j] = '\0';
  
        rtc.adjust(DateTime(_date, _time));
        mx.clear();
        newMessageAvailable = false;
        for (byte i = 0; i < 3; i++) {
          status_LED = 2;
          OS.delay(1);
        }
        status_LED = 4;
      } else if (strcmp((char* )keyMess, "mode") == 0) {
        if (strcmp((char* )valueMess, "band") == 0) {
          (band == 16) ? (band = 32) : (band = 16);
          valueMess[0] = '\0';
        } else {
          (displaymode < 5) ? (displaymode++) : (displaymode = 1);
          displayModeChange(displaymode);
        }
        newMessageAvailable = false;
        for (byte i = 0; i < 3; i++) {
          status_LED = 2;
          OS.delay(1);
        }
        status_LED = 4;
      } else if (strcmp((char* )keyMess, "text") == 0) {
        uint8_t arrayLength = sizeof(valueMess) / sizeof(valueMess[0]);
        memcpy(string, valueMess, sizeof(valueMess));
        newMessageAvailable = false;
        for (byte i = 0; i < 3; i++) {
          status_LED = 2;
          OS.delay(1);
        }
        status_LED = 4;
      } else {
        newMessageAvailable = false;
      }
    }
    OS.delay(1);
  }
}

void scrollText(const char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts

  while (*p != '\0' && screen == 3)
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

    for (uint8_t i = 0; i <= charWidth; i++) // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      mx.setColumn(0, (i < charWidth) ? cBuf[i] : 0);
      OS.delay(DELAYTIME);
    }
  }
}

void text_Screen() {
  scrollText((char* )string);
  appSerial();
}
