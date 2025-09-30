#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

// SD CARD CONNECTION SETUP
//------------------------------------------------
#define SD_CS 5 // GPIO5 - CS for SD card
#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23 
//------------------------------------------------
// VS1053 CONNECTION SETUP
#define VS1053_CS 4 // Chip select
#define VS1053_DCS 16 // DATA/COMMAND SELECT
#define VS1053_DREQ 2 // DATA REQUEST (interuption)
//------------------------------------------------
void disableUnusedServices(){
//  WIFI.mode(WIFI_OFF);
  btStop();
}
void setup() {
  Serial.begin(115200);
  delay(1000);

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
  digitalWrite(2, !digitalRead(2));
  delay(1000);
  Serial.println("Blink...");
}
