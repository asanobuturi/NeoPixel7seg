#include "RTClib.h"
#include <FastLED.h>

#define NUM_LEDS 25 //1つのパネル当たりのLEDの数

#define LEDS_V 4 //縦のLED
#define LEDS_H 3 //横のLED

//それぞれのLEDのピン
#define PIN_LED0 2
#define PIN_LED1 3
#define PIN_LED2 4
#define PIN_LED3 5

//その他のピンの設定
#define PIN_UP 6
#define PIN_DOWN 7
#define PIN_S A0 //彩度
#define PIN_V A1 //明度

CRGB led0[NUM_LEDS];
CRGB led1[NUM_LEDS];
CRGB led2[NUM_LEDS];
CRGB led3[NUM_LEDS];

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//第led面のsegment番目のセグメントをcolor色で点灯
void lightSegment( CRGB led[NUM_LEDS] , CRGB color , uint8_t segment ){
  switch(segment){
    case 0:
      for(uint8_t i=0; i<LEDS_V; i++){
        led[LEDS_V*0 + LEDS_H*0 + i]=color;
        }
      FastLED.show();
      break;
    case 1:
      for(uint8_t i=0; i<LEDS_H; i++){
        led[LEDS_V*1 + LEDS_H*0 + i]=color;
        }
      FastLED.show();
      break;
    case 2:
      for(uint8_t i=0; i<LEDS_V; i++){
        led[LEDS_V*1 + LEDS_H*1 + i]=color;
        }
      FastLED.show();
      break;
    case 3:
      for(uint8_t i=0; i<LEDS_H; i++){
        led[LEDS_V*2 + LEDS_H*1 + i]=color;
        }
      FastLED.show();
      break;
    case 4:
      for(uint8_t i=0; i<LEDS_V; i++){
        led[LEDS_V*2 + LEDS_H*2 + i]=color;
        }
      FastLED.show();
      break;
    case 5:
      for(uint8_t i=0; i<LEDS_H; i++){
        led[LEDS_V*3 + LEDS_H*2 + i]=color;
        }
      FastLED.show();
      break;
    case 6:
      for(uint8_t i=0; i<LEDS_V; i++){
        led[LEDS_V*3 + LEDS_H*3 + i]=color;
        }
      FastLED.show();
      break;
    } 
  }

//数字とセグメントの対応(10->e 11->r 12->t)
const bool seg_num[13][7] = {
  {1,1,1,0,1,1,1},
  {0,0,1,0,0,0,1},
  {0,1,1,1,1,1,0},
  {0,1,1,1,0,1,1},
  {1,0,1,1,0,0,1},
  {1,1,0,1,0,1,1},
  {1,1,0,1,1,1,1},
  {1,1,1,0,0,0,1},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1},
  {1,1,1,1,1,1,0},
  {0,0,0,1,1,0,0},
  {1,0,0,1,1,1,0},
  };

//数字を表示
void lightNum( CRGB led[NUM_LEDS] , CRGB color , uint8_t num ){
  for(uint8_t i=0; i<7; i++){
      if(seg_num[num][i]==1){
        lightSegment(led,color,i);
        }
      else{
        lightSegment(led,CRGB::Black,i);
        }
    }
  }

void setup () {
  FastLED.addLeds<NEOPIXEL, PIN_LED0>(led0, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PIN_LED1>(led1, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PIN_LED2>(led2, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PIN_LED3>(led3, NUM_LEDS);

  if (! rtc.begin()) {
    lightNum(led1,CHSV(0,127,255),10);//e
    lightNum(led2,CHSV(0,127,255),11);//r
    lightNum(led3,CHSV(0,127,255),11);//r
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    lightNum(led1,CHSV(171,127,255),5);//s
    lightNum(led2,CHSV(171,127,255),10);//e
    lightNum(led3,CHSV(171,127,255),12);//t
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  DateTime now = rtc.now();
  uint8_t hsv_s = analogRead(PIN_S)/4;//彩度
  uint8_t hsv_v = analogRead(PIN_V)/4;//明度
  uint8_t hsv_h_h = 256/60*now.minute();//時の部分の色相
  uint8_t hsv_h_m = 256/60*now.second();//分の部分の色相
  uint8_t v0 = now.hour()/10;//0面
  uint8_t v1 = now.hour()%10;//1面
  uint8_t v2 = now.minute()/10;//2面
  uint8_t v3 = now.minute()%10;//3面
    
  lightNum(led0,CHSV(hsv_h_h,hsv_s,hsv_v),v0);//時の十の位
  lightNum(led1,CHSV(hsv_h_h,hsv_s,hsv_v),v1);//時の一の位
  lightNum(led2,CHSV(hsv_h_m,hsv_s,hsv_v),v2);//分の十の位
  lightNum(led3,CHSV(hsv_h_m,hsv_s,hsv_v),v3);//分の一の位
}
