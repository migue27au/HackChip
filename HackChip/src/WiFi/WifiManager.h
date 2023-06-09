#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "Arduino.h"

#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "String.h"
#include <ArduinoJson.h>

#include "PacketSender.h"
#include "SnifferBuffer.h"

// Default color definitions
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xD69A      /* 211, 211, 211 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFE19      /* 255, 192, 203 */ //Lighter pink, was 0xFC9F
#define TFT_BROWN       0x9A60      /* 150,  75,   0 */
#define TFT_GOLD        0xFEA0      /* 255, 215,   0 */
#define TFT_SILVER      0xC618      /* 192, 192, 192 */
#define TFT_SKYBLUE     0x867D      /* 135, 206, 235 */
#define TFT_VIOLET      0x915C      /* 180,  46, 226 */


#define WIFI_MANAGER_LOG_HEADER "WiFiManager"
#define DEFAULT_TIME_OF_ATTACK 60000		//milisegundos

#define DEFAULT_AP_SSID "HackChipSSID"
#define DEFAULT_AP_SSID_PASS "strongpass"
#define DEFAULT_AP_CHANNEL 1
#define DEFAULT_AP_HIDDEN false
#define DEFAULT_AP_MAX_CONNECTIONS 4


class WifiManager{
public:
	~WifiManager();
	WifiManager();

	void createAP(String ap_ssid = DEFAULT_AP_SSID, String ap_ssid_pass = DEFAULT_AP_SSID_PASS, uint8_t ap_channel = DEFAULT_AP_CHANNEL, bool ap_hidden = DEFAULT_AP_HIDDEN, uint8_t ap_max_connections = DEFAULT_AP_MAX_CONNECTIONS);
	void connectWiFi(String ssid, String pass);
	void randomMacAddr(MacAddr newMac);

	void setMonitor();
	bool isMonitor();
	void resetWiFi();

	void sniffTraffic(bool sniffer_enable);
    
    SnifferBuffer *snifferBuffer_ptr;
  
	//list SSID
	int countSSID();
	int listSSIDs(String *json_SSID);


	//Beacon Spam Attack
	void beaconFlood(String target_ssid, unsigned int time_of_attack = DEFAULT_TIME_OF_ATTACK);

    //deauth attack
    void deauth(String ap_mac, String sta_mac, unsigned int time_of_attack = DEFAULT_TIME_OF_ATTACK);
private:
	MacAddr _my_mac; 

	bool _conected_to_wifi;
	bool _monitor_mode;

	/*WHEN WIFI IS USED AS STATION*/
	String _sta_ssid = "";
	String _sta_ssid_pass = "";

	/*WHEN WIFI IS USED AS AP*/
	//https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-class.rst
	String _ap_ssid = "";
	String _ap_ssid_pass = "";
	uint8_t _ap_channel = 1;	//optional parameter to set Wi-Fi channel, from 1 to 13. Default channel = 1.
	bool _ap_hidden = false;	//optional parameter, if set to true will hide SSID.
	uint8_t _ap_max_connections = 4;	//optional parameter to set max simultaneous connected stations, from 0 to 8. Defaults to 4. Once the max number has been reached, any other station that wants to connect will be forced to wait until an already connected station disconnects.  
};

#endif	//WIFI_MANAGER_H
