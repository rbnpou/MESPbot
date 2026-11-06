#ifndef USB_HOST_MESH_H
#define USB_HOST_MESH_H

#include <Arduino.h>

void USBHost_Init();
void USBHost_Update(void (*onMessageCallback)(String));
void USBHost_Send(String message);

#endif