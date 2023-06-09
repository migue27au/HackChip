#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_HACKCHIP_FOLDER "/HackChip/"
#define SD_WIFI_FOLDER "/HackChip/wifi/"
#define TMP_FOLDER "/HackChip/tmp"
#define SD_PCAP_FILE "/HackChip/tmp/sniff_traffic.pcap"

#define SNAP_LEN 1024 // max len of each recieved packet

class SDManager{
public:
  SDManager(bool debugEnabledX = false);

  void begin(fs::FS * fs_aux);
  
  void addBytesToFile(String filename, uint8_t *buffer, uint32_t buffer_size);

  void createPCAPFile();
  

private:
  bool listDir(const char * dirname, uint8_t levels);
  long countElementsInDir(const char * dirname);
  bool createDir(const char * path);
  bool removeDir(const char * path);
  String readFile(const char * path);
  bool writeFile(const char * path, const char * message);
  bool appendFile(const char * path, const char * message);
  bool renameFile(const char * path1, const char * path2);
  bool deleteFile(const char * path);

  fs::FS * fs;
  bool debugEnabled = false;
  bool saving = false;
  bool writing = false;
};

#endif /* SD_HANDLER_H */
