#include "KeyboardManager.h"

KeyboardManager::KeyboardManager(TFT_eSPI *display){
	display_tft = display;
}

KeyboardManager::~KeyboardManager(){
	
}

void KeyboardManager::reset(){
	for(uint8_t row = 0; row < KEYBOARD_ROWS; row++){
		for(uint8_t col = 0; col < KEYBOARD_COLS; col++){
			keyboard_letters[col][row].uppercase = "";
			keyboard_letters[col][row].lowercase = "";
			keyboard_letters[col][row].color = TFT_WHITE;
			keyboard_letters[col][row].key_width = 1;
			keyboard_letters[col][row].key_height = 1;
			keyboard_letters[col][row].key_expanded_from_x = -1;
			keyboard_letters[col][row].key_expanded_from_y = -1;
		}
	}
}

void KeyboardManager::init(){
	String rows[4] = {	"1234567890'¡",
						"qwertyuiop+[",
						"asdfghjklñ|{",
						"<zxcvbnm,.-@"};
	String ROWS[4] = {	"!\"·$%&/()=?¿",
						"QWERTYUIOP*]",
						"ASDFGHJKLÑ|}",
						">ZXCVBNM;:_#"};
	
	for(uint8_t row = 0; row < 4; row++){
		for(uint8_t col = 0; col < 12; col++){
			keyboard_letters[col][row].lowercase = String(rows[row].charAt(col));
			keyboard_letters[col][row].uppercase = String(ROWS[row].charAt(col));
			keyboard_letters[col][row].key_width = 1;
			keyboard_letters[col][row].key_height = 1;
		}
	}

	//special_letters
	//ALT
	alt_position[0] = 0;
	alt_position[1] = 4;
	keyboard_letters[0][4].lowercase = "ALT";
	keyboard_letters[0][4].uppercase = "ALT";
	keyboard_letters[0][4].key_width = 3;
	keyboard_letters[0][4].key_height = 1;
	for(uint8_t i = 0; i < keyboard_letters[0][4].key_width; i++){
		keyboard_letters[0+i][4].key_expanded_from_x = 0;
		keyboard_letters[0+i][4].key_expanded_from_y = 4;
	}

	//SPACE
	keyboard_letters[3][4].lowercase = "-";
	keyboard_letters[3][4].uppercase = "-";
	keyboard_letters[3][4].key_width = 6;
	keyboard_letters[3][4].key_height = 1;
	for(uint8_t i = 0; i < keyboard_letters[3][4].key_width; i++){
		keyboard_letters[3+i][4].key_expanded_from_x = 3;
		keyboard_letters[3+i][4].key_expanded_from_y = 4;
	}

	//MAYUS
	shift_position[0] = 9;
	shift_position[1] = 4;
	keyboard_letters[9][4].lowercase = "MAYU";
	keyboard_letters[9][4].uppercase = "MAYU";
	keyboard_letters[9][4].key_width = 3;
	keyboard_letters[9][4].key_height = 1;
	for(uint8_t i = 0; i < keyboard_letters[9][4].key_width; i++){
		keyboard_letters[9+i][4].key_expanded_from_x = 9;
		keyboard_letters[9+i][4].key_expanded_from_y = 4;
	}
	
	//CTRL
	ctrl_position[0] = 12;
	ctrl_position[1] = 4;
	keyboard_letters[12][4].lowercase = "CTRL";
	keyboard_letters[12][4].uppercase = "CTRL";
	keyboard_letters[12][4].key_width = 3;
	keyboard_letters[12][4].key_height = 1;
	for(uint8_t i = 0; i < keyboard_letters[12][4].key_width; i++){
		keyboard_letters[12+i][4].key_expanded_from_x = 12;
		keyboard_letters[12+i][4].key_expanded_from_y = 4;
	}
	
	//RETR
	keyboard_letters[12][0].lowercase = "<-";
	keyboard_letters[12][0].uppercase = "<-";
	keyboard_letters[12][0].key_width = 3;
	keyboard_letters[12][0].key_height = 1;

	//ENTR
	keyboard_letters[12][1].lowercase = "ENT";
	keyboard_letters[12][1].uppercase = "ENT";
	keyboard_letters[12][1].key_width = 3;
	keyboard_letters[12][1].key_height = 2;
	for(uint8_t i = 0; i < keyboard_letters[12][1].key_height; i++){
		keyboard_letters[12][1+i].key_expanded_from_x = 12;
		keyboard_letters[12][1+i].key_expanded_from_y = 1;
	}

	//QUIT
	keyboard_letters[12][3].lowercase = "QUIT";
	keyboard_letters[12][3].uppercase = "QUIT";
	keyboard_letters[12][3].key_width = 3;
	keyboard_letters[12][3].key_height = 1;
	keyboard_letters[12][3].color = TFT_RED;
}

bool KeyboardManager::isSpecialLetter(uint8_t col, uint8_t row){
	if(shift_pressed && col == shift_position[0] && row == shift_position[1]){
		return true;
	} else if(ctrl_pressed && col == ctrl_position[0] && row == ctrl_position[1]){
		return true;
	} else if(alt_pressed && col == alt_position[0] && row == alt_position[1]){
		return true;
	} else {
		return false;
	}
}

void KeyboardManager::printKeyboard(){
	display_tft->fillScreen(KEYBOARD_COLOR_BACKGROUND);
	display_tft->setTextSize(2);
	display_tft->setTextDatum(4);
	
	for(uint8_t row = 0; row < KEYBOARD_ROWS; row++){
		for(uint8_t col = 0; col < KEYBOARD_COLS; col++){
			//si la letra está definida
			if(keyboard_letters[col][row].lowercase.length() > 0){
				//si no es una letra especial cuando está activados los cambios después de mostrar por primera vez
				String letter_to_print = String(keyboard_letters[col][row].lowercase);
				if(shift_pressed){
					letter_to_print = String(keyboard_letters[col][row].uppercase);
				}
				unsigned int x = KEYBOARD_LETTER_LEFT_PADDING*col + KEYBOARD_LEFT_PADDING;
				unsigned int y = KEYBOARD_LETTER_TOP_PADDING*row + KEYBOARD_TOP_PADDING;
				unsigned int x_rect = x - (int)KEYBOARD_LETTER_LEFT_PADDING/2;
				unsigned int y_rect = y - (int)KEYBOARD_LETTER_TOP_PADDING/2;

				if(isSpecialLetter(col,row)){
					display_tft->fillRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BORDER);
					display_tft->setTextColor(KEYBOARD_COLOR_BACKGROUND);
				} else {
					display_tft->drawRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, keyboard_letters[col][row].color);
					display_tft->setTextColor(keyboard_letters[col][row].color);
				}
				int central_x = - (KEYBOARD_LETTER_LEFT_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_LEFT_PADDING)*keyboard_letters[col][row].key_width)/2;
				int central_y = - (KEYBOARD_LETTER_TOP_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_TOP_PADDING)*keyboard_letters[col][row].key_height)/2;

				display_tft->drawString(letter_to_print, x+central_x, y+central_y);
			}
		}
	}	
}


void KeyboardManager::updateLetter(uint8_t col, uint8_t row, bool selected){
	String letter_to_print = keyboard_letters[col][row].lowercase;
	if(shift_pressed){
		letter_to_print = String(keyboard_letters[col][row].uppercase);
	}

	unsigned int x = KEYBOARD_LETTER_LEFT_PADDING*col + KEYBOARD_LEFT_PADDING;
	unsigned int y = KEYBOARD_LETTER_TOP_PADDING*row + KEYBOARD_TOP_PADDING;
	unsigned int x_rect = x - (int)KEYBOARD_LETTER_LEFT_PADDING/2;
	unsigned int y_rect = y - (int)KEYBOARD_LETTER_TOP_PADDING/2;

	
	unsigned int color = keyboard_letters[col][row].color;
	
	//las teclas especiales (shift, ctrl y alt) se gestionan a parte
	if(isSpecialLetter(col, row) == false){
		if(selected){
			Serial.printf("Keyboard> key: %d %d\r\n", col, row);
			display_tft->fillRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BORDER);
			color = KEYBOARD_COLOR_BACKGROUND;
		} else {
			display_tft->fillRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BACKGROUND);
			display_tft->drawRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BORDER);		
		}
		int central_x = - (KEYBOARD_LETTER_LEFT_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_LEFT_PADDING)*keyboard_letters[col][row].key_width)/2;
		int central_y = - (KEYBOARD_LETTER_TOP_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_TOP_PADDING)*keyboard_letters[col][row].key_height)/2;
		display_tft->setTextDatum(4);
		display_tft->setTextColor(color);
		display_tft->drawString(letter_to_print, x+central_x, y+central_y);
	}
}

void KeyboardManager::updateSpecialLetter(uint8_t col, uint8_t row, bool is_pressed){
	//SHIFT
	String letter_to_print = keyboard_letters[col][row].lowercase;
	
	unsigned int x = KEYBOARD_LETTER_LEFT_PADDING*col + KEYBOARD_LEFT_PADDING;
	unsigned int y = KEYBOARD_LETTER_TOP_PADDING*row + KEYBOARD_TOP_PADDING;
	unsigned int x_rect = x - (int)KEYBOARD_LETTER_LEFT_PADDING/2;
	unsigned int y_rect = y - (int)KEYBOARD_LETTER_TOP_PADDING/2;
	
	unsigned int color = keyboard_letters[col][row].color;

	//las teclas especiales (shift, ctrl y alt) se gestionan a parte
	
	if(is_pressed){
		Serial.printf("key: %d %d\r\n", col, row);
		display_tft->fillRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BORDER);
		color = KEYBOARD_COLOR_BACKGROUND;
	} else {
		display_tft->fillRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BACKGROUND);
		display_tft->drawRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, keyboard_letters[col][row].color);
	}
	display_tft->setTextDatum(4);
	display_tft->setTextColor(color);
	int central_x = - (KEYBOARD_LETTER_LEFT_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_LEFT_PADDING)*keyboard_letters[col][row].key_width)/2;
	int central_y = - (KEYBOARD_LETTER_TOP_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_TOP_PADDING)*keyboard_letters[col][row].key_height)/2;

	display_tft->drawString(letter_to_print, x+central_x, y+central_y);
}

void KeyboardManager::printTextArea(){
	unsigned int text_area_width = 0;

	for(uint8_t col = 0; col < KEYBOARD_COLS; col++){
		text_area_width += KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][0].key_width;
	}

	String sub_line = "";
	for(uint8_t l = 0; l < keyboard_string.length(); l++){
		sub_line += "_";
	}

	display_tft->setTextDatum(3);

	display_tft->fillRect(0, 0, 320, KEYBOARD_LETTER_HEIGHT+KEYBOARD_LETTER_HEIGHT/2, KEYBOARD_COLOR_BACKGROUND);
	display_tft->setCursor(KEYBOARD_LEFT_PADDING/2,KEYBOARD_LETTER_HEIGHT/2);
	display_tft->setTextColor(KEYBOARD_COLOR_BORDER);
	display_tft->print(label);

	//borro
	display_tft->fillRect(KEYBOARD_LEFT_PADDING/2, KEYBOARD_TEXT_AREA_TOP_PADDING, text_area_width, KEYBOARD_TEXT_AREA_HEIGHT, KEYBOARD_COLOR_BACKGROUND);
	//borde
	display_tft->drawRect(KEYBOARD_LEFT_PADDING/2, KEYBOARD_TEXT_AREA_TOP_PADDING, text_area_width, KEYBOARD_TEXT_AREA_HEIGHT, KEYBOARD_COLOR_BORDER);
	
	//string
	display_tft->setTextColor(KEYBOARD_COLOR_BORDER);
	display_tft->setCursor(KEYBOARD_LEFT_PADDING, KEYBOARD_TEXT_AREA_TOP_PADDING+KEYBOARD_LETTER_HEIGHT/4);
	display_tft->print(keyboard_string);
	
	//barras _ de caracteres escritos	
	display_tft->setTextColor(KEYBOARD_COLOR_BACKGROUND);
	//mas un _ para borrar el siguiente caracter si hace falta, y + 5 para que sea subrayado
	display_tft->setCursor(KEYBOARD_LEFT_PADDING, KEYBOARD_TEXT_AREA_TOP_PADDING+KEYBOARD_LETTER_HEIGHT/4 + 5);
	display_tft->print(sub_line+"_");
	
	display_tft->setTextColor(KEYBOARD_COLOR_BORDER);
	display_tft->setCursor(KEYBOARD_LEFT_PADDING, KEYBOARD_TEXT_AREA_TOP_PADDING+KEYBOARD_LETTER_HEIGHT/4 + 5);
	display_tft->print(sub_line);
}

void KeyboardManager::setLabel(String new_label){
	label = new_label;
}

String KeyboardManager::displayKeyboard(){
	uint8_t selected_letter[2] = {0,0};
	keyboard_string = "";
	
	shift_pressed = false; ctrl_pressed = false; alt_pressed = false;
	
	reset();
	init();
	
	printKeyboard();
	updateLetter(0, 0, true);
	printTextArea();
	
	bool keyboard_enable = true;
	bool keyboard_text_finished = false;
	while(keyboard_enable){
		pad_checkUI();
		
		if(pad[UI_B]){
			//si hay texto escrito lo borras
			if(keyboard_string.length() > 0){
				keyboard_string = keyboard_string.substring(0,keyboard_string.length()-1);
				printTextArea();
			//si no, sales
			} else {
				keyboard_enable = false;
			}
			
			delay(100);
		}

		if(pad[UI_A]){
			//SHIFT
			if(shift_position[0] == selected_letter[0] && shift_position[1] == selected_letter[1]){
				shift_pressed = !shift_pressed;
				printKeyboard();
				updateSpecialLetter(shift_position[0], shift_position[1], shift_pressed);
			}
			//CTRL
			else if(ctrl_position[0] == selected_letter[0] && ctrl_position[1] == selected_letter[1]){
				ctrl_pressed = !ctrl_pressed;
				printKeyboard();
				updateSpecialLetter(ctrl_position[0], ctrl_position[1], ctrl_pressed);
			}
			//ALT
			else if(alt_position[0] == selected_letter[0] && alt_position[1] == selected_letter[1]){
				alt_pressed = !alt_pressed;
				printKeyboard();
				updateSpecialLetter(alt_position[0], alt_position[1], alt_pressed);
			} else {
				if(selected_letter[0] < 12 && selected_letter[1] < 4 && keyboard_string.length() < KEYBOARD_STRING_MAX_LENGTH){
				//LETTER
					if(shift_pressed){
						keyboard_string += keyboard_letters[selected_letter[0]][selected_letter[1]].uppercase;
					} else {
						keyboard_string += keyboard_letters[selected_letter[0]][selected_letter[1]].lowercase;
					}
				//ESPACE
				} else if(selected_letter[0] == 3 && selected_letter[1] == 4 && keyboard_string.length() < KEYBOARD_STRING_MAX_LENGTH){
					keyboard_string += " ";
				//RETR
				} else if(selected_letter[0] == 12 && selected_letter[1] == 0){
					keyboard_string = keyboard_string.substring(0,keyboard_string.length()-1);
				//QUIT
				} else if(selected_letter[0] == 12 && selected_letter[1] == 3){
					keyboard_enable = false;
				//ENTER
				} else if(selected_letter[0] == 12 && selected_letter[1] == 1){
					keyboard_enable = false;
					keyboard_text_finished = true;
				}
			}
			//actualizo el texto
			printTextArea();
			delay(100);
		}

		uint8_t prev_selected_letter[2] = {selected_letter[0], selected_letter[1]};
	
		if(pad[UI_DOWN]){
			if(selected_letter[1] < KEYBOARD_ROWS-1){
				selected_letter[1] = prev_selected_letter[1] + keyboard_letters[prev_selected_letter[0]][prev_selected_letter[1]].key_height;
			}			
		}

		if(pad[UI_UP]){
			if(selected_letter[1] > 0){
				selected_letter[1]--;
			}
		}

		if(pad[UI_RIGHT]){
			if(selected_letter[0] < KEYBOARD_COLS-1){
				selected_letter[0] = prev_selected_letter[0] + keyboard_letters[prev_selected_letter[0]][prev_selected_letter[1]].key_width;
			}
		}

		if(pad[UI_LEFT]){
			if(selected_letter[0] > 0){
				selected_letter[0]--;
			}
		}

		//correciones teclas especiales ultima columna. Ocurre cuando se resta el valor de selected letter
		if(keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_x != -1 && keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_y != -1){
			uint8_t new_x_value = keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_x;
			uint8_t new_y_value = keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_y;
			selected_letter[0] = new_x_value;
			selected_letter[1] = new_y_value;
		}

		if(selected_letter[0] != prev_selected_letter[0] || selected_letter[1] != prev_selected_letter[1]){
			//desactivo la anterior
			updateLetter(prev_selected_letter[0], prev_selected_letter[1], false);
			//activo la nueva
			updateLetter(selected_letter[0], selected_letter[1], true);
			delay(100);
		}
	
	}//while

	if(keyboard_text_finished){
		return keyboard_string;
	} else {
		return "";
	}
}


String KeyboardManager::displayNumericKeyboard(bool mac_format){
	uint8_t selected_letter[2] = {0,0};
	keyboard_string = "";
	
	reset();	
	initNumeric();
	
	printNumericKeyboard();
	updateLetter(0, 0, true);
	printTextArea();
	
	uint8_t max_string_length = KEYBOARD_STRING_MAX_LENGTH;
	if(mac_format) {
		max_string_length = 17;
	}

	bool keyboard_enable = true;
	bool keyboard_text_finished = false;
	while(keyboard_enable){
		pad_checkUI();

		if(pad[UI_B]){
			//si hay texto escrito lo borras
			if(keyboard_string.length() > 0){
				keyboard_string = keyboard_string.substring(0,keyboard_string.length()-1);
				
				//si es el formato mac y la ultima letra es ":" la quitas
				if(mac_format == true && keyboard_string.charAt(keyboard_string.length()-1) == ':'){
					keyboard_string = keyboard_string.substring(0,keyboard_string.length()-1);	
				}

				printTextArea();
			//si no, sales
			} else {
				keyboard_enable = false;
			}
			
			delay(100);
		}

		if(pad[UI_A]){
			if(selected_letter[0] < 5 && selected_letter[1] < 4 && keyboard_string.length() < max_string_length){
			//LETTER
				//si es el formato mac y la longitud coincide con la que corresponde a : lo pongo
				if(mac_format == true && keyboard_string.length() > 1  && keyboard_string.length() < max_string_length && (keyboard_string.length()-2)%3 == 0){
					keyboard_string += ":";	
				}
				keyboard_string += keyboard_letters[selected_letter[0]][selected_letter[1]].lowercase;
				//si es el formato mac y la longitud coincide con la que corresponde a : lo pongo
				if(mac_format == true && keyboard_string.length() > 1  && keyboard_string.length() < max_string_length && (keyboard_string.length()-2)%3 == 0){
					keyboard_string += ":";	
				}

			//RETR
			} else if(selected_letter[0] == 5 && selected_letter[1] == 0){
				keyboard_string = keyboard_string.substring(0,keyboard_string.length()-1);
			//QUIT
			} else if(selected_letter[0] == 5 && selected_letter[1] == 3){
				keyboard_enable = false;
			//ENTER
			} else if(selected_letter[0] == 5 && selected_letter[1] == 1){
				keyboard_enable = false;
				keyboard_text_finished = true;
			}
			//actualizo el texto
			printTextArea();
			delay(100);
		}

		uint8_t prev_selected_letter[2] = {selected_letter[0], selected_letter[1]};

		if(pad[UI_DOWN]){
			if(selected_letter[1] < NUMERIC_KEYBOARD_ROWS-1){
				selected_letter[1] = prev_selected_letter[1] + keyboard_letters[prev_selected_letter[0]][prev_selected_letter[1]].key_height;
			}	
		}

		if(pad[UI_UP]){
			if(selected_letter[1] > 0){
				selected_letter[1]--;
			}
		}

		if(pad[UI_RIGHT]){
			if(selected_letter[0] < NUMERIC_KEYBOARD_COLS-1){
				selected_letter[0] = prev_selected_letter[0] + keyboard_letters[prev_selected_letter[0]][prev_selected_letter[1]].key_width;
			}
		}

		if(pad[UI_LEFT]){
			if(selected_letter[0] > 0){
				selected_letter[0]--;
			}
		}

		//correciones teclas especiales ultima columna. Ocurre cuando se resta el valor de selected letter
		if(keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_x != -1 && keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_y != -1){
			uint8_t new_x_value = keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_x;
			uint8_t new_y_value = keyboard_letters[selected_letter[0]][selected_letter[1]].key_expanded_from_y;
			selected_letter[0] = new_x_value;
			selected_letter[1] = new_y_value;
		}

		if(selected_letter[0] != prev_selected_letter[0] || selected_letter[1] != prev_selected_letter[1]){
			//desactivo la anterior
			updateLetter(prev_selected_letter[0], prev_selected_letter[1], false);
			//activo la nueva
			updateLetter(selected_letter[0], selected_letter[1], true);
			delay(100);
		}
	}//while

	if(keyboard_text_finished){
		return keyboard_string;
	} else {
		return "";
	}
}

void KeyboardManager::printNumericKeyboard(){
	display_tft->fillScreen(KEYBOARD_COLOR_BACKGROUND);
	display_tft->setTextSize(2);
	display_tft->setTextDatum(4);
	
	for(uint8_t row = 0; row < NUMERIC_KEYBOARD_ROWS; row++){
		for(uint8_t col = 0; col < NUMERIC_KEYBOARD_COLS; col++){
			//si la letra está definida
			if(keyboard_letters[col][row].lowercase.length() > 0){
				//si no es una letra especial cuando está activados los cambios después de mostrar por primera vez
				String letter_to_print = String(keyboard_letters[col][row].lowercase);
				if(shift_pressed){
					letter_to_print = String(keyboard_letters[col][row].uppercase);
				}
				unsigned int x = KEYBOARD_LETTER_LEFT_PADDING*col + KEYBOARD_LEFT_PADDING;
				unsigned int y = KEYBOARD_LETTER_TOP_PADDING*row + KEYBOARD_TOP_PADDING;
				unsigned int x_rect = x - (int)KEYBOARD_LETTER_LEFT_PADDING/2;
				unsigned int y_rect = y - (int)KEYBOARD_LETTER_TOP_PADDING/2;

				if(isSpecialLetter(col,row)){
					display_tft->fillRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, KEYBOARD_COLOR_BORDER);
					display_tft->setTextColor(KEYBOARD_COLOR_BACKGROUND);
				} else {
					display_tft->drawRect(x_rect, y_rect, KEYBOARD_LETTER_LEFT_PADDING*keyboard_letters[col][row].key_width, KEYBOARD_LETTER_TOP_PADDING*keyboard_letters[col][row].key_height, keyboard_letters[col][row].color);
					display_tft->setTextColor(keyboard_letters[col][row].color);
				}
				int central_x = - (KEYBOARD_LETTER_LEFT_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_LEFT_PADDING)*keyboard_letters[col][row].key_width)/2;
				int central_y = - (KEYBOARD_LETTER_TOP_PADDING/2) + (static_cast<uint16_t>(KEYBOARD_LETTER_TOP_PADDING)*keyboard_letters[col][row].key_height)/2;

				display_tft->drawString(letter_to_print, x+central_x, y+central_y);
			}
		}
	}	
}

void KeyboardManager::initNumeric(){
	String rows[4] = {	"AB789",
						"CD456",
						"EF123",
						":.,0"};
	
	for(uint8_t row = 0; row < 4; row++){
		for(uint8_t col = 0; col < rows[row].length(); col++){
			keyboard_letters[col][row].lowercase = String(rows[row].charAt(col));
			keyboard_letters[col][row].uppercase = String(rows[row].charAt(col));
			
			
			if(keyboard_letters[col][row].lowercase == "0"){
				keyboard_letters[col][row].key_width = 2;
			} else {
				keyboard_letters[col][row].key_width = 1;
			}

			keyboard_letters[col][row].key_height = 1;
		}
	}

	//0 el cero ocupa dos espacios
	keyboard_letters[4][3].key_expanded_from_x = 3;
	keyboard_letters[4][3].key_expanded_from_y = 3;
	
	//RETR
	keyboard_letters[5][0].lowercase = "<-";
	keyboard_letters[5][0].uppercase = "<-";
	keyboard_letters[5][0].key_width = 3;
	keyboard_letters[5][0].key_height = 1;

	//ENTR
	keyboard_letters[5][1].lowercase = "ENT";
	keyboard_letters[5][1].uppercase = "ENT";
	keyboard_letters[5][1].key_width = 3;
	keyboard_letters[5][1].key_height = 2;
	for(uint8_t i = 0; i < keyboard_letters[5][1].key_height; i++){
		keyboard_letters[5][1+i].key_expanded_from_x = 5;
		keyboard_letters[5][1+i].key_expanded_from_y = 1;
	}

	//QUIT
	keyboard_letters[5][3].lowercase = "QUIT";
	keyboard_letters[5][3].uppercase = "QUIT";
	keyboard_letters[5][3].key_width = 3;
	keyboard_letters[5][3].key_height = 1;
	keyboard_letters[5][3].color = TFT_RED;
}