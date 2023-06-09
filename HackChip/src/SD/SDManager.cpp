#include "SDManager.h"

//-------------SD CONFIGURATION---------------------
SDManager::SDManager(bool debugEnabledX){
  debugEnabled = debugEnabledX;
}

void SDManager::begin(fs::FS *fs_aux){
  fs = fs_aux;
}

bool SDManager::listDir(const char * dirname, uint8_t levels){
  if(debugEnabled){
    Serial.print(F("SDManager> Listing directory: ")); Serial.println(dirname);
  }

  File root = fs->open(dirname);
  if(!root){
    Serial.println(F("SDManager> Failed to open directory"));
    return false;
  }
  if(!root.isDirectory()){
    Serial.println(F("SDManager> Not a directory"));
    return false;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print(F("  DIR : "));
      Serial.println(file.name());
      if(levels){
        listDir(file.name(), levels -1);
      }
    } else {
      Serial.print(F("  FILE: "));
      Serial.print(file.name());
      Serial.print(F("  SIZE: "));
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  return true;
}

long SDManager::countElementsInDir(const char * dirname){
  if(debugEnabled){
    Serial.print(F("SDManager> Counting files in directory: ")); Serial.println(dirname);
  }
  long num_files = 0;

  File root = fs->open(dirname);
  if(!root){
    if(debugEnabled)
      Serial.println(F("SDManager> Failed to open directory"));
    return -1;
  }
  if(!root.isDirectory()){
    if(debugEnabled)
      Serial.println(F("SDManager> Not a directory"));
    return -1;
  }

  File file = root.openNextFile();
  while(file){
    num_files++;
    file = root.openNextFile();
  }

  return num_files;
}

bool SDManager::createDir(const char * path){
  if(debugEnabled){
    Serial.print(F("SDManager> Creating directory: ")); Serial.println(path);
  }
  if(fs->mkdir(path)){
    if(debugEnabled)
      Serial.println(F("SDManager> Dir created"));
    return true;
  } else {
    if(debugEnabled)
      Serial.println(F("SDManager> mkdir failed"));
    return false;
  }
}

bool SDManager::removeDir(const char * path){
  if(debugEnabled){
    Serial.print(F("SDManager> Deleting directory: ")); Serial.println(path);
  }
  if(fs->rmdir(path)){
    if(debugEnabled)
      Serial.println(F("SDManager> Dir removed"));
    return true;
  } else {
    if(debugEnabled)
      Serial.println(F("SDManager> rmdir failed"));
    return false;
  }
}

String SDManager::readFile(const char * path){
  String s = "";
  if(debugEnabled){
    Serial.print(F("SDManager> Reading file: ")); Serial.println(path);
  }

  File file = fs->open(path);
  if(!file){
    if(debugEnabled)
      Serial.println(F("SDManager> Failed to open file for reading"));
    return s;
  }

  while(file.available()){
    s += (char)file.read();
  }
  file.close();
  if(debugEnabled){
    Serial.print(F("SDManager> Bytes readed: ")); Serial.println(s.length());
  }
  return s;
}

bool SDManager::writeFile(const char * path, const char * message){
  if(debugEnabled){
    Serial.print(F("SDManager> Writing file: ")); Serial.println(path);
  }

  File file = fs->open(path, FILE_WRITE );
  if(!file){
    if(debugEnabled)
      Serial.println(F("SDManager> Failed to open file for writing"));
    return false;
  }

  bool ok = false;
  
  if(file.print(message)){
    if(debugEnabled)
      Serial.println(F("SDManager> File written"));
    ok = true;
  } else if(debugEnabled){
    Serial.println(F("SDManager> Write failed"));
  }
  file.close();

  return ok;
}

bool SDManager::appendFile(const char * path, const char * message){
  if(debugEnabled){
    Serial.print(F("SDManager> Appending to file: ")); Serial.println(path);
  }

  File file = fs->open(path, FILE_APPEND);
  if(!file){
    if(debugEnabled){
      Serial.println(F("SDManager> Failed to open file for appending"));
    }
    return false;
  }

  bool ok = false;

  if(file.print(message)){
    if(debugEnabled)
      Serial.println(F("SDManager> Message appended"));
    ok = true;
  } else if(debugEnabled){
    Serial.println(F("SDManager> Append failed"));
  }
  file.close();
  return ok;
}

bool SDManager::renameFile(const char * path1, const char * path2){
  if(debugEnabled){
    Serial.print(F("SDManager> Renaming file: ")); Serial.println(path1); Serial.print(F(" to ")); Serial.println(path2);
  }

  if (fs->rename(path1, path2)) {
    if(debugEnabled)
      Serial.println(F("SDManager> File renamed"));
    return true;
  } else {
    if(debugEnabled)
      Serial.println(F("SDManager> Rename failed"));
    return false;
  }
}

bool SDManager::deleteFile(const char * path){
  if(debugEnabled){
    Serial.print(F("SDManager> Deleting file: ")); Serial.println(path);
  }
  if(fs->remove(path)){
    if(debugEnabled)
      Serial.println(F("SDManager> File deleted"));
    return true;
  } else {
    if(debugEnabled)
      Serial.println(F("SDManager> Delete failed"));
    return false;
  }
}


//PUBLIC FUNCTIONS

void SDManager::createPCAPFile(){
  deleteFile(SD_PCAP_FILE);

  uint32_t microSeconds = micros(); // e.g. 45200400 => 45s 200ms 400us
  uint32_t seconds = (microSeconds/1000)/1000; // e.g. 45200400/1000/1000 = 45200 / 1000 = 45s

  Serial.println("saving file");
  
  File pcap_file = fs->open(SD_PCAP_FILE, FILE_APPEND);
  if (!pcap_file) {
    Serial.printf("Failed to open file %s\r\n", SD_PCAP_FILE);
    return;
  }
  
  saving = true;

  uint8_t magic_bytes[24];
  
  uint32_t magic_number = 0xa1b2c3d4;
  magic_bytes[0] = magic_number;
  magic_bytes[1] = magic_number >> 8;
  magic_bytes[2] = magic_number >> 16;
  magic_bytes[3] = magic_number >> 24;
  
  uint16_t major_version_number = 2;
  magic_bytes[4] = major_version_number;
  magic_bytes[5] = major_version_number >> 8;
  
  uint16_t minor_version_number = 4;
  magic_bytes[6] = minor_version_number;
  magic_bytes[7] = minor_version_number >> 8;
  
  int32_t GMT_to_local_correction = 0;
  magic_bytes[8] = GMT_to_local_correction;
  magic_bytes[9] = GMT_to_local_correction >> 8;
  magic_bytes[10] = GMT_to_local_correction >> 16;
  magic_bytes[11] = GMT_to_local_correction >> 24;
  
  uint32_t accuracy_of_timestamps = 0;
  magic_bytes[12] = accuracy_of_timestamps;
  magic_bytes[13] = accuracy_of_timestamps >> 8;
  magic_bytes[14] = accuracy_of_timestamps >> 16;
  magic_bytes[15] = accuracy_of_timestamps >> 24;
  
  uint32_t max_length_of_packets = SNAP_LEN;
  magic_bytes[16] = max_length_of_packets;
  magic_bytes[17] = max_length_of_packets >> 8;
  magic_bytes[18] = max_length_of_packets >> 16;
  magic_bytes[19] = max_length_of_packets >> 24;
  
  uint32_t data_link_type = 105;
  magic_bytes[20] = data_link_type;
  magic_bytes[21] = data_link_type >> 8;
  magic_bytes[22] = data_link_type >> 16;
  magic_bytes[23] = data_link_type >> 24;
    
  pcap_file.write(magic_bytes, 24);
  pcap_file.close();
  
  saving = false;
}

void SDManager::addBytesToFile(String filename, uint8_t *buffer, uint32_t buffer_size){

  File pcap_file = fs->open(filename, FILE_APPEND);
  if (!pcap_file) {
    Serial.printf("Failed to open file %s\r\n", SD_PCAP_FILE);
    return;
  }
  saving = true;


  pcap_file.write(buffer, buffer_size);
  pcap_file.close();

  
  saving = false;
}