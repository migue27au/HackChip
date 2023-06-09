#include "SnifferBuffer.h"

SnifferBuffer snifferBuffer = SnifferBuffer();


//WIFI PACKET
WifiPacket::~WifiPacket(){
  clear();
}

WifiPacket::WifiPacket(){
  clear();
}

WifiPacket::WifiPacket(uint8_t payload[], uint32_t payload_length){
  if(payload_length <= MAX_PACKET_LENGTH-16){
    _payload_length = payload_length;

    uint32_t microSeconds = micros(); // e.g. 45200400 => 45s 200ms 400us
    uint32_t seconds = (microSeconds/1000)/1000; // e.g. 45200400/1000/1000 = 45200 / 1000 = 45s
        
    microSeconds -= seconds*1000*1000; // e.g. 45200400 - 45*1000*1000 = 45200400 - 45000000 = 400us (because we only need the offset)
    // ts_usec
    _seconds = seconds;
    _microseconds = microSeconds;
    for(uint16_t i = 0; i < payload_length; i++){
      _payload[i] = payload[i];
    }
  }
}

void WifiPacket::set(WifiPacket wifi_packet){
  _payload_length = wifi_packet.getPacketSize();
  getPayload(_payload);
}

void WifiPacket::clear(){
  _payload_length = 0;
  for(uint16_t i = 0; i < MAX_PACKET_LENGTH; i++){
    _payload[i] = 0;
  }
}

String WifiPacket::getString(){
  String s = "";

  for(uint16_t i = 0; i < _payload_length; i++){
    s += String(_payload[i], HEX);
  }

  return s;
}

uint16_t WifiPacket::getPacketSize(){
  return _payload_length;
}
void WifiPacket::getPayload(uint8_t payload[]){
  for(uint16_t i = 0; i < _payload_length; i++){
    payload[i] = _payload[i];
  }
}

uint32_t WifiPacket::getSeconds(){
  return _seconds;
}
uint32_t WifiPacket::getMicroSeconds(){
  return _microseconds;
}

void WifiPacket::getPCAPPacketFirstBytes(uint8_t bytes[]){
  bytes[0] = _seconds;
  bytes[1] = _seconds >> 8;
  bytes[2] = _seconds >> 16;
  bytes[3] = _seconds >> 24;
  
  bytes[4] = _microseconds;
  bytes[5] = _microseconds >> 8;
  bytes[6] = _microseconds >> 16;
  bytes[7] = _microseconds >> 24;
  
  // incl_len
  bytes[8] = _payload_length;
  bytes[9] = _payload_length >> 8;
  bytes[10] = _payload_length >> 16;
  bytes[11] = _payload_length >> 24;

  // orig_len
  bytes[12] = _payload_length;
  bytes[13] = _payload_length >> 8;
  bytes[14] = _payload_length >> 16;
  bytes[15] = _payload_length >> 24;
}

//SNIFFER BUFFER

SnifferBuffer::~SnifferBuffer(){
  reset();
}

SnifferBuffer::SnifferBuffer(){
  reset();
}

void SnifferBuffer::reset(){
  packets_size = 0;
  tmpPacketCounter = 0;
  deauths = 0;       // deauth frames per second
  rssiSum = 0;

  ap_stations_num = 0;
  for(uint8_t i = 0; i < MAX_AP_STATIONS; i++){
    ap_stations[i].sta_num = 0;
  }
}


void SnifferBuffer::addPacket(WifiPacket wifi_packet){
  if(packets_size < MAX_PACKETS){
    packets[packets_size] = wifi_packet;
    packets_size++;  
    Serial.print("Packet added"); 
  }
}

void SnifferBuffer::increaseRRSI(uint32_t new_rssiSum){
  rssiSum += new_rssiSum;
}
void SnifferBuffer::increaseDeauths(){
  deauths++;
}
void SnifferBuffer::increasePacketCounter(){
  tmpPacketCounter++;
}

WifiPacket SnifferBuffer::getPacket(uint16_t position){
  if(position <= packets_size-1){
    return packets[position];
  } else {
    WifiPacket wp;
    return wp;
  }
}

uint32_t SnifferBuffer::getPacketsSize(){
  return packets_size;
}

void SnifferBuffer_wifi_sniffer(void* buf, wifi_promiscuous_pkt_type_t type){
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  if (type == WIFI_PKT_MGMT && (pkt->payload[0] == 0xA0 || pkt->payload[0] == 0xC0 )) {
    snifferBuffer.increaseDeauths();
  }

  if (type == WIFI_PKT_MISC) return;
  if (ctrl.sig_len > MAX_PACKET_LENGTH) return;

  uint32_t packetLength = ctrl.sig_len;
  if (type == WIFI_PKT_MGMT) packetLength -= 4;  // fix for known bug in the IDF https://github.com/espressif/esp-idf/issues/886


  snifferBuffer.increasePacketCounter();
  snifferBuffer.increaseRRSI(ctrl.rssi);

  WifiPacket wifi_packet = WifiPacket(pkt->payload, packetLength);  
  
  Serial.print("LEN: ");
  Serial.print(packetLength);
  Serial.print("\tpkt: ");
  Serial.println(wifi_packet.getString());

  //beacon
  if(pkt->payload[0] == 0x80 && snifferBuffer.ap_stations_num < MAX_AP_STATIONS){
    Serial.println("BEACON");
    bool ap_station_added = false;
    uint8_t i = 0;
    while(i < snifferBuffer.ap_stations_num && ap_station_added == false){
      if (pkt->payload[10] == snifferBuffer.ap_stations[i].ap_addr[0] && pkt->payload[11] == snifferBuffer.ap_stations[i].ap_addr[1] && pkt->payload[12] == snifferBuffer.ap_stations[i].ap_addr[2] && pkt->payload[13] == snifferBuffer.ap_stations[i].ap_addr[3] && pkt->payload[14] == snifferBuffer.ap_stations[i].ap_addr[4] && pkt->payload[15] == snifferBuffer.ap_stations[i].ap_addr[5]){
        ap_station_added = true;
      } else {
        i++;  
      }
    }
    if(ap_station_added == false){
      snifferBuffer.ap_stations[i].ap_addr[0] = pkt->payload[10];
      snifferBuffer.ap_stations[i].ap_addr[1] = pkt->payload[11];
      snifferBuffer.ap_stations[i].ap_addr[2] = pkt->payload[12];
      snifferBuffer.ap_stations[i].ap_addr[3] = pkt->payload[13];
      snifferBuffer.ap_stations[i].ap_addr[4] = pkt->payload[14];
      snifferBuffer.ap_stations[i].ap_addr[5] = pkt->payload[15];
      
      snifferBuffer.ap_stations_num++;
    }
  //probe request or probe response
  } else if(pkt->payload[0] == 0x40 || pkt->payload[0] == 0x50){
    Serial.println("PROBE");
    bool ap_added = false;
    uint8_t i = 0;
    
    while(i < snifferBuffer.ap_stations_num && ap_added == false){
      //si la dirección origen es un ap
      if (pkt->payload[10] == snifferBuffer.ap_stations[i].ap_addr[0] && pkt->payload[11] == snifferBuffer.ap_stations[i].ap_addr[1] && pkt->payload[12] == snifferBuffer.ap_stations[i].ap_addr[2] && pkt->payload[13] == snifferBuffer.ap_stations[i].ap_addr[3] && pkt->payload[14] == snifferBuffer.ap_stations[i].ap_addr[4] && pkt->payload[15] == snifferBuffer.ap_stations[i].ap_addr[5]){
        ap_added = true;
      } else {
        i++;  
      }
    }
    if(ap_added && snifferBuffer.ap_stations[i].sta_num < MAX_STA_ADDR){
      bool ap_station_added = false;
      uint8_t j = 0;
      if(snifferBuffer.ap_stations_num < MAX_AP_STATIONS){
        while(j < snifferBuffer.ap_stations[i].sta_num && ap_station_added == false){
          //si la dirección destino es un sta
          if (pkt->payload[4] == snifferBuffer.ap_stations[i].sta_addr[j][0] && pkt->payload[5] == snifferBuffer.ap_stations[i].sta_addr[j][1] && pkt->payload[6] == snifferBuffer.ap_stations[i].sta_addr[j][2] && pkt->payload[7] == snifferBuffer.ap_stations[i].sta_addr[j][3] && pkt->payload[8] == snifferBuffer.ap_stations[i].sta_addr[j][4] && pkt->payload[9] == snifferBuffer.ap_stations[i].sta_addr[j][5]){
            ap_station_added = true;
          } else {
            i++;  
          }
        }
        if(ap_station_added == false){
          snifferBuffer.ap_stations[i].sta_addr[j][0] = pkt->payload[4];
          snifferBuffer.ap_stations[i].sta_addr[j][1] = pkt->payload[5];
          snifferBuffer.ap_stations[i].sta_addr[j][2] = pkt->payload[6];
          snifferBuffer.ap_stations[i].sta_addr[j][3] = pkt->payload[7];
          snifferBuffer.ap_stations[i].sta_addr[j][4] = pkt->payload[8];
          snifferBuffer.ap_stations[i].sta_addr[j][5] = pkt->payload[9];
          
          snifferBuffer.ap_stations[i].sta_num++;  
        }
      }
    }
  }

  snifferBuffer.addPacket(wifi_packet);
}

void SnifferBuffer::printAPStations(){
  for(uint8_t i = 0; i < ap_stations_num; i++){
    Serial.print("AP: ");
    Serial.print(ap_stations[i].ap_addr[0], HEX);
    Serial.print(":");
    Serial.print(ap_stations[i].ap_addr[1], HEX);
    Serial.print(":");
    Serial.print(ap_stations[i].ap_addr[2], HEX);
    Serial.print(":");
    Serial.print(ap_stations[i].ap_addr[3], HEX);
    Serial.print(":");
    Serial.print(ap_stations[i].ap_addr[4], HEX);
    Serial.print(":");
    Serial.println(ap_stations[i].ap_addr[5], HEX);
    
    for(uint8_t j = 0; j < ap_stations[i].sta_num; j++){
      Serial.print("\tSTA: ");
      Serial.print(ap_stations[i].sta_addr[j][0], HEX);
      Serial.print(":");
      Serial.print(ap_stations[i].sta_addr[j][1], HEX);
      Serial.print(":");
      Serial.print(ap_stations[i].sta_addr[j][2], HEX);
      Serial.print(":");
      Serial.print(ap_stations[i].sta_addr[j][3], HEX);
      Serial.print(":");
      Serial.print(ap_stations[i].sta_addr[j][4], HEX);
      Serial.print(":");
      Serial.println(ap_stations[i].sta_addr[j][5], HEX);
    }
  }
}

uint8_t SnifferBuffer::getAPNumDiscovered(){
  return ap_stations_num;
}

APStations SnifferBuffer::getAPDiscovered(uint8_t num){
  if(num >= ap_stations_num){
    return ap_stations[0];
  } else {
    return ap_stations[num];  
  }
}