#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include "Arduino.h"
#include "string.h"
#include "TFT_eSPI.h"
#include <ArduinoJson.h>

#define PIXELS_HORIZONTAL 320
#define PIXELS_VERTICAL 170


#define COLOR_BACKGROUND TFT_BLACK
#define COLOR_DEFAULT TFT_WHITE
#define COLOR_LAYOUT TFT_DARKGREY

#define HEADER_CELL_HEIGHT 20

#define LETTER_WIDTH 6
#define LETTER_HEIGHT 8 + 2 //+2 because i want spacing
#define MAX_LETTERS_BY_COL 53
#define MAX_LETTERS_BY_ROW 21

#define HEADER_CELL_WIDTH 53
#define HEADER_CELLS_SHOWED 5
#define HEADER_CELL_LEFT_OFFSET 53/2  //26
#define HEADER_DOWN_BAR_HEIGTH 2

#define ROW_CELL_HEIGHT 53
#define ROW_CELLS_SHOWED 10
#define ROW_CELL_LEFT_OFFSET 53/2  //26

#define HEADER_ELEMENT_X 53  	//HEADER_CELL_WIDTH
#define HEADER_ELEMENT_Y 20/2 	//(HEADER_CELL_HEIGHT/2)
#define HEADER_ELEMENT_SIZE 1
#define HEADER_ELEMENT_ALIGN 4

#define MAX_HEADER_ELEMENTS 10

#define ROW_ELEMENT_X 53/2  	//HEADER_CELL_WIDTH
#define ROW_ELEMENT_Y_OFFSET 50	//(HEADER_CELL_HEIGHT/2)
#define ROW_ELEMENT_Y 16		//(HEADER_CELL_HEIGHT/2)
#define ROW_ELEMENT_SIZE 2
#define ROW_ELEMENT_ALIGN 3

#define ROW_CURSOR_X 53/4
#define ROW_CURSOR_Y_OFFSET 50
#define ROW_CURSOR ">"

#define MAX_ROW_ELEMENTS 10
#define MAX_SUBMENU_ELEMENTS 5

#define SUBMENU_ELEMENT_X_OFFSET 53/2  	//HEADER_CELL_WIDTH
#define SUBMENU_ELEMENT_Y_OFFSET 50	//(HEADER_CELL_HEIGHT/2)
#define SUBMENU_INFO_HEIGHT 10
#define SUBMENU_INFO_HEIGHT_OFFSET 20+2 //(HEADER_CELL_HEIGHT+HEADER_DOWN_BAR_HEIGTH)


class MenuItem{
public:
	MenuItem(String utext = "", uint16_t ux = 0, uint16_t uy = 0, uint8_t usize = 1, int ucolor = COLOR_DEFAULT, uint8_t ualign = MC_DATUM);
	~MenuItem();

	String text = "";
	uint16_t x = 0;
	uint16_t y = 0;
	uint8_t size = 1;
	int color = COLOR_DEFAULT;
	uint8_t align = MC_DATUM;
private:
};


class DisplayManager {
public:
	DisplayManager(TFT_eSPI *display);
	~DisplayManager();
	
	void addHeaderElement(String string, int color = COLOR_DEFAULT);
	void addRowElement(String string, int color = COLOR_DEFAULT);
	void addSubmenuElement(String string, int color = COLOR_DEFAULT);
	void popSubmenuElement();
	void deleteSubmenuElements();

	void addHeaders(String headers_str);
	void addRows(String rows_str);

	void setSelectorSize(uint8_t new_cursor_size = 2);
	void showSelectors(uint8_t header_selector, uint8_t row_selector, uint8_t submenu_selector);

	uint8_t getMaxRows();
	uint8_t getMaxHeaders();
	uint8_t getMaxSubmenus();

	void init();
	void clear();
	void reset();

	void showMenuLayout();
	void fillMenuHeaders(uint8_t header_selector = 0);
	void fillMenuRows(uint8_t row_selector = 0);
	void fillMenuSubmenu();

	void hello();
	void fillSSIDs(String json_str, uint8_t row_selector = 0);
	void fillBSSIDs(String json_str, uint8_t row_selector = 0);
private:
	TFT_eSPI *display_tft;

	MenuItem header_elements[MAX_HEADER_ELEMENTS];
	uint8_t header_elements_num = 0;
	MenuItem row_elements[MAX_ROW_ELEMENTS];
	uint8_t row_elements_num = 0;
	MenuItem submenu_info_elements[MAX_SUBMENU_ELEMENTS];
	uint8_t submenu_info_elements_num = 0;

	uint8_t cursor_size = 2;
};


#endif //DISPLAYMANAGER_H