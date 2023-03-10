#include <Arduino.h>

#include "effects.h"
#include "led.h"

#ifdef ARDUINO_ESP8266_ADAFRUIT_HUZZAH

// ESP8266 Feather pinout
#define LEDARRAY_CLA 4
#define LEDARRAY_CLK 5
#define LEDARRAY_DI 2
#define LEDARRAY_EN 16

#elif ARDUINO_ESP8266_NODEMCU_ESP12E

// ESP8266 NodeMCU ESP-12E pinout
#define LEDARRAY_CLA 0
#define LEDARRAY_CLK 4
#define LEDARRAY_DI 5
#define LEDARRAY_EN 16

#else

#error unknown board pinout

#endif

#define MASK_CLA (1 << LEDARRAY_CLA)
#define MASK_CLK (1 << LEDARRAY_CLK)
#define MASK_DI (1 << LEDARRAY_DI)

// ticks at TIM_DIV16 setting (16 * 1us / 80 = 0.2us)
// (minimum GPIO time is about 50-60us because each IO write takes 8 or so CPU
// cycles or 100ns, and we use two writes per pixel)
#define TIMER1_TICKS 300

// actual LED layout corresponding to the shift register queue is complex and
// snaking, this is the LUT
// see https://github.com/atesgoral/obegraensad-hack/blob/main/src/main.cpp#L41
const unsigned char positions[ROWS * COLS] = {
    // clang-format off
    0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x4f, 0x4e, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x8f, 0x8e, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x88, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xaf, 0xae, 0xad, 0xac, 0xab, 0xaa, 0xa9, 0xa8, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xcf, 0xce, 0xcd, 0xcc, 0xcb, 0xca, 0xc9, 0xc8, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0xc2, 0xc1, 0xc0, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    // clang-format on
};

unsigned char renderQueue[256]; // pixel buffer arranged by physical LED layout,
                                // with duty cycle values

unsigned int pwmStep = 0; // cycling through "stop points"

// per brightness level, how many frames are on out of an N-frame full duty
// cycle (note that the progression is very non-linear)
// full length is 256 to avoid weird memory access in case of over-limit pixel
// values
#define MAX_DUTY_FRAME 128
unsigned int pwmDutyCounts[256] = {0, 1, 3, 10, 25, 54, 72, MAX_DUTY_FRAME};

void ICACHE_RAM_ATTR onTimerISR() {
  const unsigned int frameDuty = pwmDutyCounts[pwmStep];

  // increment PWM step and overflow back to zero when we hit the max
  pwmStep++;
  const unsigned int nextFrameDuty = pwmDutyCounts[pwmStep];

  if (nextFrameDuty >= MAX_DUTY_FRAME) {
    pwmStep = 0;
  }

  // start countdown to next PWM duty cycle - note that this is variable
  // as the target brightness increases, and the
  // longer delays are also needed to allow other processing to happen
  timer1_write((nextFrameDuty - frameDuty) * TIMER1_TICKS);

  // render the frame
  for (int idx = 0; idx < ROWS * COLS; idx++) {
    const int pwmDuty = renderQueue[idx];

    // toggle on the frame if its PWM duty cycle is on
    // we combine data bit + CLK rising edge into one IO write to GPO
    GPO = ((frameDuty < pwmDuty) << LEDARRAY_DI) | MASK_CLK;
    GPOC = MASK_CLK; // fast direct write clear
  }

  GPOS = MASK_CLA; // fast direct write set
  GPOC = MASK_CLA; // fast direct write clear
}

void initLEDOutput() {
  // LED panel
  pinMode(LEDARRAY_CLA, OUTPUT);
  pinMode(LEDARRAY_CLK, OUTPUT);
  pinMode(LEDARRAY_DI, OUTPUT);
  pinMode(LEDARRAY_EN, OUTPUT);

  // initial state
  digitalWrite(LEDARRAY_CLA, LOW);
  digitalWrite(LEDARRAY_CLK, LOW);
  digitalWrite(LEDARRAY_DI, LOW);

  // turn on display
  digitalWrite(LEDARRAY_EN, LOW);

  // setup render loop
  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE,
                TIM_SINGLE); // restarted inside frameloop itself
  timer1_write(TIMER1_TICKS);
}

void updateRenderQueue(unsigned char *buffer) {
  // convert render buffer into raw physically-arranged PWM duty counts
  for (int pixel = 0; pixel < ROWS * COLS; pixel++) {
    const unsigned char pos = positions[pixel];
    const unsigned char value = buffer[pos] >> 5; // reduce to 3 bits

    const int pwmDuty = pwmDutyCounts[value];
    renderQueue[pixel] = pwmDuty;
  }
}
