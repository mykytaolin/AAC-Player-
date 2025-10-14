#ifndef CONFIG_H
#define CONFIG_H

// Registers SCI VS1053
#define SCI_MODE 0x00
#define SCI_STATUS 0x01
#define SCI_BASS 0x02
#define SCI_CLOCKF 0x03
#define SCI_DECODE_TIME 0x04
#define SCI_AUDATA 0x05
#define SCI_WRAM 0x06
#define SCI_WRAMADDR 0x07
#define SCI_HDAT0 0x08
#define SCI_HDAT1 0x09
#define SCI_AIADDR 0x0A
#define SCI_VOL 0x0B
#define SCI_AICTRL0 0x0C
#define SCI_AICTRL1 0x0D
#define SCI_AICTRL2 0x0E
#define SCI_AICTRL3 0x0F

// Mode register bits
#define SM_SDINEW 0x0800
#define SM_RESET 0x0004
#define SM_TESTS 0x0020

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
