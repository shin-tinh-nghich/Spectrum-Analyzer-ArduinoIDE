#include <RTClib.h>
#include "fonts.h"

#define MAX_COL 29

RTC_DS1307 rtc;

void fillBuffer(word buffer[], byte newTime[], byte lastTime[]);
void printBuffer(word buffer[], byte newTime[], byte lastTime[]);
bool timeChanged(byte newTime[], byte lastTime[]);
void printTime(word buffer[], byte newTime[], byte lastTime[]);
void getTime(byte newTime[]);
//#####################################################################
void setTime()
{
  rtc.begin();
  // RTC config
  if (! rtc.isrunning()) {
    //Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(2024, 1, 1, 0, 0, 0));
  }
}

byte lastTime[6] = {0, 0, 0, 0, 0, 0};
byte newTime[6] = {0, 0, 0, 0, 0, 0};
word buffer[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void time_Screen()
{
  getTime(newTime);
  fillBuffer(buffer, newTime, lastTime);
  printTime(buffer, newTime, lastTime);
  memcpy(lastTime, newTime, 6);
  appSerial();
}
//#####################################################################
void fillBuffer(word buffer[], byte newTime[], byte lastTime[])
{
  byte buf = 0;
  for (byte i = 0; i < 6; i++)
  {
    for (byte j = 0; j < 3; j++)
    {
      //buffer[buf] = (numbers[lastTime[i]][j] << 8) + numbers[newTime[i]][j];
      buffer[buf] = (pgm_read_byte_near(&numbers[lastTime[i]][j]) << 8) + pgm_read_byte_near(&numbers[newTime[i]][j]);
      buf++;
    }
  }
}

void printBuffer(word buffer[], byte newTime[], byte lastTime[])
{
  byte digit = 0;
  byte col = MAX_COL;
  mx.control(0, MAX_DEVICES - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (byte i = 0; i < 18; i++)
  {
    if (i % 3 == 0 && i > 0)
    {
      col--;
      digit++;
    }
    if (i % 6 == 0 && i > 0)
    {
      mx.setColumn(col, points);
      col -= 2;
    }
    mx.setColumn(col, (buffer[i] >> 8));
    col--;
  }
  mx.control(0, MAX_DEVICES - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

bool timeChanged(byte newTime[], byte lastTime[])
{
  for (byte i = 0; i < 6; i++)
  {
    if (newTime[i] != lastTime[i])
      return true;
  }
  return false;
}

void printTime(word buffer[], byte newTime[], byte lastTime[])
{
  if (!timeChanged(newTime, lastTime))
    return;

  printBuffer(buffer, newTime, lastTime);
}

void getTime(byte newTime[])
{
  DateTime now = rtc.now();
  newTime[0] = floor(now.hour() / 10);
  newTime[1] = now.hour() - (newTime[0] * 10);
  newTime[2] = floor(now.minute() / 10);
  newTime[3] = now.minute() - (newTime[2] * 10);
  newTime[4] = floor(now.second() / 10);
  newTime[5] = now.second() - (newTime[4] * 10);    //delay 6s
}
