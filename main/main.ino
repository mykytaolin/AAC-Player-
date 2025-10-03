
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
#include "BluetoothA2DPSink.h"
//--------------------------------
//----------INTERFACE-----------//
#include <Encoder.h>
//--------------------------------
#include "config.h"
#include "display_manager.h" // display manager
#include "audio_player.h" //audio manager
#include "file_manager.h"
#include "interface_manager.h"

//------------------------------------------------
void disableUnusedServices(){
//  WIFI.mode(WIFI_OFF);
  btStop();
}

VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ, VS1053_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);

  if(player.begin(115200){
    Serial.println("VS1053 was FOUND");
  }else{
    Serial.println("VS1053 NOT FOUND");
  }
  
  Serial.println("----ESP32 DevKitC Test -----");
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Chip Cores: ");
  Serial.println(ESP.getChipCores());
  Serial.print("Flash Size: ");
  Serial.println(ESP.getFlashChipSize());
  Serial.print("PSRAM Size: ");
  Serial.println(ESP.getPsramSize());

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.println("GPIO 2 (LED) ON");
}

// the loop function runs over and over again forever
void loop() {
}
