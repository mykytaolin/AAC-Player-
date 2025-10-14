#include "audio_player.h"
#include <SD.h>
#include <SPI.h>

AudioPlayer audioPlayer;

// Constructor
AudioPlayer::AudioPlayer() 
:player(VS1053_CS, VS1053_DCS, VS1053_DREQ){
  
  state = STATE_STOPPED;
  current_source = SOURCE_SD_CARD;
  current_volume = DEFAULT_VOLUME;
  current_format = FORMAT_UNKNOWN;
  
} 

bool AudioPlayer::begin(){
  Serial.println("Initializing high quality Audio Player (AAC/MP3)");

  /*       RESETING VS1053
---------------------------------------*/
  pinMode(VS1053_RST, OUTPUT);
  digitalWrite(VS1053_RST, LOW);
  delay(100);
  digitalWrite(VS1053_RST, HIGH);
  delay(100);
  
  player.begin();

  // Simple test - try to set volume and see if DREQ responds
  player.setVolume(0); // MAX volume
  
  // Check if DREQ pin responds (goes HIGH after initialization)
  delay(50);

  if (digitalRead(VS1053_DREQ) == HIGH) {
    Serial.println("VS1053B OK - DREQ responding");
  } else {
    Serial.println("VS1053B WARNING - DREQ not responding, but continuing...");
  }
  /*       Config headphones
---------------------------------------*/
  player.softReset();
  delay(100);
  
  player.writeRegister(0x0B, 0x0001); // Optimize for headphones
  
  player.setVolume(0);
  Serial.println("VS1053 is Configured for headphones");
  
  // Init SD card 
  if(!SD.begin(SD_CS)){
    Serial.println("SD card init failed");
    return false;
  }
  Serial.println("SD card OK");

  setVolume(80);

  Serial.println("MP3 Player is READY");
  Serial.println("Supported formats: MP3/WAV");
  
  return true;

}

bool AudioPlayer::isSupportedFormat(String filename){
  filename.toLowerCase();
  return filename.endsWith(".mp3") || filename.endsWith(".wav");
}

bool AudioPlayer::playFile(String filename){

  if(!isSupportedFormat(filename)){
    Serial.println("Format is not supported:" + filename);
    Serial.println("Supported: .mp3 , .wav");
    return false;
  }
  
  Serial.println("Playing" + filename);
  stop();

  File file = SD.open(filename);
  if(!file){
    Serial.println("Cannot open file" + filename);
    return false;
  }
  
  current_file = filename;

  if(filename.endsWith(".mp3")){
    current_format = FORMAT_MP3;
    Serial.println("Format: MP3");
  }else if(filename.endsWith(".wav")) {
    current_format = FORMAT_WAV;
    Serial.println("Format: WAV");
  }
  
  setVolume(current_volume);
    
  state = STATE_PLAYING;
  
  // Data stream to VS1053
  const int bufferSize = 32;
  uint8_t buffer[bufferSize];

  Serial.println("Playback STARTED");

  player.stopSong();
  delay(10);
  player.startSong();
  delay(10);

  while(file.available() && state == STATE_PLAYING){
    int bytesRead = file.read(buffer, bufferSize);

    // Sending data to VS1053
    player.playChunk(buffer, bytesRead);

    // Waiting for VS1053 to receive more data, DREQ = HIGH   
    while(!digitalRead(VS1053_DREQ)){
      delay(1); 
    }
    if(state != STATE_PLAYING) break;
    
    delay(1); // for stability
  }
  
  file.close();

  if(state == STATE_PLAYING){
    state = STATE_STOPPED;
    Serial.println("Playback FINISHED");
  }

  return true;
}

void AudioPlayer::pause(){
  if(state == STATE_PLAYING) {
    state = STATE_PAUSED;
    Serial.println("Playback PAUSED");
  }   
}

void AudioPlayer::resume(){
  if(state == STATE_PAUSED) {
    state = STATE_PLAYING;
    Serial.println("Playback RESUMED");
  }  
}

void AudioPlayer::stop(){
  state = STATE_STOPPED;
  Serial.println("Playback STOPPED"); 
}

void AudioPlayer::setVolume(int volume){
  current_volume = constrain(volume, VOLUME_MIN, VOLUME_MAX);
  int vs1053_volume = map(current_volume, VOLUME_MIN, VOLUME_MAX, 0, 255);
  player.setVolume(vs1053_volume);
  Serial.println("VOLUME: " + String(current_volume) + "%");
}

void AudioPlayer::volumeUp(){
  setVolume(current_volume + 5);  
}

void AudioPlayer:: volumeDown(){
  setVolume(current_volume - 5);  
}

void AudioPlayer::update(){};

//FUNCTIONS OF GETTING
PlayerState AudioPlayer::getState(){return state;}
AudioSource AudioPlayer::getSource(){return current_source;}
String AudioPlayer::getCurrentFile(){return current_file;}
AudioFormat AudioPlayer::getCurrentFormat(){return current_format;}
int AudioPlayer::getVolume(){ return current_volume;}

//BLUETOOTH
void AudioPlayer::startBluetooth(){
  Serial.println("Starting bluetooth A2DP...");
  a2dp_sink.start("AAC-HQ-Player");
  current_source = SOURCE_BLUETOOTH;
  state = STATE_PLAYING;
  Serial.println("Bluetooth is READY - connect from your phone");
}

void AudioPlayer::stopBluetooth(){
  a2dp_sink.stop();
  current_source = SOURCE_SD_CARD;
  state = STATE_STOPPED;
  Serial.println("Bluetooth STOPPED");
}
void AudioPlayer::configureForHeadphones() {
  // Soft reset
  player.softReset();
  delay(100);
  
  // Set volume to MAX (0 = max, 255 = min)
  player.setVolume(0);
  
  // Configure for headphone use on SPK output
  player.writeRegister(0x0B, 0x0000); // Set default
  delay(10);
  player.writeRegister(0x0B, 0x0001); // Lower drive current
  delay(10);
}

void AudioPlayer::checkConfiguration() {
  Serial.println("----VS1053B CONFIGURATION----");

  Serial.print("DREQ state: ");
  Serial.println(digitalRead(VS1053_DREQ));
  Serial.print("Current volume: ");
  Serial.println(current_volume);
  
  Serial.println("VS1053B basic check completed");
}
