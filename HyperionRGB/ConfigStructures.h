#ifndef ConfigStructures_h
#define ConfigStructures_h

#define CONFIG_START_ADDRESS 0
#define CONFIG_ACTIVE_VERSION 3

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
  uint8_t idleMode;
  uint32_t timeoutMs;
  boolean autoswitch;

  char spacer[59];
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

#endif
