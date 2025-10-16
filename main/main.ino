#include <Arduino.h>
#include <SPI.h>
//-------------------------------
// ---------FILE SYSTEM---------//
#include <SD.h>
#include <FS.h>
//-------------------------------
//----------DISPLAY-------------//
#include <TFT_eSPI.h>
//--------------------------------
//----------AUDIO---------------//
#include <VS1053.h>
#include <BluetoothA2DPSink.h>
//--------------------------------
//----------INTERFACE-----------//
#include <Encoder.h>
//--------------------------------
#include "config.h"
#include "display_manager.h" // display manager
#include "audio_player.h" //audio manager
#include "file_manager.h"
#include "interface_manager.h"

extern AudioPlayer audioPlayer;
VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);
void listMP3Files();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("----MP3 Player HeadPhones FIX -----");

/*       RESETING VS1053
---------------------------------------*/
  pinMode(VS1053_RST, OUTPUT);
  digitalWrite(VS1053_RST, LOW);
  delay(100);
  digitalWrite(VS1053_RST, HIGH);
  delay(100);

/*       INIT VS1053
---------------------------------------*/
  player.begin();
  // Simple test - set volume and check DREQ
  player.setVolume(0);
  delay(50);

  if (digitalRead(VS1053_DREQ) == HIGH) {
    Serial.println("OK - VS1053B responding");
  
    // Configure for HEADPHONES on SPK output
    configureVS1053BForHeadphones();
  
    Serial.println("VS1053B configured for HEADPHONES on SPK output");
  }else{
    Serial.println("WARNING - DREQ not high, but continuing...");
  }

/*       SD CARD INIT
---------------------------------------*/
  Serial.print("SD card... ");
  if (SD.begin(SD_CS)) {
    Serial.println("OK");
    listMP3Files();
  }else {
    Serial.println("FAILED");
    while(1);
  }

  Serial.println("Connect headphones");  
  Serial.println("Testing test.mp3");  

  configureVS1053BForHeadphones();
  checkMP3File("/test.mp3");
  playFirstMP3();
  
}
void listMP3Files(){
  Serial.println("MP3 files on SD:");
  File root = SD.open("/");
  File file = root.openNextFile();
  bool found = false;

  while(file){
    String filename = String(file.name());
    if(filename.endsWith(".mp3") || filename.endsWith(".MP3")){
      Serial.print(" ");
      Serial.println(filename);
      found = true;
    }
    file = root.openNextFile();
  }
  if(!found){
    Serial.println("No MP3 files found");
  }
}

void playFirstMP3() {
  Serial.println("🎯 playFirstMP3() called");
  
  String filename = "/test.mp3";
  
  if(SD.exists(filename)) {
    Serial.println("✅ test.mp3 found");
    playMP3File(filename);
  } else {
    Serial.println("❌ test.mp3 not found");
    
    // Szukaj innych plików MP3
    File root = SD.open("/");
    while(File file = root.openNextFile()) {
      String foundFilename = String(file.name());
      if(foundFilename.endsWith(".mp3") || foundFilename.endsWith(".MP3")) {
        Serial.println("Found alternative: " + foundFilename);
        playMP3File("/" + foundFilename);
        root.close();
        return;
      }
    }
    root.close();
    Serial.println("❌ No MP3 files found!");
  }
}
void configureVS1053ForMP3() {
  Serial.println("Configuring VS1053 for MP3 decoding...");
  
  // ★★★ HARD RESET ★★★
  digitalWrite(VS1053_RST, LOW);
  delay(100);
  digitalWrite(VS1053_RST, HIGH);
  delay(100);
  
  // ★★★ LIBRARY INIT ★★★
  player.begin();
  
  // ★★★ KONFIGURACJA DLA MP3 ★★★
  player.writeRegister(SCI_MODE, 0x0800);  // SM_SDINEW - VS1002 native mode
  delay(10);
  
  player.writeRegister(SCI_CLOCKF, 0x8800); // 3.5x multiplier, 12.288MHz
  delay(10);
  
  player.writeRegister(SCI_BASS, 0x0000);   // Flat response
  delay(10);
  
  player.writeRegister(SCI_AUDATA, 44101);  // 44.1kHz stereo
  delay(10);
  
  // ★★★ TURN OFF EFFECTS THAT CAN CLOCK REGISTERS ★★★
  player.writeRegister(SCI_AIADDR, 0x0000); // CHECKING IF THERE ARE ANY ACTIVE TESTS
  delay(10);
  
  // ★★★ SET VOLUME ★★★
  player.setVolume(65); // MAX volume
  
  Serial.println("VS1053 configured for MP3 decoding");
}


void playMP3File(String filename){
  Serial.println("🚀 STARTING MP3 PLAYBACK WITH MP3 CONFIG");
  
  if(!SD.exists(filename)){
    Serial.println("❌ File not found");
    return;
  }

  // ★★★ USING MP3 CONFIG ★★★
  configureVS1053ForMP3();
  delay(100);

  File file = SD.open(filename);
  if(!file){
    Serial.println("❌ Cannot open file");
    return;
  }
  
  Serial.print("📁 File size: ");
  Serial.println(file.size());

  player.setVolume(65);

  // ★★★ DECODER START ★★★
  player.startSong();
  delay(50);
  
  Serial.println("🎵 Sending MP3 data to decoder...");

  uint8_t buffer[32];
  unsigned long bytesSent = 0;
  unsigned long lastPrint = millis();
  bool decoderBusy = false;

  while(file.available()){
    int bytesRead = file.read(buffer, 32);
    bytesSent += bytesRead;

    // ★★★ SEND DATA TO MP3 DECODER ★★★
    player.playChunk(buffer, bytesRead);

    // WAITING ON DRAQ
    unsigned long waitStart = millis();
    while(!digitalRead(VS1053_DREQ)) {
      delay(1);
    }
    unsigned long waitTime = millis() - waitStart;
    
    // if waiting more than 1ms than it's decoding now
    if(waitTime > 1 && !decoderBusy) {
      Serial.println("✅ DECODER STARTED WORKING!");
      decoderBusy = true;
    }

    // SHOW PROGRESS
    if(millis() - lastPrint > 1000) {
      Serial.print(".");
      lastPrint = millis();
    }
  }
  
  file.close();
  Serial.println("\n✅ PLAYBACK COMPLETE");
  Serial.print("📊 Total bytes sent: ");
  Serial.println(bytesSent);
  
  if(!decoderBusy) {
    Serial.println("❌ WARNING: Decoder never became busy - MP3 not being decoded");
  }
}

void configureVS1053BForHeadphones() {
  Serial.println("Configuring VS1053B for headphones...");
  
  // Soft reset
  player.softReset();
  delay(100);
  
  // ★★★ SET MODE TO MP3 DECODER ★★★
  player.writeRegister(SCI_MODE, 0x0804); // SM_SDINEW | SM_RESET
  delay(100);
  
  // ★★★ TURN OFF TEST MODE AND SET NORMAL MODE ★★★
  player.writeRegister(SCI_MODE, 0x0800); // Tylko SM_SDINEW
  delay(10);
  
  // SET CLOCK
  player.writeRegister(SCI_CLOCKF, 0x8800); // 3.5x multiplier
  delay(10);
  
  // SET NORMAL VOLUME
  player.setVolume(65); 
  delay(10);
  
  player.writeRegister(0x00, 0x0800); // SCI_MODE - tylko SM_SDINEW
  delay(10);
  
  Serial.println("VS1053B configured for headphones");
}


void loop(){
  Serial.println("\n=== Available commands ===");
  Serial.println("audio - check audio output");
  Serial.println("volume - test volume levels");
  Serial.println("modes - test audio output modes");
  Serial.println("hp - headphone connection diagnosis");
  Serial.println("play - play MP3 with current config");
  Serial.println("==========================");

  String command = Serial.readString();
  command.trim();

  if (command == "audio") checkAudioOutput();
  else if (command == "volume") checkAudioConfiguration();
  else if (command == "modes") testAudioOutputModes();
  else if (command == "hp") diagnoseHeadphoneConnection();
  else if (command == "play") playFirstMP3();
  else Serial.println("Unknown command: " + command);
}
