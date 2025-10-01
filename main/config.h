#ifndef CONFIG_H
#define CONFIG_H

//-------------PINOUT----------------

// VS1053 AUDIO DECODER
#define VS1053_CS 4
#define VS1053_DCS 16
#define VS1053_DREQ 2
#define VS1053_RST 17

// SD CARD
#define SD_CS 5

// TFT DISPLAY (SPI)
#define TFT_CS 15
#define TFT_DC 21
#define TFT_RST 22

// ENCODER & BUTTONS
#define ENCODER_A 34
#define ENCODER_B 35
#define ENCODER_BIN 32
#define BTN_PLAY 33
#define BTN_VOL_UP 25
#define BTN_VOL_DOWN 26

//-------------SETTINGS----------------

// AUDIO
#define VOLUME_MIN 0
#define VOLUME_MAX 100
#define DEFAULT_VOLUME 50

//DISPLAY
#define SCREEN_WIDTH 240
#define SCREEN HEIGHT 320
#define TEXT_SIZE 2

//FILE SYS
#define MAX_FILES 100
#define MAX_FILENAME_LENGTH 50

//-------------DATA STRUCT----------------
enum PlayerState{
  STATE_STOPPED,
  STATE_PLAYING,
  STATE_PAUSED
};

enum AuidoSource{
  SOURCE_SD_CARD,
  SOURCE_BLUETOOTH
};

#endif
