#ifndef DISPLAY_H
#define DISPLAY_H

#include "String.h"
#include "TFT_eSPI.h"
#include <ArduinoJson.h>


TFT_eSPI display_tft= TFT_eSPI();

String display_fillDisplay_string = "";
char display_row_delimiter = ';';

char cursor_char = '>';

uint8_t top_padding = 5;
uint8_t left_padding = 20;
uint8_t cursor_padding = 5;

uint8_t display_text_size = 1;
uint8_t max_rows_to_show = 16;
uint8_t row_jump = 10;


void display_init(){
  display_tft.init();
  display_tft.setRotation(3);
  display_tft.setSwapBytes(true);
  display_tft.fillScreen(TFT_BLACK);

  display_tft.setTextColor(TFT_WHITE);
  display_tft.setTextDatum(4);
}

TFT_eSPI* display_returnTFTObject(){
  return &display_tft;
}

void display_clearDisplay(){
  display_fillDisplay_string = "";
  display_tft.fillScreen(TFT_BLACK);
}

void display_resetDisplay(){
  display_tft.fillScreen(TFT_BLACK);
}

void display_newData(String newString, unsigned int new_text_size = 2){
  display_fillDisplay_string = newString;
  display_text_size = new_text_size;

  //corrijo el tamaño de las celdas conrespecto al texto
  max_rows_to_show = 16/display_text_size;
  row_jump = 10*display_text_size;
  
  Serial.print("Display: ");
  Serial.println(display_fillDisplay_string);
  Serial.print("Text Size: ");
  Serial.println(display_text_size);
  newString = "";
}

unsigned display_getMaxRowsToShow(){
  return max_rows_to_show;
}

unsigned int display_countRows(){
  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, display_fillDisplay_string);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return 0;
  }
  
  return doc.size();
}

void display_fillWithNumbers(){
  while(true){
    String string = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    string += "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    string += "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    display_tft.setCursor(0,0);
    display_tft.setTextSize(1);
    display_tft.print(string);

  }
}

void display_fillDisplay(unsigned int display_cursor_position){  
  StaticJsonDocument<4096> doc;
  
  DeserializationError error = deserializeJson(doc, display_fillDisplay_string);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  unsigned int start_row = int(display_cursor_position/max_rows_to_show)*max_rows_to_show;
  unsigned int end_row = start_row+max_rows_to_show;
  
  if(end_row > doc.size()){
    end_row = doc.size();
  }

  display_tft.setTextSize(display_text_size);
  for(unsigned int row_index = start_row; row_index < end_row; row_index++){
    display_tft.setCursor(left_padding, top_padding + row_jump*(row_index%max_rows_to_show));
    
    for(unsigned int col_index = 0; col_index < doc[row_index]["r"].size(); col_index++){
      String string_to_display = doc[row_index]["r"][col_index]["s"];
      int color_to_display = doc[row_index]["r"][col_index]["c"];

      display_tft.setTextColor(color_to_display);

      display_tft.print(string_to_display);
      display_tft.print("  ");
    }
  }
  

  //imprimir cursor
  for(uint8_t i = 0; i < max_rows_to_show; i++){
    display_tft.setCursor(cursor_padding, top_padding + row_jump*i);
    display_tft.setTextColor(TFT_BLACK);
    display_tft.print(cursor_char);
  }
  display_tft.setTextColor(TFT_WHITE);
  display_tft.setCursor(cursor_padding, top_padding + row_jump*(display_cursor_position%max_rows_to_show));
  display_tft.print(cursor_char);
}

#endif  /* DISPLAY_H */
