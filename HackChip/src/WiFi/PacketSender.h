#ifndef PACKET_SENDER_H
#define PACKET_SENDER_H

#include "Arduino.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <cstring>

#define MAX_CH 14       // 1 - 14 channels (1-11 for US, 1-13 for EU and 1-14 for Japan)


typedef uint8_t MacAddr[6];


const uint8_t probePacket[] = {
            /*  0 - 1  */ 0x40, 0x00,                                       // Type: Probe Request
            /*  2 - 3  */ 0x00, 0x00,                                       // Duration: 0 microseconds
            /*  4 - 9  */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination: Broadcast
            /* 10 - 15 */ 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // Source: random MAC
            /* 16 - 21 */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // BSS Id: Broadcast
            /* 22 - 23 */ 0x00, 0x00,                                       // Sequence number (will be replaced by the SDK)
            /* 24 - 25 */ 0x00, 0x20,                                       // Tag: Set SSID length, Tag length: 32
            /* 26 - 57 */ 0x20, 0x20,               0x20, 0x20,             // SSID
            0x20,               0x20,               0x20, 0x20,
            0x20,               0x20,               0x20, 0x20,
            0x20,               0x20,               0x20, 0x20,
            0x20,               0x20,               0x20, 0x20,
            0x20,               0x20,               0x20, 0x20,
            0x20,               0x20,               0x20, 0x20,
            0x20,               0x20,               0x20, 0x20,
            /* 58 - 59 */ 0x01, 0x08, // Tag Number: Supported Rates (1), Tag length: 8
            /* 60 */ 0x82,            // 1(B)
            /* 61 */ 0x84,            // 2(B)
            /* 62 */ 0x8b,            // 5.5(B)
            /* 63 */ 0x96,            // 11(B)
            /* 64 */ 0x24,            // 18
            /* 65 */ 0x30,            // 24
            /* 66 */ 0x48,            // 36
            /* 67 */ 0x6c             // 54
        };

        uint8_t deauthPacket[26] = {
          /* 0 - 1 */ 0xc0, 0x00,                             // type, subtype c0: deauth (a0: disassociate),
          /* 2 - 3 */ 0x3a, 0x01,                             //duration (SDK takes care of that)
          /* 4 - 9 */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     //reciever (target)
          /* 10 - 15 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //source (ap)
          /* 16 - 21 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //BSSID (ap)
          /* 22 - 23 */ 0xf0, 0xff,                             //fragment & squence number
          /* 24 - 25 */ 0x02, 0x00                              // reason code (1 = unspecified reason)                    
        };

        const uint8_t beaconPacket[109] = {
            /*  0 - 3  */ 0x80,   0x00, 0x00, 0x00,                         // Type/Subtype: managment beacon frame
            /*  4 - 9  */ 0xFF,   0xFF, 0xFF, 0xFF, 0xFF, 0xFF,             // Destination: broadcast
            /* 10 - 15 */ 0x01,   0x02, 0x03, 0x04, 0x05, 0x06,             // Source
            /* 16 - 21 */ 0x01,   0x02, 0x03, 0x04, 0x05, 0x06,             // BSSID: Source

            // Fixed parameters
            /* 22 - 23 */ 0x00,   0x00,                                     // Fragment & sequence number (will be done by the SDK)
            /* 24 - 31 */ 0x83,   0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
            /* 32 - 33 */ 0xff,   0xff,                                     // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
            /* 34 - 35 */ 0x31,   0x00,                                     // capabilities Tnformation

            // Tagged parameters

            // SSID parameters
            /* 36 - 37 */ 0x00,   0x20, // Tag: Set SSID length, Tag length: 32
            /* 38 - 69 */ 0x20,   0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20,
            0x20,                 0x20, 0x20, 0x20, // SSID

            // Supported Rates
            /* 70 - 71 */ 0x01,   0x08,             // Tag: Supported Rates, Tag length: 8
            /* 72 */ 0x82,                          // 1(B)
            /* 73 */ 0x84,                          // 2(B)
            /* 74 */ 0x8b,                          // 5.5(B)
            /* 75 */ 0x96,                          // 11(B)
            /* 76 */ 0x24,                          // 18
            /* 77 */ 0x30,                          // 24
            /* 78 */ 0x48,                          // 36
            /* 79 */ 0x6c,                          // 54

            // Current Channel
            /* 80 - 81 */ 0x03,   0x01,             // Channel set, length
            /* 82 */ 0x01,                          // Current Channel

            // RSN information
            /*  83 -  84 */ 0x30, 0x18,
            /*  85 -  86 */ 0x01, 0x00,
            /*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
            /*  91 -  92 */ 0x02, 0x00,
            /*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
            /* 101 - 102 */ 0x01, 0x00,
            /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
            /* 107 - 108 */ 0x00, 0x00
        };


//https://github.com/spacehuhn/esp8266_deauther/blob/fda01a64861f3e0e8e9379a8e8ed743cb56038f7/esp8266_deauther/Attack.h
//packets and code based on
//https://github.com/spacehuhn/esp8266_deauther/blob/fda01a64861f3e0e8e9379a8e8ed743cb56038f7/esp8266_deauther/Attack.h
//and
//https://github.com/spacehuhn/esp8266_deauther/blob/fda01a64861f3e0e8e9379a8e8ed743cb56038f7/esp8266_deauther/Attack.cpp

class PacketSender {
    public:
        esp_err_t deauth(const MacAddr ap, const MacAddr station, const MacAddr bssid, uint8_t reason, uint8_t channel);
        void deauth_bypassed(const MacAddr ap, const MacAddr station, const MacAddr bssid, uint8_t reason, uint8_t channel);
        
        esp_err_t beacon(const MacAddr mac, const char* ssid, uint8_t channel, bool wpa2);
        void beacon_bypassed(const MacAddr mac, const char* ssid, uint8_t channel, bool wpa2);

        esp_err_t probe(const MacAddr mac, const char* ssid, uint8_t channel);
        
        esp_err_t raw(const uint8_t* packet, int32_t len, bool en_sys_seq = false, bool interface_ap = true);
        void wsl_bypasser_send_raw_frame(const uint8_t *frame_buffer, int size);
    private:
        //esp_err_t change_channel(uint8_t channel);
        uint16_t seqnum;
        uint8_t buffer[200];
};

#endif  //PACKET_SENDER_H