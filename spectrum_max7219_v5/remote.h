#include <IRremote.h> //IR lib

#define IR_RECEIVE_PIN      1
#define ENABLE_LED_FEEDBACK 1
#define FEEDBACK_LED_PIN    LED_PIN

#define FEEDBACK_LED_ON()   status_LED = 3;
#define FEEDBACK_LED_OFF()  status_LED = 4;

void setupIR()
{
//  pinMode(FEEDBACK_LED_PIN, OUTPUT);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, FEEDBACK_LED_PIN); // Start the receiver
//  printActiveIRProtocols(&Serial);
}

void vIR_task()
{
  for (;;) {
    if (IrReceiver.decode()) {
//      IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
      uint32_t data = IrReceiver.decodedIRData.decodedRawData;
//      Serial.println(data, HEX);
      switch (data) {
        case 0xFE01FF00:    // PREV
          Consumer.write(MEDIA_PREVIOUS);
          break;
        case 0xFD02FF00:    // PLAY
          Consumer.write(MEDIA_PLAY_PAUSE);
          break;
        case 0xFC03FF00:    // NEXT
          Consumer.write(MEDIA_NEXT);
          break;
        case 0xFB04FF00:    // V-DOW
          Consumer.write(MEDIA_VOLUME_DOWN);
          break;
        case 0xFA05FF00:    // MUTE
          Consumer.write(MEDIA_VOLUME_MUTE);
          break;
        case 0xF906FF00:    // V-UP
          Consumer.write(MEDIA_VOLUME_UP);
          break;
        case 0xE916FF00:    // MODE
          if  (screen < 2) screen ++;
          else screen = 1;
          mx.clear();
          OS.delay(50);
          break;
      }
      OS.delay(50);
      IrReceiver.resume();  // Enable receiving of the next value
    }
    OS.delay(1);
  }
}
