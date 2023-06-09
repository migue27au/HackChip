#ifndef SNIFFER_BUFFER_H
#define SNIFFER_BUFFER_H

#include "Arduino.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "String.h"

#include "PacketSender.h"

#define MAX_PACKETS 64
#define MAX_PACKET_LENGTH 1024

#define PCAP_PACKET_FIRST_BYTES_SIZE 16

class WifiPacket{
    public:
      ~WifiPacket();
      WifiPacket();
      WifiPacket(uint8_t payload[], uint32_t payload_length);
      void set(WifiPacket wifi_packet);
      
      uint16_t getPacketSize();
      void getPayload(uint8_t payload[]);
      uint32_t getSeconds();
      uint32_t getMicroSeconds();
      
      void clear();
      String getString();
      
      void getPCAPPacketFirstBytes(uint8_t bytes[]);
    private:
      uint8_t _payload[MAX_PACKET_LENGTH];
      uint16_t _payload_length = 0;
      uint32_t _microseconds = 0;
      uint32_t _seconds = 0;
};

#define MAX_STA_ADDR 16
#define MAX_AP_STATIONS 16

typedef struct ap_stations {
    MacAddr ap_addr;
    uint8_t sta_num = 0;
    MacAddr sta_addr[MAX_STA_ADDR];
} APStations;

void SnifferBuffer_wifi_sniffer(void* buf, wifi_promiscuous_pkt_type_t type);

class SnifferBuffer {
    public:
      ~SnifferBuffer();
      SnifferBuffer();
      
      void addPacket(WifiPacket wifi_packet);
      void reset();
      
      WifiPacket getPacket(uint16_t position);
      uint32_t getPacketsSize();
      
      void increaseRRSI(uint32_t new_rssiSum);
      void increaseDeauths();
      void increasePacketCounter();
      
      void printAPStations();

      uint8_t getAPNumDiscovered();
      APStations getAPDiscovered(uint8_t num);
      
      uint8_t ap_stations_num = 0;
      APStations ap_stations[MAX_AP_STATIONS];
      
    private:
      uint16_t packets_size = 0;
      WifiPacket packets[MAX_PACKETS];
      uint32_t tmpPacketCounter;
      uint32_t deauths;
      uint32_t rssiSum;
};

#endif  //SNIFFER_BUFFER_H