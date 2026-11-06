#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct CustomCommand {
    char keyword[32];
    char response[128];
    bool active;
};

struct SystemConfig {
    char botName[32];
    int powerMode;         // 0: Rendimiento, 1: Ahorro
    bool autoReply;
    int connectionType;    // 0: USB Nativo (Heltec T114), 1: UART / Serial (Faketec V3/V4)
    CustomCommand commands[10];
};

extern SystemConfig sysConfig;

#endif