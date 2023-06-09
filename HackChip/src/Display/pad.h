#ifndef PAD_H
#define PAD_H

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#define UI_UP 0
#define UI_LEFT 1
#define UI_DOWN 2
#define UI_RIGHT 3
#define UI_B 4
#define UI_A 5


Adafruit_ADS1115 ads;


bool pad[6] = {0,0,0,0,0,0};

void pad_checkUI(){
  int analog0 = ads.readADC_SingleEnded(0);
  int analog1 = ads.readADC_SingleEnded(1);
  int analog2 = ads.readADC_SingleEnded(2);
  int analog3 = ads.readADC_SingleEnded(3);

  /*Serial.printf("analog0: %d\r\n", analog0);
  Serial.printf("analog1: %d\r\n", analog1);
  Serial.printf("analog2: %d\r\n", analog2);
  Serial.printf("analog3: %d\r\n", analog3);*/

  if(analog3 >= 22500){
    pad[0] = false;
    pad[1] = false;
  } else if(analog3 >= 10000){
    pad[0] = true;
    pad[1] = false;
  } else if(analog3 >= 6750){
    pad[0] = false;
    pad[1] = true;
  } else {
    pad[0] = true;
    pad[1] = true;
  }
  
  if(analog2 >= 22500){
    pad[2] = false;
    pad[3] = false;
  } else if(analog2 >= 10000){
    pad[2] = true;
    pad[3] = false;
  } else if(analog2 >= 6750){
    pad[2] = false;
    pad[3] = true;
  } else {
    pad[2] = true;
    pad[3] = true;
  }
  
  if(analog0 >= 22500){
    pad[4] = false;
    pad[5] = false;
  } else if(analog0 >= 10000){
    pad[4] = false;
    pad[5] = true;
  } else if(analog0 >= 6750){
    pad[4] = true;
    pad[5] = false;
  } else {
    pad[4] = true;
    pad[5] = true;
  }
  
  //Serial.printf("pad> Button UP: %d\tButton LEFT: %d\tButton DOWN: %d\tButton RIGTH: %d\tButton B: %d\tButton A: %d\r\n", pad[0],pad[1],pad[2],pad[3],pad[4],pad[5]);
}

#endif  /* PAD_H */
