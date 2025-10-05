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

void setup() {
  //Serial.begin(115200);
  //delay(1000);
  
  //Serial.println("----MP3 Test -----");
  
  //if(!audioPlayer.begin()){
    //Serial.println("Initialization FAILED");
    //return;
  //}

  //Serial.println("Pliki na karcie:");
  //listFiles("/");
  
  // Test odtwarzania
  //testPlayback();
  
  //Serial.println("✅All systems READY");
  //Serial.println("Connect headphones to VS1053");

  //delay(3000);
  //testPlayback();

  //pinMode(BTN_PLAY, INPUT_PULLUP);
  //pinMode(BTN_VOL_UP, INPUT_PULLUP);
  //pinMode(BTN_VOL_DOWN, INPUT_PULLUP);
  Serial.begin(115200);
  delay(3000);
  pinMode(VS1053_RST, OUTPUT);
  digitalWrite(VS1053_RST, LOW);
  delay(100);
  digitalWrite(VS1053_RST, HIGH);
  delay(100);
  
  Serial.println("=== TEST VS1053 ===");
  Serial.println("=== TEST VS1053 ===");
  
  Serial.print("VS1053 begin... ");
  player.begin();
  Serial.println("DONE");
  
  Serial.print("VS1053 set volume... ");
  player.setVolume(50);
  Serial.println("DONE");
  Serial.println("✅ VS1053 initialized (if no errors)");

  Serial.print("SD card... ");
  if (SD.begin(SD_CS)) {
    Serial.println("OK");
    
    Serial.println("Files on SD:");
    File root = SD.open("/");
    File file = root.openNextFile();
    while (file) {
      Serial.print("  ");
      Serial.println(file.name());
      file = root.openNextFile();
    }
  } else {
    Serial.println("FAILED");
  }

  Serial.println("Testing HP output...");  
  
  Serial.println("You should hear 500Hz tone in headphones");
  
  Serial.println("=== TEST PLAYBACK ===");
  if (SD.exists("/test.mp3")) {
    Serial.println("Found test.mp3 - attempting playback...");
  
  // Test playback
  
  debugVS1053();
  setMaxVolumeDirect();
  testTones();
  testPlayback();

  } else {
    Serial.println("test.mp3 not found - create this file on SD");
  }
  
  Serial.println("=== TEST COMPLETE ===");
}

void testPlayback() {

  player.setVolume(0);  // MAX volume
  Serial.println("Volume set to 100%");
  
  File file = SD.open("/test.mp3");
  if (!file) {
    Serial.println("Cannot open test.mp3");
    return;
  }
  
  const int bufferSize = 32;
  uint8_t buffer[bufferSize];
  
  Serial.println("Playback started...");
  unsigned long startTime = millis();
  
  while (file.available()) {
    int bytesRead = file.read(buffer, bufferSize);
    player.playChunk(buffer, bytesRead);
    
    while (!digitalRead(VS1053_DREQ)) {
      delay(1);
    }
    
    if (millis() - startTime > 2000) {
      Serial.print(".");
      startTime = millis();
    }
    
    delay(1);
  }
  
  file.close();
  Serial.println("\nPlayback finished");
}
void testSPKWithHeadphones() {
  Serial.println("Testing SPK with headphones - VOLUME MAX");
  player.setVolume(0);
  
  player.writeRegister(0x0, 0x0820);
  player.writeRegister(0x05, 0x3A18); // 150Hz - niski ton
  delay(2000);
  player.writeRegister(0x05, 0x0000);
  player.writeRegister(0x0, 0x0800);
}

void testSineWave() {
  Serial.println("Testing VS1053 sine wave...");
  
  // Włącz test sinusoidalny (1kHz)
  player.writeRegister(0x0, 0x0820);  // MODE = SM_TESTS | SM_SDINEW
  player.writeRegister(0x05, 0xAC45); // 1000Hz sine wave
  delay(2000);
  

  player.writeRegister(0x05, 0x0000);
  player.writeRegister(0x0, 0x0800);  // Normal mode
  
  Serial.println("Sine wave test finished - should hear 1kHz tone");
}
void debugVS1053() {
  Serial.println("=== VS1053 DEBUG ===");
  
  uint16_t version = 0;
  for (int i = 0; i < 3; i++) {
    player.setVolume(0); // 0 = max volume
    delay(100);

    Serial.print("DREQ state: ");
    Serial.println(digitalRead(VS1053_DREQ));
    delay(100);
  }
  
  Serial.println("VS1053 basic test completed");
}
void setMaxVolumeDirect() {
  Serial.println("Setting MAX volume...");

  Serial.println("Testing volume 0 (MAX)...");
  player.setVolume(0);
  delay(1000);
  
  Serial.println("Testing volume 50...");
  player.setVolume(50);
  delay(1000);
  
  Serial.println("Testing volume 100...");
  player.setVolume(100);
  delay(1000);
  
  Serial.println("Testing volume 0 again...");
  player.setVolume(0); // MAX volume
}
void testTones() {
  Serial.println("=== TONE TESTS ===");
  
  player.setVolume(0); // MAX volume
  
  int frequencies[] = {100, 500, 1000, 2000};
  
  for (int i = 0; i < 4; i++) {
    Serial.print("Testing frequency: ");
    Serial.print(frequencies[i]);
    Serial.println("Hz");
    
    player.writeRegister(0x00, 0x0820); // SM_TESTS | SM_SDINEW
    
    uint16_t freq_value;
    switch(frequencies[i]) {
      case 100: freq_value = 0x2A10; break;
      case 500: freq_value = 0x6A60; break;
      case 1000: freq_value = 0xAC45; break;
      case 2000: freq_value = 0x7D12; break;
      default: freq_value = 0x6A60;
    }
    
    player.writeRegister(0x05, freq_value);
    delay(2000);
    
    // Wyłącz test
    player.writeRegister(0x05, 0x0000);
    player.writeRegister(0x00, 0x0800);
    
    delay(500);
  }
}
//void listFiles(const char* dirname) {
  //File root = SD.open(dirname);
  //if (!root) {
    //Serial.println("Open folder error");
    //return;
  //}
  
  //File file = root.openNextFile();
  //while (file) {
    //Serial.print("  ");
    //Serial.print(file.name());
    //Serial.print(" (");
    //Serial.print(file.size());
    //Serial.println(" bytes)");
    //file = root.openNextFile();
  //}
//}

//void testPlayback() {
  //Serial.println("=== PLAYING TEST ===");
  
  //const char* filenames[] = {"test.mp3", "music.mp3", "1.mp3", "song.mp3"};
  
  //for (int i = 0; i < 4; i++) {
    //if (SD.exists(filenames[i])) {
      //Serial.print("Playing: ");
      //Serial.println(filenames[i]);
      
      //if (audioPlayer.playFile(filenames[i])) {
        //Serial.println("✅ Playback successful");
      //}else {
        //Serial.println("❌ Playback failed");
      //}
      //return;
    //}
  //}
  //Serial.println("Can't find any file MP3!");
//}

//void playFile(File file) {
  //const int bufferSize = 32;
  //uint8_t buffer[bufferSize];
  
  //unsigned long startTime = millis();
  
  //while (file.available()) {
    //int bytesRead = file.read(buffer, bufferSize);
    //audioPlayer.playChunk(buffer, bytesRead);
    
    //while (!digitalRead(VS1053_DREQ)) {
      //delay(1);
    //}
    
    //if (millis() - startTime > 2000) {
      //Serial.print(".");
      //startTime = millis();
    //}
    
    //delay(1);
  //}
  
  //Serial.println("\nEND OF FILE");
//}


void loop() {
  //audioPlayer.update();

  //if(digitalRead(BTN_PLAY) == LOW){
    //delay(50); // debounce
    //if(digitalRead(BTN_PLAY) == LOW){
      //if(audioPlayer.getState() == STATE_PLAYING){
        //audioPlayer.pause();
      //}else{
        //audioPlayer.resume();
      //}
      //while(digitalRead(BTN_PLAY) == LOW); // wait for low state
    //}
  //}

  //if(digitalRead(BTN_VOL_UP) == LOW){
    //delay(50);
    //if(digitalRead(BTN_VOL_UP) == LOW){
      //audioPlayer.volumeUp();
      //while(digitalRead(BTN_VOL_UP) == LOW);
    //}
  //}

  //if(digitalRead(BTN_VOL_DOWN) == LOW){
    //delay(50);
    //if(digitalRead(BTN_VOL_DOWN) == LOW){
      //audioPlayer.volumeDown();
      //while(digitalRead(BTN_VOL_DOWN) == LOW);
    //}
  //}
  //if(Serial.available()){
    //char cmd = Serial.read();
    //switch(cmd){
      //case '+': audioPlayer.volumeUp(); break;
      //case '-': audioPlayer.volumeDown(); break;
      //case 'p':
        //if(audioPlayer.getState() == STATE_PLAYING){
          //audioPlayer.pause();
        //}else{
          //audioPlayer.resume();
        //}
        //break;
      //case 's': audioPlayer.stop(); break;
      //case 'b': audioPlayer.startBluetooth(); break;
      //case 't': testPlayback(); break;
    //}
  //}
  //delay(100);
}
