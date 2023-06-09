#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include "string.h"
#include "TFT_eSPI.h"

#include "pad.h"

#define KEYBOARD_COLOR_BACKGROUND TFT_DARKGREY
#define KEYBOARD_COLOR_BORDER TFT_WHITE

#define KEYBOARD_COLS 13
#define KEYBOARD_ROWS 5

#define NUMERIC_KEYBOARD_COLS 6
#define NUMERIC_KEYBOARD_ROWS 4

#define KEYBOARD_STRING_MAX_LENGTH 24

#define KEYBOARD_LETTER_TOP_PADDING 20
#define KEYBOARD_LETTER_LEFT_PADDING 20
#define KEYBOARD_TOP_PADDING 70
#define KEYBOARD_LEFT_PADDING 20

#define KEYBOARD_TEXT_AREA_TOP_PADDING 30
#define KEYBOARD_TEXT_AREA_HEIGHT 25

#define KEYBOARD_LETTER_WIDTH 12
#define KEYBOARD_LETTER_HEIGHT 16

typedef struct letter_struct {
	String uppercase = "";
	String lowercase = "";
	int color = TFT_WHITE;

	uint8_t key_width = 1;
	uint8_t key_height = 1;

	int8_t key_expanded_from_x = -1;
	int8_t key_expanded_from_y = -1;
	
} letter;

class KeyboardManager{
public:

  KeyboardManager(TFT_eSPI *display);
  ~KeyboardManager();

  void reset();

  void init();
	bool isSpecialLetter(uint8_t col, uint8_t row);
	void printKeyboard();
	void updateLetter(uint8_t col, uint8_t row, bool selected);
	void updateSpecialLetter(uint8_t col, uint8_t row, bool is_pressed);
	void printTextArea();
	

  void setLabel(String new_label);
	String displayKeyboard();

	String displayNumericKeyboard(bool mac_format = false);
	void printNumericKeyboard();
	void initNumeric();
private:
  TFT_eSPI *display_tft;

	bool shift_pressed = false;
  bool ctrl_pressed = false;
  bool alt_pressed = false;

	uint8_t shift_position[2] = {0,0};
	uint8_t ctrl_position[2] = {0,0};
	uint8_t alt_position[2] = {0,0};

	//array de struct de letras
	letter keyboard_letters[KEYBOARD_COLS][KEYBOARD_ROWS];
	String keyboard_string = "";
  String label = "";
};
#endif	/* KEYBOARD_MANAGER_H */
