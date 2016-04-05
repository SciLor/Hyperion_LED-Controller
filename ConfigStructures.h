#ifndef ConfigStructures_h
#define ConfigStructures_h

#define CONFIG_START_ADDRESS 0
#define CONFIG_ACTIVE_VERSION 2

typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;  
} ConfigIP;

typedef struct {
  char ssid[32];
  char spacer1[32];
  char password[64];
  char spacer2[64];
  
  ConfigIP ip;
  ConfigIP subnet;
  ConfigIP dns;
  char spacer3[16];
  
  char hostname[32];
  char spacer4[128];
} ConfigWifi;

typedef struct {
  uint8_t chipset;
  uint8_t colorOrder;
  uint8_t dataPin;
  uint8_t clockPin;
  uint16_t count;
  uint8_t idleMode;

  char spacer[32];
} ConfigLed;

typedef struct {
  uint16_t jsonServer;
  uint16_t udpLed;
  
  char spacer[32];
} ConfigPort;

typedef struct {
  uint8_t version;
  ConfigWifi wifi;
  ConfigLed led;
  ConfigPort ports;
} ConfigStruct;

enum Chipset {
  SPI_LPD8806,
  SPI_WS2801,
  SPI_WS2803,
  SPI_SM16716,
  SPI_P9813,
  SPI_APA102,
  SPI_DOTSTAR
};

#endif
