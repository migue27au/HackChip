#include "DisplayManager.h"

MenuItem::MenuItem(String utext, uint16_t ux, uint16_t uy, uint8_t usize, int ucolor, uint8_t ualign){
	text = utext;
	x = ux;
	y = uy;
	size = usize;
	color = ucolor;
	align = ualign;
}

MenuItem::~MenuItem(){}


DisplayManager::DisplayManager(TFT_eSPI *display){
	display_tft = display;

	display_tft->init();
	display_tft->setRotation(3);
	display_tft->setSwapBytes(true);
	display_tft->fillScreen(COLOR_BACKGROUND);

	display_tft->setTextColor(COLOR_DEFAULT);
	display_tft->setTextDatum(MC_DATUM);
}

DisplayManager::~DisplayManager(){
	
}

void DisplayManager::init(){
	display_tft->init();
	display_tft->setRotation(3);
	display_tft->setSwapBytes(true);
	display_tft->fillScreen(COLOR_BACKGROUND);

	display_tft->setTextColor(COLOR_DEFAULT);
	display_tft->setTextDatum(MC_DATUM);
}

void DisplayManager::clear(){
	display_tft->fillScreen(COLOR_BACKGROUND);
}

void DisplayManager::reset(){
	display_tft->fillScreen(COLOR_BACKGROUND);
}


void DisplayManager::addHeaderElement(String string, int color){
	MenuItem element = MenuItem(string, HEADER_ELEMENT_X, HEADER_ELEMENT_Y, HEADER_ELEMENT_SIZE, color, HEADER_ELEMENT_ALIGN);
	header_elements[header_elements_num] = element;
	header_elements_num++;
}

void DisplayManager::addRowElement(String string, int color){
	MenuItem element = MenuItem(string, ROW_ELEMENT_X, ROW_ELEMENT_Y_OFFSET, ROW_ELEMENT_SIZE, color, ROW_ELEMENT_ALIGN );
	row_elements[row_elements_num] = element;
	row_elements_num++;
}

void DisplayManager::addSubmenuElement(String string, int color){
	MenuItem element = MenuItem(string, SUBMENU_ELEMENT_X_OFFSET, SUBMENU_INFO_HEIGHT_OFFSET, 1, color, ML_DATUM );
	submenu_info_elements[submenu_info_elements_num] = element;
	submenu_info_elements_num++;
}

void DisplayManager::popSubmenuElement(){
	if(submenu_info_elements_num > 0){
		submenu_info_elements_num--;
	}
}

void DisplayManager::deleteSubmenuElements(){
	submenu_info_elements_num = 0;
}

void DisplayManager::addHeaders(String headers_str){
	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, headers_str);

	header_elements_num = 0;
	for(uint8_t col = 0; col < doc.size(); col++){
		String string_to_display = doc[col]["s"];
		int color_to_display = doc[col]["c"];
		
		addHeaderElement(string_to_display, color_to_display);
	}
}

void DisplayManager::addRows(String rows_str){
	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, rows_str);

	row_elements_num = 0;
	for(uint8_t row = 0; row < doc.size(); row++){
		String string_to_display = doc[row]["s"];
		int color_to_display = doc[row]["c"];
		
		addRowElement(string_to_display, color_to_display);
	}
}

void DisplayManager::showSelectors(uint8_t header_selector, uint8_t row_selector, uint8_t submenu_selector){
	/* HEADER SELECTOR */
	if(header_selector > 0){
		header_selector = (header_selector-1) % (HEADER_CELLS_SHOWED);
		if(row_selector == 0){
			display_tft->fillRect(HEADER_CELL_LEFT_OFFSET+(header_selector)*HEADER_CELL_WIDTH, 0, HEADER_CELL_WIDTH, HEADER_CELL_HEIGHT, COLOR_DEFAULT);
		} else {
			display_tft->fillRect(HEADER_CELL_LEFT_OFFSET+(header_selector)*HEADER_CELL_WIDTH, 0, HEADER_CELL_WIDTH, HEADER_CELL_HEIGHT, COLOR_LAYOUT);
		}
		display_tft->setTextColor(COLOR_BACKGROUND);
		display_tft->setTextDatum(header_elements[header_selector].align);
		display_tft->setTextSize(header_elements[header_selector].size);
		display_tft->drawString(header_elements[header_selector].text, 
								header_elements[header_selector].x + HEADER_CELL_WIDTH*(header_selector), 
								header_elements[header_selector].y);
	}

	/* ROW SELECTOR */
	display_tft->fillRect(0,ROW_CURSOR_Y_OFFSET-(LETTER_HEIGHT/2), ROW_ELEMENT_X, ROW_CELL_HEIGHT*5, COLOR_BACKGROUND);	//clear cursor
	if(row_selector > 0){
		row_selector = (row_selector-1) % (ROW_CELLS_SHOWED/cursor_size);
	
		display_tft->setTextColor(COLOR_DEFAULT);
		display_tft->setTextDatum(ROW_ELEMENT_ALIGN);
		display_tft->setTextSize(cursor_size);
		display_tft->drawString(ROW_CURSOR,		//character
								ROW_CURSOR_X,	//x offset
								ROW_CURSOR_Y_OFFSET + (cursor_size*LETTER_HEIGHT)*row_selector);	//y offset + jump(depends on letter size)	
	}
	
}

uint8_t DisplayManager::getMaxHeaders(){
	return header_elements_num;
}

uint8_t DisplayManager::getMaxRows(){
	return row_elements_num;
}

uint8_t DisplayManager::getMaxSubmenus(){
	return submenu_info_elements_num;
}

void DisplayManager::showMenuLayout(){
	
	/* HEADER */
	display_tft->setTextSize(1);
	display_tft->drawRect(0, HEADER_CELL_HEIGHT-HEADER_DOWN_BAR_HEIGTH, PIXELS_HORIZONTAL, HEADER_DOWN_BAR_HEIGTH, COLOR_LAYOUT);

	//header slots
	for(uint8_t header_col = 0; header_col < HEADER_CELLS_SHOWED; header_col++){
		display_tft->drawRect(HEADER_CELL_LEFT_OFFSET+header_col*HEADER_CELL_WIDTH, 0, HEADER_CELL_WIDTH, HEADER_CELL_HEIGHT, COLOR_LAYOUT);
	}

	/* SUBMENU INFO */
	display_tft->drawRect(0, SUBMENU_INFO_HEIGHT_OFFSET+SUBMENU_INFO_HEIGHT, PIXELS_HORIZONTAL, 1, COLOR_LAYOUT);
}

void DisplayManager::fillMenuHeaders(uint8_t header_selector){
	if(header_selector > 0){
		uint8_t col_init = ((header_selector-1)/HEADER_CELLS_SHOWED)*HEADER_CELLS_SHOWED;
		uint8_t col_last = col_init+HEADER_CELLS_SHOWED;
		if(col_last > header_elements_num){
			col_last = header_elements_num;
		}
		for(uint8_t header_col = col_init; header_col < col_last; header_col++){
			display_tft->setTextColor(header_elements[header_col].color);
			display_tft->setTextDatum(header_elements[header_col].align);
			display_tft->setTextSize(header_elements[header_col].size);
			
			display_tft->drawString(header_elements[header_col].text, 
									header_elements[header_col].x + HEADER_CELL_WIDTH*header_col, 
									header_elements[header_col].y);
		}
	}
}

void DisplayManager::fillMenuRows(uint8_t row_selector){
	cursor_size = 2;
	uint8_t row_init = (row_selector/(ROW_CELLS_SHOWED/cursor_size))*(ROW_CELLS_SHOWED/cursor_size);
	uint8_t row_last = row_init+(ROW_CELLS_SHOWED/cursor_size);
	if(row_last > row_elements_num){
		row_last = row_elements_num;
	}
	for(uint8_t row = row_init; row < row_last; row++){
		display_tft->setTextColor(row_elements[row].color, COLOR_BACKGROUND);
		display_tft->setTextDatum(row_elements[row].align);
		display_tft->setTextSize(row_elements[row].size);
		display_tft->drawString(row_elements[row].text, 
									row_elements[row].x,
									row_elements[row].y + (row_elements[row].size*LETTER_HEIGHT)*row);
	}
}


void DisplayManager::fillMenuSubmenu(){
	//clear
	display_tft->fillRect(0, SUBMENU_INFO_HEIGHT_OFFSET, PIXELS_HORIZONTAL, SUBMENU_INFO_HEIGHT, COLOR_BACKGROUND);
	if(submenu_info_elements_num > 0){
		display_tft->setTextSize(submenu_info_elements[0].size);
		display_tft->setTextColor(submenu_info_elements[0].color, COLOR_BACKGROUND);
		display_tft->setTextDatum(submenu_info_elements[0].align);

		String s = "";

		for(uint8_t element = 0; element < submenu_info_elements_num; element++){
			s += submenu_info_elements[element].text;
			if(element < submenu_info_elements_num-1){
				s += " > ";	
			}
		}
		display_tft->drawString(s, submenu_info_elements[0].x, submenu_info_elements[0].y+(static_cast<uint16_t>(LETTER_HEIGHT)*submenu_info_elements[0].size)/2);
	}
}

void DisplayManager::fillSSIDs(String json_str, uint8_t row_selector){
	StaticJsonDocument<4096> doc;
	
	DeserializationError error = deserializeJson(doc, json_str);
	cursor_size = 1;

	uint8_t row_init = ((row_selector-1)/(ROW_CELLS_SHOWED/cursor_size))*(ROW_CELLS_SHOWED/cursor_size);
	uint8_t row_last = row_init+(ROW_CELLS_SHOWED/cursor_size);

	if(row_last > doc.size()){
		row_last = doc.size();
	}

	display_tft->setTextSize(cursor_size);
	display_tft->setTextDatum(ROW_ELEMENT_ALIGN);
	//clear
	display_tft->fillRect(ROW_ELEMENT_X, ROW_ELEMENT_Y_OFFSET-(LETTER_HEIGHT/2), PIXELS_HORIZONTAL-ROW_ELEMENT_X,PIXELS_VERTICAL-ROW_ELEMENT_Y_OFFSET, COLOR_BACKGROUND);

	//						ssid 			security							strength signal
	uint16_t col_offset[] = {ROW_ELEMENT_X, ROW_ELEMENT_X+HEADER_CELL_WIDTH*4, ROW_ELEMENT_X+HEADER_CELL_WIDTH*5};


	for(uint8_t row = row_init; row < row_last; row++){
		//empiezo en 1 porque el cero es el mac del ap
		for(uint8_t col = 1; col < doc[row]["r"].size(); col++){
			String string_to_display = doc[row]["r"][col]["s"];
			int color_to_display = doc[row]["r"][col]["c"];
			
			display_tft->setTextColor(color_to_display);
			display_tft->drawString(string_to_display, 
									col_offset[col-1],
									ROW_ELEMENT_Y_OFFSET + static_cast<uint16_t>(LETTER_HEIGHT)*(row%(ROW_CELLS_SHOWED/cursor_size)));
		}
	}

	json_str = "";
	doc = NULL;
}

void DisplayManager::fillBSSIDs(String json_str, uint8_t row_selector){
	StaticJsonDocument<4096> doc;
	
	DeserializationError error = deserializeJson(doc, json_str);
	cursor_size = 1;

	uint8_t row_init = ((row_selector-1)/(ROW_CELLS_SHOWED/cursor_size))*(ROW_CELLS_SHOWED/cursor_size);
	uint8_t row_last = row_init+(ROW_CELLS_SHOWED/cursor_size);

	if(row_last > doc.size()){
		row_last = doc.size();
	}

	display_tft->setTextSize(cursor_size);
	display_tft->setTextDatum(ROW_ELEMENT_ALIGN);
	//clear
	display_tft->fillRect(ROW_ELEMENT_X, ROW_ELEMENT_Y_OFFSET-(LETTER_HEIGHT/2), PIXELS_HORIZONTAL-ROW_ELEMENT_X,PIXELS_VERTICAL-ROW_ELEMENT_Y_OFFSET, COLOR_BACKGROUND);

	//						ssid 			security							strength signal
	uint16_t col_offset[] = {ROW_ELEMENT_X, ROW_ELEMENT_X+HEADER_CELL_WIDTH*2, ROW_ELEMENT_X+HEADER_CELL_WIDTH*5};


	for(uint8_t row = row_init; row < row_last; row++){
		//empiezo en 1 porque el cero es el mac del ap
		for(uint8_t col = 0; col < 2; col++){
			String string_to_display = doc[row]["r"][col]["s"];
			int color_to_display = doc[row]["r"][col]["c"];
			
			display_tft->setTextColor(color_to_display);
			display_tft->drawString(string_to_display, 
									col_offset[col],
									ROW_ELEMENT_Y_OFFSET + static_cast<uint16_t>(LETTER_HEIGHT)*(row%(ROW_CELLS_SHOWED/cursor_size)));
		}
	}

	json_str = "";
	doc = NULL;
}

void DisplayManager::hello(){
	display_tft->fillScreen(COLOR_BACKGROUND);

	String s = "HackChip!";
	int colors[9] = {	TFT_RED,
						TFT_ORANGE,
						TFT_YELLOW,
						TFT_GREENYELLOW,
						TFT_GREEN,
						TFT_CYAN,
						TFT_BLUE,
						TFT_VIOLET,
						TFT_MAGENTA
					};
	display_tft->pushImage(1,1,170,170,logo);

	display_tft->setTextDatum(4);
	uint8_t text_size = 3;	
	display_tft->setTextSize(text_size);

	for(uint8_t c = 0; c < s.length(); c++){
		display_tft->setTextColor(colors[c]);
		display_tft->drawString(String(s.charAt(c)),
								((PIXELS_HORIZONTAL*3/4)-((s.length()/2)*text_size*LETTER_WIDTH))+(c*text_size*LETTER_WIDTH),
								PIXELS_VERTICAL/4);
	}

	display_tft->setTextSize(2);
	display_tft->setTextColor(TFT_WHITE);
	display_tft->drawString("Press ->",
								PIXELS_HORIZONTAL*3/4,
								PIXELS_VERTICAL*3/4);
	display_tft->drawString("to begin",
								PIXELS_HORIZONTAL*3/4,
								PIXELS_VERTICAL*3/4 + (LETTER_HEIGHT*3));
}

void DisplayManager::showAttackMessage(String title, String description){
	//clear
	display_tft->fillRect(ROW_ELEMENT_X, ROW_ELEMENT_Y_OFFSET-(LETTER_HEIGHT/2), PIXELS_HORIZONTAL-ROW_ELEMENT_X,PIXELS_VERTICAL-ROW_ELEMENT_Y_OFFSET, COLOR_BACKGROUND);

	display_tft->setTextSize(3);
	display_tft->setTextDatum(4);
	display_tft->setTextColor(TFT_RED);

	display_tft->drawString(title, 
							PIXELS_HORIZONTAL/2,
							PIXELS_VERTICAL/2);


	display_tft->setTextSize(1);
	display_tft->setTextDatum(3);
	display_tft->setTextColor(TFT_WHITE);

	display_tft->drawString(description, 
							ROW_ELEMENT_X,
							PIXELS_VERTICAL/2 + ROW_ELEMENT_Y_OFFSET);
}