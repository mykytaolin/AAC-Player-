#include "audio_player.h"

AudioPlayer audioPlayer;

// Constructor
AudioPlayer::AudioPlayer(){
  audio = new Audio() // dynamic creation Audio objects
  state = STATE-STOPPED;
  current_source = SOURCE_SD_CARD;
  current_volume = 50;
  current_format = FORMAT_UNKNOWN;
}

//Destructor
AudioPlayer::~AudioPlayer(){
  if(audio){
    delete(audio);
  }
}

bool AudioPlayer::begin(){
  Serial.println("Initializing high quality Audio Player (AAC/MP3)");

  // Init SD card
  if(!SD.begin(SD_CS){
    Serial.println("SD card init failed");
    return false;
  }
  Serial.println(SD card OK);

  // Config output audio I2S to VS1053
  audio->setPinout(VS1053_BCLK, VS1053_LRC, VS1053_DOUT);

  // Set callback
  setuoAudioCallbacks();

  setVolume(DEFAULT_VOLUME);

  Serial.println("AAC/MP3 PLAYER READY");
  Serial.println("Supports formats MP3 AAC M4A WAV FLAC");
  
  return true;

}

// Setting callback func for info aboud audio
void AudioPlayer:setupAudioCallbacks(){
  audio->setNotifyCallback([this](const char* info) {
    this->audioInfo(info);
  });

  audio->setNotifyOnConnect([](const char* info) {
    Serial.printf("Connected %s\n", info);
  });
}

bool AudioPlayer::playFile(String filename){
    Serial.println("Playing" + filename);

    stop();

    // Audio Fromat Detection
    if(audio->connectoFS(SD, filename.c_str())) {
      current_file = filename;
      state = STATE_PLAYING;
      is_playing = true;

      if(filename.endWith(".aac") || filename.endWith(".m4a") {
        current_format = FORMAT_AAC;
        Serial.println("Format AAC (HIGH QUALITY)");
      }else if(filename.endWith(".mp3")) {
        current_format = FORMAT_MP3;
        Serial.println("Format MP3");
      }else if(filename.endWith(".wav")) {
        current_format = FORMAT_WAV;
        Serial.println("Format WAV (LOSSLESS)");
      }else{
        current_format = UNKNOWN_FORMAT;
        Serial.println("Format Unknown");
      }
      
      return true;
    }

    Serial.println("Failed to play:" + filename);
    return false;
}

void AudioPlayer::pause(){
  if(state == STATE_PLAYING) {
    audio->pauseResume();
    state = STATE_PAUSED;
    Serial.println("Playback PAUSED");
  }  
}

void AudioPlayer::resume(){
  if(state == STATE_PAUSED) {
    audio->pauseResume();
    state = STATE_PLAYING;
    Serial.println("Playback RESUMED");
  }  
}

void AudioPlayer::stop(){
  audio->stopSong();
  state = STATE_STOPPED;
  is_playing = false;
  Serial.println("Playback STOPPED"); 
}

void AudioPlayer::setVolume(int volume){
  current_volume = constrain(volume, VOLUME_MIN, VOLUME_MAX);
  int audio_volume = map(current_volume, VOLUME_MIN, VOLUME_MAX, 0, 21);
  audio->setVolume(audio_volume);
  Serial.println("VOLUME: " + String(current_volume) + "%");
}

void AudioPlayer:: volumeUP(){
  setVolume(current_volume + 5);  
}

void AudioPlayer:: volumeDown(){
  setVolume(current_volume - 5);  
}

//Callback info about audio
void AudioPlayer::audioInfo(const char* info){
  Serial.prinf("Audio info: ");
  Serial.println(info);

  // TODO display actualization (bitrate)
}

void AudioPlayer::update() {
  // Update audio state - calling in loop
  audio->loop();

  // Checking if song finished
  if(state == STATE_PLAYING && !audio->isRunning()) {
    state = STATE_STOPPED;
    is_playing = false;
    Serial.println("Playback FINISHED");

    // TODO autoplay next track
  }
}

//FUNCTIONS OF GETTING
PlayerState AudioPlayer::getState(){return state;}
AudioSurce AudioPlayer::getSource(){return current_source;}
String AudioPlayer::getCurrentFile(){return current_file;}
AudioFormat AudioPlayer::getCurrentFormat(){return current_format;}
bool AudioPlayer::isPlaying(){return is_playing;}
int AudioPlayer::getVolume(){ return current_volume;}

int AudioPlayer::getDuration(){
  return audio->getAudioFileDuration();
}

int AudioPlauer::getCurrentTime(){
  return audio->getAudioCurrentTime();
}

//BLUETOOTH
void AudioPlayer::startBluetooth(){
  Serial.println("Starting bluetooth A2DP...");
  a2dp_sink.start("AAC-HQ-Player");
  current_source = SOURCE_BLUETOOTH;
  state = STATE_PLAYING;
}

void AudioPlayer::stopBluetooth(){
  a2dp_sink.stop();
  current_source = SOURCE_SD_CARD;
}
  
