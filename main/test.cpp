#include "test.h"

void checkMP3File(String filename) {
  Serial.println("=== FILE ANALYSIS ===");
  
  if(!SD.exists(filename)) {
    Serial.println("File does not exist");
    return;
  }

  File file = SD.open(filename);
  if(!file) {
    Serial.println("Cannot open file");
    return;
  }

  Serial.print("File: ");
  Serial.println(filename);
  Serial.print("Size: ");
  Serial.print(file.size());
  Serial.println(" bytes");

  // CHECK FILE VOLUME
  uint8_t header[12];
  if(file.read(header, 12) == 12) {
    Serial.print("First 12 bytes: ");
    for(int i=0; i<12; i++) {
      if(header[i] < 0x10) Serial.print("0");
      Serial.print(header[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  
  file.close();
  }
}

void testHeadphoneConnection() {
  Serial.println("Testing headphone connection...");
  
  // SET NORMAL VOLUME
  player.setVolume(65);
  
  Serial.println("You should hear audio now - check headphones");
  
  // SIMPLE TEST
  player.startSong();
  
  // TEST WITH SOME SINEWAVES
  testSimpleTone(1000); // 1kHz
  delay(2000);
  testSimpleTone(2000); // 2kHz  
  delay(2000);
  testSimpleTone(500);  // 500Hz
  delay(2000);
  
  // STOP TEST
  stopTestTone();
  Serial.println("Test tones finished - should be silent now");
}


void testSimpleTone(int freq) {
  Serial.print("Testing ");
  Serial.print(freq);
  Serial.println("Hz tone");
  
  player.writeRegister(SCI_MODE, 0x0820); // SM_TESTS | SM_SDINEW
  
  // SET TEST FREQUANCY 
  uint16_t freq_val = (uint16_t)((freq * 65536L) / 44100);
  player.writeRegister(SCI_AICTRL0, freq_val); // Lewy kanał
  player.writeRegister(SCI_AICTRL1, freq_val); // Prawy kanał
  
  // START TEST
  player.writeRegister(SCI_AIADDR, 0x4020);
  
  delay(2000);
}

void stopTestTone() {
  // STOP SINEWAVE TEST
  player.writeRegister(SCI_AIADDR, 0x0000); // Stop test
  player.writeRegister(SCI_MODE, 0x0800);   // Normal mode (tylko SM_SDINEW)
  delay(10);
}

void testVS1053Configurations() {
  Serial.println("=== TESTING VS1053 CONFIGURATIONS ===");
  
  // BASIC CONFIG
  Serial.println("Test 1: Basic configuration");
  player.softReset();
  delay(100);
  player.writeRegister(SCI_MODE, 0x0800); // SM_SDINEW
  player.writeRegister(SCI_CLOCKF, 0x8800); // Clock
  player.setVolume(0);
  playShortTest();
  delay(2000);

  // TEST2 WITH rESET
  Serial.println("Test 2: With reset");
  player.softReset();
  delay(100);
  player.writeRegister(SCI_MODE, 0x0804); // SM_SDINEW | SM_RESET
  delay(100);
  player.writeRegister(SCI_MODE, 0x0800); // SM_SDINEW
  player.writeRegister(SCI_CLOCKF, 0x8800);
  player.setVolume(0);
  playShortTest();
  delay(2000);

  // TEST3 WITH MORE MODIFICATIONS (bass/treble)
  Serial.println("Test 3: With bass/treble");
  player.softReset();
  delay(100);
  player.writeRegister(SCI_MODE, 0x0800);
  player.writeRegister(SCI_CLOCKF, 0x8800);
  player.writeRegister(SCI_BASS, 0x0000); // Flat response
  player.setVolume(0);
  playShortTest();
  delay(2000);
}

void playShortTest() {
  if(!SD.exists("/test.mp3")) return;
  
  File file = SD.open("/test.mp3");
  if(!file) return;
  
  player.startSong();
  
  // SEND FIRST 2000 bytes
  uint8_t buffer[32];
  unsigned long bytesSent = 0;
  
  while(file.available() && bytesSent < 2000) {
    int bytesRead = file.read(buffer, 32);
    bytesSent += bytesRead;
    
    player.playChunk(buffer, bytesRead);
    
    while(!digitalRead(VS1053_DREQ)) {
      delay(1);
    }
  }
  
  file.close();
  Serial.print("Short test completed - bytes sent: ");
  Serial.println(bytesSent);
}

void checkMP3Detection() {
  Serial.println("=== CHECKING MP3 DETECTION ===");
  
  if(!SD.exists("/test.mp3")) {
    Serial.println("test.mp3 not found");
    return;
  }

  // CONFIG VS1053
  configureVS1053BForHeadphones();
  player.startSong();
  
  File file = SD.open("/test.mp3");
  if(!file) {
    Serial.println("Cannot open file");
    return;
  }

  // SEND FIRST 500 bytes AND SEE WHAT HAPPENd
  uint8_t buffer[500];
  file.read(buffer, 500);
  
  Serial.println("Sending 500 bytes to VS1053...");
  
  for(int i = 0; i < 500; i += 32) {
    int chunkSize = min(32, 500 - i);
    player.playChunk(&buffer[i], chunkSize);
    
    while(!digitalRead(VS1053_DREQ)) {
      delay(1);
    }
  }
  
  file.close();
  
  // ★★★ CHECKING IF VS1035 IS BUSY DECODING ★★★
  Serial.println("Checking if VS1053 is busy decoding...");
  delay(1000);
  
  if(digitalRead(VS1053_DREQ) == LOW) {
    Serial.println("✅ VS1053 is BUSY (DREQ=LOW) - likely decoding MP3!");
  } else {
    Serial.println("❌ VS1053 is READY (DREQ=HIGH) - not decoding");
  }
}

void testMP3File(String filename) {
  if(!SD.exists(filename)) return;
  
  File file = SD.open(filename);
  if(!file) return;
  
  // Hard reset VS1053
  digitalWrite(VS1053_RST, LOW);
  delay(100);
  digitalWrite(VS1053_RST, HIGH);
  delay(100);
  
  // Basic configuration
  player.begin();
  player.writeRegister(SCI_MODE, 0x0800);
  player.writeRegister(SCI_CLOCKF, 0x8800);
  player.setVolume(0);
  player.startSong();
  
  // SEND ONLY 10 KB FOR TESTIUNG
  uint8_t buffer[32];
  unsigned long bytesSent = 0;
  
  while(file.available() && bytesSent < 10240) {
    int bytesRead = file.read(buffer, 32);
    bytesSent += bytesRead;
    
    player.playChunk(buffer, bytesRead);
    
    while(!digitalRead(VS1053_DREQ)) {
      delay(1);
    }
  }
  
  file.close();
  Serial.print("  Sent ");
  Serial.print(bytesSent);
  Serial.println(" bytes");
  
  // CHECKING IF VS1053 IS BUSY
  delay(500);
  if(digitalRead(VS1053_DREQ) == LOW) {
    Serial.println("  ✅ VS1053 is DECODING!");
  } else {
    Serial.println("  ❌ VS1053 is NOT decoding");
  }
}

void checkAudioOutput() {
  Serial.println("=== AUDIO OUTPUT CHECK ===");
  
  Serial.println("VS1053 Audio Output Pins:");
  Serial.println("LEFT  - Pin 46");
  Serial.println("RIGHT - Pin 39"); 
  Serial.println("GBUF  - Pin 42 (common/ground for headphones)");
  Serial.println("SPK   - Single output (may be connected to GBUF)");
  
  Serial.println("Headphone connection:");
  Serial.println("Connect headphones between SPK and GBUF");
  Serial.println("OR between LEFT/RIGHT and GBUF if SPK not available");
  
  Serial.println("Testing audio output with sine wave...");
  
  // Test LEFT channel
  Serial.println("Testing LEFT channel...");
  player.writeRegister(SCI_MODE, 0x0820);
  player.writeRegister(SCI_AICTRL0, 0x0C34); // 1000Hz to LEFT
  player.writeRegister(SCI_AICTRL1, 0x0000); // Mute RIGHT
  player.writeRegister(SCI_AIADDR, 0x4020);
  delay(2000);
  
  // Test RIGHT channel  
  Serial.println("Testing RIGHT channel...");
  player.writeRegister(SCI_AICTRL0, 0x0000); // Mute LEFT
  player.writeRegister(SCI_AICTRL1, 0x0C34); // 1000Hz to RIGHT
  delay(2000);
  
  // Test both channels
  Serial.println("Testing BOTH channels...");
  player.writeRegister(SCI_AICTRL0, 0x0C34); // 1000Hz to LEFT
  player.writeRegister(SCI_AICTRL1, 0x0C34); // 1000Hz to RIGHT
  delay(2000);
  
  player.writeRegister(SCI_AIADDR, 0x0000);
  player.writeRegister(SCI_MODE, 0x0800);
  
  Serial.println("Audio output test completed");
}

void checkAudioConfiguration() {
  Serial.println("=== AUDIO CONFIGURATION CHECK ===");
  
  // Sprawdź rejestr STATUS dla konfiguracji audio
  // SCI_STATUS zawiera informacje o wzmacniaczu i referencji
  
  Serial.println("Audio configuration tips:");
  Serial.println("1. GBUF must NOT be connected to ground");
  Serial.println("2. LEFT/RIGHT need coupling capacitors if GBUF not used");
  Serial.println("3. Check if SPK output is actually connected to GBUF");
  Serial.println("4. Verify headphone impedance (16-32Ω recommended)");
  
  Serial.println("Testing different volume levels...");
  
  for(int vol = 0; vol <= 100; vol += 25) {
    Serial.print("Volume: ");
    Serial.println(vol);
    player.setVolume(vol);
    
    // Krótki test tone
    player.writeRegister(SCI_MODE, 0x0820);
    player.writeRegister(SCI_AICTRL0, 0x0C34);
    player.writeRegister(SCI_AICTRL1, 0x0C34);
    player.writeRegister(SCI_AIADDR, 0x4020);
    delay(1000);
    player.writeRegister(SCI_AIADDR, 0x0000);
    player.writeRegister(SCI_MODE, 0x0800);
    
    delay(500);
  }
  
  player.setVolume(60);
}

void testAudioOutputModes() {
  Serial.println("=== TESTING AUDIO OUTPUT MODES ===");
  
  // Tryb 1: Normalny
  Serial.println("Mode 1: Normal output");
  configureVS1053ForMP3();
  playShortMP3Test();
  delay(2000);
  
  // BASS MODE
  Serial.println("Mode 2: With bass enhancement");
  player.writeRegister(SCI_BASS, 0x007A); // Bass boost
  playShortMP3Test();
  delay(2000);
  
  // DIFFERENT FREQUANCIES
  Serial.println("Mode 3: Different test frequencies");
  int freqs[] = {500, 1000, 2000, 4000};
  for(int i = 0; i < 4; i++) {
    Serial.print("Testing ");
    Serial.print(freqs[i]);
    Serial.println("Hz");
    
    player.writeRegister(SCI_MODE, 0x0820);
    uint16_t freq_val = (uint16_t)((freqs[i] * 65536L) / 44100);
    player.writeRegister(SCI_AICTRL0, freq_val);
    player.writeRegister(SCI_AICTRL1, freq_val);
    player.writeRegister(SCI_AIADDR, 0x4020);
    delay(1000);
    player.writeRegister(SCI_AIADDR, 0x0000);
    player.writeRegister(SCI_MODE, 0x0800);
    delay(500);
  }
}

void playShortMP3Test() {
  if(!SD.exists("/test.mp3")) return;
  
  File file = SD.open("/test.mp3");
  if(!file) return;
  
  player.startSong();
  
  uint8_t buffer[32];
  unsigned long bytesSent = 0;
  
  // SEND JUST 10KB FOR TEST
  while(file.available() && bytesSent < 10240) {
    int bytesRead = file.read(buffer, 32);
    bytesSent += bytesRead;
    
    player.playChunk(buffer, bytesRead);
    
    while(!digitalRead(VS1053_DREQ)) {
      delay(1);
    }
  }
  
  file.close();
  Serial.print("  Sent ");
  Serial.print(bytesSent);
  Serial.println(" bytes");
}

void diagnoseHeadphoneConnection() {
  Serial.println("=== HEADPHONE CONNECTION DIAGNOSIS ===");
  
  Serial.println("VS1053B has TWO possible audio outputs:");
  Serial.println("1. SPK output - single ended headphone output");
  Serial.println("2. LEFT/RIGHT + GBUF - differential output");
  
  Serial.println("For SPK output:");
  Serial.println("  Connect headphones between SPK and GND");
  Serial.println("  SPK may already be internally connected to GBUF");
  
  Serial.println("For differential output:");
  Serial.println("  Connect LEFT to tip, RIGHT to ring, GBUF to sleeve");
  Serial.println("  GBUF provides common voltage (~1.24V)");
  
  Serial.println("Troubleshooting:");
  Serial.println("✓ Check if headphones work with other devices");
  Serial.println("✓ Try different headphones");
  Serial.println("✓ Verify SPK/GBUF connection");
  Serial.println("✓ Check solder joints on audio outputs");
  
  Serial.println("Testing SPK vs LEFT/RIGHT outputs...");
  
  Serial.println("If you hear sound in ANY test, note which one works:");
}
