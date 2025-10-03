#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include <Audio.h>
#include <VS1053Plugin.h>
#include "BluetoothA2DPSink.h"
#include <config.h>

class AudioPlayer{ // classs that manage audio system (VS1053 and Bluetooth)
  private:
    //VS1053Plugin vs1053_plugin; // hardware decoder VS1053 for different codecs
    Audio* audio;
    BluetoothA2DPSink a2dp_sink; // bluetooth reciever A2DP (sound from the phone)

    PlayerState state; // playing, pused, stoped
    AudioSource current_source; // SOURCE_SD_CARD or SOURCE_BLUETOOTH
    int current_volume; // volume from 0 to 100
    String current_file; // active playing file ( needed for SD)
    AudioFormat current_format;
    bool is_playing = false;

  public:
  
    AudioPlayer(); // constructor - init variables, (not hardware)
    ~AudioPlayer(); // destructor - freeing mem   
    
    bool begin(); // begin() - init hardware (VS1053, SD card) - calling in setup()

    // SD CARD PLAYBACK
    bool playFile(String filename); // playing file 
    void pause();
    void resume();
    void stop();

    // VOOLUME CONTROl
    void setVolume(int volume);
    void volumeUp();
    void volumeDown();
    int getVolume();

    //BLUETOOTH
    void startBluetooth();
    void stopBluetooth();

    //STATE MANAGER
    PlayerState getState();
    AudioSource getSource();
    String getCurrentFile();
    AudioFormat getCurrentFormat();
    bool isPlaying();
    int getDuration();
    int getCurrentTime();
    
    // Bluetooth
    void startBluetooth();
    void stopBluetooth();

    // Callback function for Audio library
    void audioInfo(const char *info);
    void audioEofSpeech(const char *info);
    void audioShowstreamtitle(const char *info);

    void update();
    
  private:
    void setupAudioCallbacks();
};

extern AudioPlayer audioPlayer;

#endif
