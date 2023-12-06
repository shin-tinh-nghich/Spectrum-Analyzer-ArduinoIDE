#include <RotaryEncoder.h>
#include <HID-Project.h>

#define PIN_IN1 A3
#define PIN_IN2 A2

const uint8_t keyNEXT = 5;
const uint8_t keyPLAY = 6;
const uint8_t keyPREV = 7;
const uint8_t keyMODE = 8;
const uint8_t keyMUTE = 9;

const uint8_t LED_PIN = 4;
uint8_t status_LED = 4;

uint8_t screen = 1;

int pos = 0;
RotaryEncoder *encoder = nullptr;

void checkPosition() {
  encoder->tick(); // just call tick() to check the state.
}

void setupBRLC() {
  Serial.begin(115200);
  Consumer.begin();

  encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

  for (int i = 5; i <= 9; i++)
    pinMode(i, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void vISR_RotaryEncoder()  // This is a task.
{ 
  for (;;) {
    encoder->tick(); // just call tick() to check the state.

    int newPos = encoder->getPosition();
    int dir = (int)encoder->getDirection();
    if (pos != newPos && dir == 1) {
      Consumer.write(MEDIA_VOLUME_UP);
      pos = newPos;
    } else if (pos != newPos && dir == -1) {
      Consumer.write(MEDIA_VOLUME_DOWN);
      pos = newPos;
    }
    OS.delay(1);
  }
}

// LED task
void vLED_Task()
{
  for (;;) {
    OS.delay(1);
    switch (status_LED) {
      case 1:
        OS.delay(1000);
        digitalWrite(LED_PIN, LOW);
        OS.delay(1000);
        digitalWrite(LED_PIN, HIGH);
        break;
      case 2:
        OS.delay(100);
        digitalWrite(LED_PIN, LOW);
        OS.delay(100);
        digitalWrite(LED_PIN, HIGH);
        break;
      case 3:
        digitalWrite(LED_PIN, HIGH);
        break;
      case 4:
        digitalWrite(LED_PIN, LOW);
        break;
    }
  }
}

// Button task
void vButton_Task()
{
  for (;;) {
    OS.delay(1);
    if (digitalRead(keyNEXT) == LOW) {
      Consumer.press(MEDIA_NEXT);
      status_LED = 3;
      OS.delay(1);
      while (digitalRead(keyNEXT) == LOW) OS.delay(1);
      Consumer.release(MEDIA_NEXT);
      status_LED = 4;
    }
    if (digitalRead(keyPLAY) == LOW) {
      Consumer.press(MEDIA_PLAY_PAUSE);
      status_LED = 3;
      OS.delay(1);
      while (digitalRead(keyPLAY) == LOW) OS.delay(1);
      Consumer.release(MEDIA_PLAY_PAUSE);
      status_LED = 4;
    }
    if (digitalRead(keyPREV) == LOW) {
      Consumer.press(MEDIA_PREVIOUS);
      status_LED = 3;
      OS.delay(1);
      while (digitalRead(keyPREV) == LOW) OS.delay(1);
      Consumer.release(MEDIA_PREVIOUS);
      status_LED = 4;
    }
    if (digitalRead(keyMUTE) == LOW) {
      Consumer.press(MEDIA_VOLUME_MUTE);
      status_LED = 3;
      OS.delay(1);
      while (digitalRead(keyMUTE) == LOW) OS.delay(1);
      Consumer.release(MEDIA_VOLUME_MUTE);
      status_LED = 4;
    }
    if (digitalRead(keyMODE) == LOW) {
      status_LED = 3;
      OS.delay(1);
      while (digitalRead(keyMODE) == LOW) OS.delay(1);
      if  (screen < 3) screen ++;
      else screen = 1;
      mx.clear();
      OS.delay(50);
      status_LED = 4;
    }
  }
}

//ScreenTask
void vScreenTask()
{
  for (;;) {
    switch (screen) {
      case 1:
        spectrumAnalize_Screen();
        OS.delay(1);
        break;
      case 2:
        time_Screen();
        OS.delay(1);
        break;
      case 3:
        text_Screen();
        OS.delay(1);
        break;
    }
  }
}
