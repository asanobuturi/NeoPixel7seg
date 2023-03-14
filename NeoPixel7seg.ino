#include "RTClib.h"
#include <Adafruit_NeoPixel.h>

#define LEDS_V 4 //縦のLED
#define LEDS_H 3 //横のLED

#define NUM_LEDS LEDS_V * 4 + LEDS_H * 3 //1つのパネル当たりのLEDの数

#define NUM_PANELS 4 //7セグのパネルの数

//NeoPixelのピン、ボタンのピン
#if NUM_PANELS == 4

#define PIN_LED0 2
#define PIN_LED1 3
#define PIN_LED2 4
#define PIN_LED3 5
#define PIN_UP 7
#define PIN_DOWN 6

Adafruit_NeoPixel led[4] = {
  {NUM_LEDS, PIN_LED0, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED1, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED2, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED3, NEO_GRB + NEO_KHZ800},
};

#elif NUM_PANELS == 6

#define PIN_LED0 2
#define PIN_LED1 3
#define PIN_LED2 4
#define PIN_LED3 5
#define PIN_LED4 6
#define PIN_LED5 7
#define PIN_UP 8
#define PIN_DOWN 9

Adafruit_NeoPixel led[6] = {
  {NUM_LEDS, PIN_LED0, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED1, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED2, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED3, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED4, NEO_GRB + NEO_KHZ800},
  {NUM_LEDS, PIN_LED5, NEO_GRB + NEO_KHZ800},
};

#else
#error The number of the panels must be 4 or 6!

#endif

//その他のピンの設定
#define PIN_S A0 //彩度
#define PIN_V A1 //明度

#define RAINBOW_CYCLE 2500 //虹色のサイクル(ms)

RTC_DS1307 rtc;

//数字とセグメントの対応(10->e 11->r 12->t)
const bool seg_num[13][7] = {
  {1, 1, 1, 0, 1, 1, 1},
  {0, 0, 1, 0, 0, 0, 1},
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 0, 0, 1},
  {1, 1, 0, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 1, 0, 0},
  {1, 0, 0, 1, 1, 1, 0},
};

//1桁の整数numに光らせるときにn番目のledを点灯させるのか
bool isNthLedLit( uint8_t num, uint8_t n ){
  uint8_t segment = 2 * floor(n / (LEDS_V + LEDS_H)) + ( n % (LEDS_V + LEDS_H) >= LEDS_V );
  return seg_num[num][segment];
}

//虹色に光らせるときのpanel面目のn番目のLEDの色相
uint16_t hueOfRainbow( uint8_t panel, uint8_t n ){
  uint8_t rowFromLeft = panel * (LEDS_H + 2) + max(0,min(LEDS_H + 1, (abs((2 * n + 2 * (LEDS_H + 1) + (LEDS_V - 1)) % (4 * (LEDS_H + LEDS_V)) - 2 * (LEDS_H + LEDS_V)) - LEDS_V + 1) / 2));
  uint16_t hue = (-RAINBOW_CYCLE / (NUM_PANELS * (LEDS_H * 2)) * rowFromLeft + millis()) % RAINBOW_CYCLE * 65536 / RAINBOW_CYCLE;
  return hue;
}

//panel面目のn番目のLEDの色相
uint16_t hueOfLed( uint8_t panel, uint8_t n, DateTime now ){
  uint16_t hue;
  if(now.minute() == 0 || now.minute() == 30){
    hue = hueOfRainbow(panel, n);
  }
  else{
    switch(NUM_PANELS - panel){
      case 6: hue = 65536 / 60 * now.hour(); break;
      case 5: hue = 65536 / 60 * now.hour(); break;
      case 4: hue = 65536 / 60 * now.minute(); break;
      case 3: hue = 65536 / 60 * now.minute(); break;
      case 2: hue = 65536 / 60 * now.second(); break;
      case 1: hue = 65536 / 60 * now.second(); break;
    }
  }
  return hue;
}

//時刻がnowのときのpanel面の数字
uint8_t numOfTime( uint8_t panel, DateTime now ){
  uint8_t num;
  switch(panel){
    case 0: num = now.hour() / 10; break;
    case 1: num = now.hour() % 10; break;
    case 2: num = now.minute() / 10; break;
    case 3: num = now.minute() % 10; break;
    case 4: num = now.second() / 10; break;
    case 5: num = now.second() % 10; break;
  }
  return num;
}

//時刻を表示する
void displayTime( DateTime now, uint8_t sat, uint8_t bri ){
  for(uint8_t ip = 0; ip < NUM_PANELS; ip++){
    for(uint8_t il = 0; il < NUM_LEDS; il++){
      if(isNthLedLit(numOfTime(ip, now), il) == true){
        led[ip].setPixelColor(il, led[ip].ColorHSV(hueOfLed(ip, il, now), sat, bri));
      }
      else{
        led[ip].setPixelColor(il, 0, 0, 0);
      }
    }
    led[ip].show();
  }
}

void setup(){
  //上下ボタンを設定
  pinMode(PIN_UP, INPUT);
  pinMode(PIN_DOWN, INPUT);

  //LEDテープを初期化
  for( uint8_t i = 0; i < 6; i++ )led[i].begin();

  //LEDテープが起動しなかったら
  if (! rtc.begin()) {
    while (1) delay(10);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop(){
  DateTime now = rtc.now();
  uint8_t sat = analogRead(PIN_S) / 4; //彩度
  uint8_t bri = analogRead(PIN_V) / 4; //明度
  displayTime(now, sat, bri);

  //上下ボタンによる時間調整
  if (digitalRead(PIN_UP) == HIGH){
    while(digitalRead(PIN_UP) == HIGH);//ボタンが離されるまで待つ
    rtc.adjust(DateTime(now.unixtime() + 60 - now.second()));
    }
  else if (digitalRead(PIN_DOWN) == HIGH){
    while(digitalRead(PIN_DOWN) == HIGH);//ボタンが離されるまで待つ
    rtc.adjust(DateTime(now.unixtime() - 60 - now.second()));
    }
}
