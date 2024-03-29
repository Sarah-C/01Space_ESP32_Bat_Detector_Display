/*
   Frequency counter and display.
   
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
  
#include <Arduino.h>

// Input pin
#define freqInputPin GPIO_NUM_7

// Display update timer (not interupt driven)
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 250; // 1000 = 1 display update per second. So 250 is 4 updates a second.

// Display
#include <U8g2lib.h>
#include <Wire.h>
#define SDA_PIN 5
#define SCL_PIN 6
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42" OLED
volatile long unsigned int counter = 0;
char buf[10];

// Unsigned long to string
char *ultos_recursive(unsigned long val, char *s, unsigned radix, int pos) {
  int c;
  if (val >= radix)
    s = ultos_recursive(val / radix, s, radix, pos + 1);
  c = val % radix;
  c += (c < 10 ? '0' : 'a' - 10);
  *s++ = c;
  if (pos % 3 == 0) *s++ = ',';
  return s;
}

// Long to string
char *ltos(long val, char *s, int radix) {
  if (radix < 2 || radix > 36) {
    s[0] = 0;
  } else {
    char *p = s;
    if (radix == 10 && val < 0) {
      val = -val;
      *p++ = '-';
    }
    p = ultos_recursive(val, p, radix, 0) - 1;
    *p = 0;
  }
  return s;
}

void IRQcounter() {
  counter++;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.setFont(u8g2_font_10x20_me);
  u8g2.setFlipMode(1);
  u8g2.setContrast(1);
  startMillis = millis();
  attachInterrupt(8, IRQcounter, RISING);
}

void loop() {
  currentMillis = millis();  //get the current number of milliseconds since the program started
  if (currentMillis - startMillis >= period) { //test whether the period between updating the display has elapsee
    ltos((counter * 4), buf, 10);
    counter = 0;
    u8g2.clearBuffer();
    u8g2.drawStr(0, 24, buf);
    u8g2.sendBuffer();
    startMillis = currentMillis;
  }
}
