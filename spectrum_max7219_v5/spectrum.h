#include <arduinoFFT.h>

arduinoFFT FFT = arduinoFFT();                                    // FFT object

#define SAMPLES       64  //Must be a power of 2
#define AUDIO_INPUT   0  // A0 pin
#define xres 32          // Total number of  columns in  the display, must be <= SAMPLES/2
#define yres 8           // Total number of  rows  in the display

uint8_t MY_ARRAY[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};  // default = standard pattern
uint8_t MY_MODE_1[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF}; // standard pattern
uint8_t MY_MODE_2[] = {0, 128, 64, 32, 16, 8, 4, 2, 1};            // only peak pattern
uint8_t MY_MODE_3[] = {0, 128, 192, 160, 144, 136, 132, 130, 129}; // only peak +  bottom point
uint8_t MY_MODE_4[] = {0, 128, 192, 160, 208, 232, 244, 250, 253}; // one gap in the top , 3rd light onwards
uint8_t MY_MODE_5[] = {0, 1, 3, 7, 15, 31, 63, 127, 255};          // standard pattern, mirrored vertically

double vReal[SAMPLES], vImag[SAMPLES];
char data_avgs[xres + 1];
uint8_t displaycolumn , displayvalue;
int peaks[xres + 1];
uint8_t band = 16;
uint8_t displaymode = 1;

void displayModeChange(uint8_t displaymode);

void  setupADC() {
  // ADC config
  ADCSRA = 0b11100101;      // set ADC to free running mode and set pre-scalar to 32 (500kHz)
  ADCSRB = bit(ADHSM);
  ADMUX = bit(REFS0) | bit(REFS1) | (analogPinToChannel(AUDIO_INPUT) & 0b00000111);
}

void spectrumAnalize_Screen() {
  // Sampling
  for (int  i = 0; i < SAMPLES; i++)
  {
    while (!(ADCSRA & 0b00010000));       // wait for  ADC to complete current conversion ie ADIF bit set
    ADCSRA = 0b11110101;                  // clear ADIF bit so that ADC can do next operation (0xf5)
    int value = ADC - 512;                // Read from ADC and subtract DC offset caused value
    vReal[i] = value / 8;                 // Copy to bins after compressing
    vImag[i] = 0;
  }
  //----------------------------------------------------
  // FFT
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  //----------------------------------------------------
  // re-arrange FFT result  to match with no. of columns on display ( xres )
  int step = (SAMPLES / 2) / xres;
  int c = 0;
  for (int i = 0; i <= (SAMPLES / 2); i += step)
  {
    data_avgs[c] = 0;
    for (int k = 0; k < step; k++) {
      data_avgs[c] = data_avgs[c] + vReal[i + k];
    }
    data_avgs[c] = data_avgs[c] / step;
    c++;
  }
  //----------------------------------------------------
  // send to display according measured value
  if (band == 16)
  {
    mx.control(0, MAX_DEVICES - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (int i = 1; i <= 16; i++)              // Remove noise at first data
    {
      data_avgs[i] = constrain(data_avgs[i], 0, 80);          // set max & min values for buckets
      data_avgs[i] = map(data_avgs[i], 0, 80, 0, yres);       // remap averaged values to yres

      peaks[i] = peaks[i] - 1;  // decay by one light
      if (data_avgs[i] > peaks[i]) peaks[i] = data_avgs[i];

      if (peaks[i] == 0)
        displayvalue = MY_ARRAY[1];
      else
        displayvalue = MY_ARRAY[peaks[i]];
      displaycolumn = xres - i * 2 + 1;
      mx.setColumn(displaycolumn, displayvalue);              //  for left to right
      displaycolumn = xres - i * 2;
      mx.setColumn(displaycolumn, displayvalue);              //  for left to right
    }
    mx.control(0, MAX_DEVICES - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  } else if (band == 32)
  {
    mx.control(0, MAX_DEVICES - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (int i = 1; i <= xres; i++)              // Remove noise at first data
    {
      data_avgs[i] = constrain(data_avgs[i], 0, 80);          // set max & min values for buckets
      data_avgs[i] = map(data_avgs[i], 0, 80, 0, yres);       // remap averaged values to yres

      peaks[i] = peaks[i] - 1;  // decay by one light
      if (data_avgs[i] > peaks[i]) peaks[i] = data_avgs[i];

      if (peaks[i] == 0)
        displayvalue = MY_ARRAY[1];
      else
        displayvalue = MY_ARRAY[peaks[i]];
      displaycolumn = xres - i;
      mx.setColumn(displaycolumn, displayvalue);              //  for left to right
    }
    mx.control(0, MAX_DEVICES - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  }
  //----------------------------------------------------
  appSerial();
}

void displayModeChange(uint8_t displaymode) {
  switch (displaymode) {
    case 1:
      for (int i = 0; i <= 8; i++) {
        MY_ARRAY[i] = MY_MODE_2[i];
      }
      break;
    case 2:
      for  (int i = 0; i <= 8; i++) {
        MY_ARRAY[i] = MY_MODE_3[i];
      }
      break;
    case 3:
      for  (int i = 0; i <= 8; i++) {
        MY_ARRAY[i] = MY_MODE_4[i];
      }
      break;
    case 4:
      for  (int i = 0; i <= 8; i++) {
        MY_ARRAY[i] = MY_MODE_5[i];
      }
      break;
    case 5:
      for (int i = 0; i <= 8; i++) {
        MY_ARRAY[i] = MY_MODE_1[i];
      }
      break;
  }
}
