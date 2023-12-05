/* Frequency counter and display - early draft, I wouldn't use this one.

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   
*/

#include <Arduino.h>

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

// Display
#include <U8g2lib.h>
#include <Wire.h>
#define SDA_PIN 5
#define SCL_PIN 6
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42" OLED

#define freqInputPin GPIO_NUM_7
#define squareWavePin GPIO_NUM_8

volatile long unsigned int counter = 0;
long unsigned int counterBuf = 0;
char buf[10];

static void gpio_isr_handler(void* arg) {
  counter++;
}

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

void inputConfig() {
  gpio_install_isr_service(0);
  Serial.printf("Configuring input\n");
  gpio_reset_pin(freqInputPin);
  gpio_set_direction(freqInputPin, GPIO_MODE_INPUT);
  gpio_pullup_en(freqInputPin);
  gpio_set_intr_type(freqInputPin, GPIO_INTR_NEGEDGE);
  gpio_isr_handler_add(freqInputPin, gpio_isr_handler, NULL);
  Serial.printf("Config complete\n");
}

void outputConfig() {
  pinMode(squareWavePin, OUTPUT);
  analogWrite(squareWavePin, 128);
}
void displayConfig() {
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.setFont(u8g2_font_10x20_me);
  u8g2.setFlipMode(1);
  u8g2.setContrast(1);
}

void setup() {
  Serial.begin(115200);
  displayConfig();
  inputConfig();
  outputConfig();
}

void loop() {
  delay(238);

  counterBuf = counter * 4;
  counter=0;
 
  Serial.println(counterBuf);
  ltos(counterBuf, buf, 10);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 24, buf);
  u8g2.sendBuffer();

}




/*
  #include <Arduino.h>

  int squareWave = 9;
  int freqPin = 7;

  void setup() {
  Serial.begin(115200);
  Serial.print("Starting...");
  pinMode(freqPin, INPUT);
  FreqCount.begin(1000);
  pinMode(squareWave, OUTPUT);
  analogWrite(squareWave, 128);
  }

  char *ultos_recursive(unsigned long val, char *s, unsigned radix, int pos) { // Format an unsigned long (32 bits) into a string
  int c;
  if (val >= radix)
    s = ultos_recursive(val / radix, s, radix, pos + 1);
  c = val % radix;
  c += (c < 10 ? '0' : 'a' - 10);
   s++ = c;
  if (pos % 3 == 0) *s++ = ',';
  return s;
  }

  char *ltos(long val, char *s, int radix) {                                 // Format an long (32 bits) into a string
  if (radix < 2 || radix > 36) {
    s[0] = 0;
  } else {
    char *p = s;
    if (radix == 10 && val < 0) {
      val = -val;
       p++ = '-';
    }
    p = ultos_recursive(val, p, radix, 0) - 1;
     p = 0;
  }
  return s;
  }

  void increment() {
  x++;
  }

  void loop() {
  Serial.print(x);
  Serial.println("hz");
  x = 0;
  delay(990);
  }

*/
