#ifndef CONFIG_H
#define CONFIG_H

//-------------PINOUT----------------

// VS1053 AUDIO DECODER
#define VS1053_BCLK 26 // Bit Clock
#define VS1053_LRC 25 // Left/Right Clock
#define VS1053_DOUT 27 // Data out from ESP32 to VS1053

// PINOUT VS1053 SPI MODE
#define VS1053_CS 4 // XCS chip selection
#define VS1053_DCS 16   // XDCS data selection
#define VS1053_DREQ 2 // Data request
#define VS1053_RST 17 // XRESET


// SD CARD
#define SD_CS 5 // CS for SD CARD
#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23

// TFT DISPLAY (SPI)
#define TFT_CS 15
#define TFT_DC 21
#define TFT_RST 22

// ENCODER & BUTTONS
#define ENCODER_A 34
#define ENCODER_B 35
#define ENCODER_BTN 32
#define BTN_PLAY 33
#define BTN_VOL_UP 13
#define BTN_VOL_DOWN 14

//-------------SETTINGS----------------

// AUDIO FORMATS
#define SUPPORTED_FORMATS ".mp3 .wav"

// AUDIO QUALITY
#define SAMPLE_RATE 44100 // 44.1kHz
#define BIT_DEPTH 16 // 16 bit audio
#define I2S_BUFFER_SIZE 1024 // Buffer I2S

// AUDIO
#define VOLUME_MIN 0
#define VOLUME_MAX 255
#define DEFAULT_VOLUME 150

//DISPLAY
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TEXT_SIZE 2

//FILE SYS
#define MAX_FILES 100
#define MAX_FILENAME_LENGTH 50

//-------------DATA STRUCT----------------
enum AudioFormat{
  FORMAT_MP3,
  FORMAT_WAV,
  FORMAT_UNKNOWN
};
enum PlayerState{
  STATE_STOPPED,
  STATE_PLAYING,
  STATE_PAUSED
};

enum AudioSource{
  SOURCE_SD_CARD,
  SOURCE_BLUETOOTH
};

#endif
