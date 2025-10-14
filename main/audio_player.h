#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include <VS1053.h>
#include "BluetoothA2DPSink.h"
#include <config.h>
#include <SD.h>
#include <SPI.h>

class AudioPlayer{ // classs that manage audio system (VS1053 and Bluetooth)
  private:
    VS1053 player; // hardware decoder for MP3/WAV
    BluetoothA2DPSink a2dp_sink; // bluetooth reciever A2DP (sound from the phone)

    PlayerState state; // playing, pused, stoped
    AudioSource current_source; // SOURCE_SD_CARD or SOURCE_BLUETOOTH
    int current_volume; // volume from 0 to 100
    String current_file; // active playing file ( needed for SD)
    AudioFormat current_format;

  public:
  
    AudioPlayer(); // constructor - init variables, (not hardware)
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
    void playChunk(uint8_t* buffer, int bytes) {
      player.playChunk(buffer, bytes);
    }

    void update();
    void configureForHeadphones();
    void checkConfiguration();
    
  private:
    bool isSupportedFormat(String filename);
};

void configureVS1053BForHeadphones();
void checkVS1053BConfiguration();

extern AudioPlayer audioPlayer;

#endif
