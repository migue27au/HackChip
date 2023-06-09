#include "TFT_eSPI.h"

#include "HackChip.h"


#define CS_SD 10


bool readPad(uint8_t min_cursor_value = 0, uint8_t max_cursor_value = 0);

int cursor_position = 0;
unsigned int max_cursor_position = 8;

unsigned short menu_hierarchy_selector = 0;  //indica la profundidad de los menús 0 MAIN> 1 WIFI> 2 etc. Usado para el índice de la variable inferior
short menu_selector[3] = {-1, -1, -1}; //no hay menu seleccionado

unsigned int menu_wifi_size = 5;
String main_menu = "["
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"DISPLAY\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"WIFI 802.11n\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"SETTINGS\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"NFC\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"USB\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"TEST3\"" + "},{\"c\":\""+String(TFT_GREEN)+  "\",\"s\":" + "\"TEST3\"" + "},{\"c\":\""+String(TFT_MAGENTA)+  "\",\"s\":" + "\"TEST3\"" + "}]}" +
  "]";
String menu_wifi = "["
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"LIST SSID\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"BEACON FLOOD\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"BEACON RICK-ROLL FLOOD\"" + "}]}," + 
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"SNIFF TRAFFIC\"" + "}]}," + 
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"CONNECT WIFI\"" + "}]}," + 
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"DEAUTH\"" + "}]}" +
  "]";

String menu_nfc = "["
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"READ TAG\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"TEST\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"TEST\"" + "}]}" + 
  "]";
  
String menu_usb = "["
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"BADUSB\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"TEST\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"TEST\"" + "}]}" + 
  "]";
String menu_display = "["
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"SHOW KEYBOARD\"" + "}]}," +
  "{\"r\":[{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"TEST COLORS\"" + "}]}" +
  "]";

TFT_eSPI my_display_tft= TFT_eSPI();
DisplayManager display = DisplayManager(&my_display_tft);
KeyboardManager keyboard = KeyboardManager(&my_display_tft);

WifiManager wifiManager;

SDManager sdManager = SDManager(true);

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("HackChip!");
  
  Wire.begin(43,44);
      
  wireScan();


  //SD EN ESP32
  if (!SD.begin(CS_SD)) {
    Serial.println(F("Card Mount Failed"));
    delay(250);
    ESP.restart();
  } else {
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
      Serial.println(F("No SD card attached"));
    }
    Serial.print(F("SD Card Type: "));
    if (cardType == CARD_MMC) {
      Serial.println(F("MMC"));
    } else if (cardType == CARD_SD) {
      Serial.println(F("SDSC"));
    } else if (cardType == CARD_SDHC) {
      Serial.println(F("SDHC"));
    } else {
      Serial.println(F("UNKNOWN"));
    }
  }

  sdManager.begin(&SD);
  delay(100);

  if(ads.begin(0x48) == false){
    Serial.println("ADS NOT STARTED");
    ESP.restart();
  }
  
  Serial.println("Setup done");
  Serial.println("Display> Hello");
  
  display.clear();
  display.hello();
  Serial.println("Display> Add headers");
  display.addHeaders("[{\"s\":\"Settings\",\"c\":65535},{\"s\":\"WiFi\",\"c\":65535},{\"s\":\"NFC\",\"c\":65535},{\"s\":\"USB\",\"c\":65535},{\"s\":\"RF\",\"c\":65535}]");
  delay(5000);     
  updateMenu();
}

uint8_t menu_level[3] = {0,0,0};
uint8_t cursor_row_position = 0;

void loop() {
  
  pad_checkUI();
  
  if(readPad()){
      updateMenu();
  }
}

#define MENU_0      0     //NO MENU
#define MENU_1      100   //SETTINGS
#define MENU_1_1    110   //  DISPLAY
#define MENU_1_1_1  111   //    SHOW KEYBOARD
#define MENU_1_1_2  112   //    FILL NUMBERS

#define MENU_2      200   //WIFI
#define MENU_2_1    210   //  LIST SSID
#define MENU_2_2    220   //  BEACON FLOOD
#define MENU_2_2_1  221   //    MANUAL
#define MENU_2_2_2  222   //    RICK ROLL
#define MENU_2_3    230   //  DEAUTH
#define MENU_2_3_1  231   //    BROADCAST
#define MENU_2_3_2  232   //    MANUAL
#define MENU_2_4    240   //  SNIFF
#define MENU_2_4_1  241   //    ALL
#define MENU_2_4_2  242   //    FILTER

#define MENU_3 300    //NFC

#define MENU_4 400    //USB

#define MENU_5 500    //RF

void updateMenu(){
  display.clear();
  switch(menu_level[0]*100+menu_level[1]*10+menu_level[2]){
    case MENU_0:
      display.deleteSubmenuElements();
      
    break;
    
    case MENU_1:  //SETTINGS
      display.deleteSubmenuElements();
      display.addSubmenuElement("Settings");
      display.addRows("[{\"s\":\"Display\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;

    case MENU_1_1:
      display.deleteSubmenuElements();
      display.addSubmenuElement("Settings");
      display.addSubmenuElement("Display");
      
      display.addRows("[{\"s\":\"Show Keyboard\",\"c\":65535},{\"s\":\"Fill with numbers\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;

    case MENU_1_1_1:
    {
      keyboard.setLabel("INPUT TEXT EXAMPLE");
      String text_str = keyboard.displayKeyboard();
      Serial.println(text_str);
      
      menu_level[2] = 0;    //salgo de la funcionalidad
      updateMenu();
    }
    break;

    case MENU_2:  //WIFI
      display.deleteSubmenuElements();
      display.addSubmenuElement("WiFi");
      
      display.addRows("[{\"s\":\"List SSID\",\"c\":65535},{\"s\":\"Beacon Flood\",\"c\":65535},{\"s\":\"Deauthentication\",\"c\":65535},{\"s\":\"Sniffer\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;
    
    case MENU_2_1:
    {
      String wifis = "";
      int max_cursor_value = wifiManager.listSSIDs(&wifis);
      display.deleteSubmenuElements();
      display.addSubmenuElement("WiFi");
      display.addSubmenuElement("List SSIDs");
      display.fillMenuSubmenu();
      
      display.showMenuLayout();
      display.fillMenuHeaders(menu_level[0]);
      display.fillSSIDs(wifis, cursor_row_position);
      display.showSelectors(menu_level[0], cursor_row_position, menu_level[2]);
      
      while(menu_level[1] != 0){
        //actualiza el display si se pulsa algún botón
        if(readPad(1, max_cursor_value)){
          display.fillSSIDs(wifis, cursor_row_position);
          display.showSelectors(menu_level[0], cursor_row_position, menu_level[2]);
        }
      }
    }
    break;
    case MENU_2_2:
      display.deleteSubmenuElements();
      display.addSubmenuElement("WiFi");
      display.addSubmenuElement("Beacon Flood");
      
      display.addRows("[{\"s\":\"Manual\",\"c\":65535},{\"s\":\"Rick Roll\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;

    //beacon_flood_manual
    case MENU_2_2_1:
    {
      String ssid_str = keyboard.displayKeyboard();
      if(ssid_str.length() > 0){
        wifiManager.beaconFlood(ssid_str);
      }

      menu_level[2] = 0;    //salgo de la funcionalidad
      updateMenu();
    }
    break;
    
    case MENU_2_3:
      display.deleteSubmenuElements();
      display.addSubmenuElement("WiFi");
      display.addSubmenuElement("Deauthentication");
      
      display.addRows("[{\"s\":\"Manual\",\"c\":65535},{\"s\":\"Broadcast\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;


    case MENU_3:  //NFC
      display.deleteSubmenuElements();
      display.addSubmenuElement("NFC");
      
      display.addRows("[{\"s\":\"Read TAG\",\"c\":65535},{\"s\":\"Write TAG\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;

    case MENU_4:  //USB
      display.deleteSubmenuElements();
      display.addSubmenuElement("USB");
      
      display.addRows("[{\"s\":\"BadUSB\",\"c\":65535},{\"s\":\"Simule Keyboard\",\"c\":65535},{\"s\":\"Simule Mouse\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;

    case MENU_5:  //RF
      display.deleteSubmenuElements();
      display.addSubmenuElement("RF");
      
      display.addRows("[{\"s\":\"Receive 433MHz\",\"c\":65535},{\"s\":\"Send 433 MHz\",\"c\":65535}]");
      display.fillMenuRows(cursor_row_position);
    break;
    
    default:
      
    break;
  }
  
  display.showMenuLayout();
  display.fillMenuHeaders(menu_level[0]);
  display.fillMenuSubmenu();
  display.showSelectors(menu_level[0], cursor_row_position, menu_level[2]);
  Serial.println("Menu updated");
}

/*
void updateMenu2(){
  //limpio el display
  display_clearDisplay();
  //profundidad 0 --> menú principal
  if(menu_hierarchy_selector == 0){
    display_newData(main_menu);
  } else {
    //cambiar el menu
    switch(menu_selector[0]){
      //DISPLAY OPTIONS
      case 0:
        if(menu_hierarchy_selector == 1){
          display_newData(menu_display);  
        } else {
          switch(menu_selector[1]){

            case 0:
            {
              String keyboard_text = keyboard_displayKeyboard();
              Serial.printf("TEXT: %s\r\n", keyboard_text.c_str());
              
              //Vuelvo al menú anterior
              menu_selector[menu_hierarchy_selector] = -1;
              //reseteo el cursor
              cursor_position = 0;
              //incremento el selector de jerarquía
              menu_hierarchy_selector--;
          
              //actualizo el menu
              updateMenu();
            }
            break;

            case 1:
              display_fillWithNumbers();
            break;
          }
        }
      break;
      
      //WIFI OPTIONS
      case 1:
        if(menu_hierarchy_selector == 1){
          display_newData(menu_wifi);  
        } else {
          //aqui menu_hierarchy_selector = 2
          switch(menu_selector[1]){

            case 0:
              //display_newData(wifiManager.listSSIDs(), 1); //json, text_size
            break;
            case 1:
            {
                String ssid_str = keyboard_displayKeyboard();
                if(ssid_str.length() > 0){
                  wifiManager.beaconFlood(ssid_str);
                }
            }
            break;
            case 2:
            break;
            //SNIFFER
            case 3:
              //STOP
              if(wifiManager.isMonitor()){
                menu_wifi.replace("{\"c\":\""+String(TFT_RED)+  "\",\"s\":" + "\"STOP SNIFF TRAFFIC\"" + "}", "{\"c\":\""+String(TFT_WHITE)+"\",\"s\":" + "\"SNIFF TRAFFIC\"" + "}");
                Serial.println("Stop sniffing");
                delay(100);
                
                wifiManager.sniffTraffic(false);
                wifiManager.resetWiFi();

                Serial.println(wifiManager.snifferBuffer_ptr->getPacketsSize());
                
                sdManager.createPCAPFile();
                
                for(uint16_t p = 0; p < wifiManager.snifferBuffer_ptr->getPacketsSize(); p++){
                  Serial.println(wifiManager.snifferBuffer_ptr->getPacket(p).getString());
                  uint8_t first_bytes[PCAP_PACKET_FIRST_BYTES_SIZE];
                  wifiManager.snifferBuffer_ptr->getPacket(p).getPCAPPacketFirstBytes(first_bytes);
                  sdManager.addBytesToFile(SD_PCAP_FILE, first_bytes, PCAP_PACKET_FIRST_BYTES_SIZE);

                  uint8_t packet_payload[wifiManager.snifferBuffer_ptr->getPacket(p).getPacketSize()];
                  wifiManager.snifferBuffer_ptr->getPacket(p).getPayload(packet_payload);
                  sdManager.addBytesToFile(SD_PCAP_FILE, packet_payload, wifiManager.snifferBuffer_ptr->getPacket(p).getPacketSize());
                }
                wifiManager.snifferBuffer_ptr->printAPStations();
                String aps = "[";
                for(uint8_t ap = 0; ap < wifiManager.snifferBuffer_ptr->getAPNumDiscovered(); ap++){
                  APStations ap_station = wifiManager.snifferBuffer_ptr->getAPDiscovered(ap);
                  String ap_mac = String();
                  char buffer[3];
                  
                  for (int i = 0; i < 6; i++) {
                    sprintf(buffer, "%02X", ap_station.ap_addr[i]);
                    ap_mac += buffer;
                    if (i < 5) {
                      ap_mac += ":";
                    }
                  }
                  
                  for(uint8_t sta = 0; sta < ap_station.sta_num; sta++){
                    String sta_mac = String();
                    for (int i = 0; i < 6; i++) {
                      sprintf(buffer, "%02X", ap_station.sta_addr[sta][i]);
                      sta_mac += buffer;
                      if (i < 5) {
                        sta_mac += ":";
                      }
                    }
                    aps += "{\"r\":[{\"c\":\""+String(TFT_RED)+  "\",\"s\":" + "\"" + sta_mac + "\"" + "},{\"c\":\""+String(TFT_WHITE)+  "\",\"s\":" + "\"" + ap_mac + "\"" + "}]},";
                  }
                }
                aps = aps.substring(0, aps.length()-1) + "]";
                display_newData(aps, 1);

                wifiManager.snifferBuffer_ptr->reset();
                
              //START
              } else {
                menu_wifi.replace("{\"c\":\""+String(TFT_WHITE)+"\",\"s\":" + "\"SNIFF TRAFFIC\"" + "}", "{\"c\":\""+String(TFT_RED)+  "\",\"s\":" + "\"STOP SNIFF TRAFFIC\"" + "}");
                wifiManager.setMonitor();
                Serial.println("Start sniffing");
                delay(100);
                wifiManager.sniffTraffic(true);
              }
            break;
            //CONNECT TO WIFI
            case 4:
              {
                String ssid_str = keyboard_displayKeyboard();
                String pass_str = keyboard_displayKeyboard();
                //String ssid_str = "Europa";
                //String pass_str = "bebesitofiufiu";


                wifiManager.connectWiFi(ssid_str, pass_str);
              }
            break;
            //DEAUTH
            case 5:
              {
                //String ap_addr = keyboard_displayKeyboard();
                //String sta_addr = keyboard_displayKeyboard();
                String ap_addr = "4C:19:5D:D8:52:8E";
                //String sta_addr = "DC:6A:E7:C0:AB:DA";
                String sta_addr = "FF:FF:FF:FF:FF:FF";
                
                wifiManager.deauth(ap_addr, sta_addr);
              }
            break;
          }
        }
      break;
      
      //SETTINGS OPTIONS
      case 2:
      break;
      //NFC OPTIONS
      case 3:
      if(menu_hierarchy_selector == 1){
          display_newData(menu_nfc);  
        } else {
          switch(menu_selector[1]){

            case 0:
              nfc_init();
              while(true){
                nfc_start();
              }
            break;
          }
        }
      break;
    }
  }
  //maximo movimiento del cursor igual al número de rows
  max_cursor_position = display_countRows();
}
*/


bool readPad(uint8_t min_cursor_value, uint8_t max_cursor_value){
  pad_checkUI();
  bool change_menu = false;

  if(menu_level[1] != 0){
    min_cursor_value = 1;
  }
  if(max_cursor_value == 0){
    max_cursor_value = display.getMaxRows();
  }

  if(cursor_row_position < min_cursor_value){
    cursor_row_position = min_cursor_value;
  } else if(cursor_row_position > max_cursor_value){
    cursor_row_position = max_cursor_value;
  }

  //CAMBIAR MENU
  if(cursor_row_position == 0 && menu_level[1] == 0){
    if(pad[UI_RIGHT] && menu_level[0] < display.getMaxHeaders()){
      menu_level[0]++;
      menu_level[1] = 0;    //si cambio la pestaña del menu, el row seleccionado es 0
      change_menu = true;
    }
    if(pad[UI_LEFT] && menu_level[0] > 0){
      menu_level[0]--;
      menu_level[1] = 0;    //si cambio la pestaña del menu, el row seleccionado es 0
      change_menu = true;
    }
    if(pad[UI_DOWN] && cursor_row_position < max_cursor_value){
      cursor_row_position++;
      change_menu = true;
    }
    
  //CAMBIAR ROW (requiere un menu seleccionado)
  } else {
    if(pad[UI_DOWN] && cursor_row_position < max_cursor_value){
      cursor_row_position++;
      change_menu = true;
    }
    if(pad[UI_UP] && cursor_row_position > min_cursor_value){
      cursor_row_position--;
      change_menu = true;
    }

    //si le doy a la flecha derecha, actualizo el menu_level[1]
    if(pad[UI_RIGHT]){
      if(menu_level[1] == 0){
        menu_level[1] = cursor_row_position;
      } else if(menu_level[2] == 0){
        menu_level[2] = cursor_row_position;
      }
      change_menu = true;
      cursor_row_position = 1;
    }
    if(pad[UI_LEFT]){
      if(menu_level[2] != 0){
        menu_level[2] = 0;  
      } else {
        menu_level[1] = 0;
      }
      cursor_row_position = 1;
      change_menu = true;
    }
  }

  if(change_menu == true){
    delay(75);
    Serial.printf("Menu: %d Row: %d (cursor %d), submenu %d\r\n", menu_level[0], menu_level[1], cursor_row_position, menu_level[2]);
  }
  return change_menu;
}

void wireScan() {
  byte error, address;
  int nDevices = 0;

  delay(5000);

  Serial.println("Scanning for I2C devices ...");
  for(address = 0x01; address < 0x7f; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.printf("I2C device found at address 0x%02X\n", address);
      nDevices++;
    } else if(error != 2){
      Serial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0){
    Serial.println("No I2C devices found");
  }
}
