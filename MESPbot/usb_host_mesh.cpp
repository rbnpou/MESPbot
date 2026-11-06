#include "usb_host_mesh.h"
#include "config.h"
#include "cdcacm_ftdi.h"

CDCacm MeshUSB;
HardwareSerial FaketecSerial(1); // UART1 para Faketec V3/V4

void USBHost_Init() {
    if (sysConfig.connectionType == 0) {
        Serial.println("[USB] Inicializando USB Host nativo para Heltec T114...");
        MeshUSB.begin(115200);
    } else {
        Serial.println("[UART] Inicializando puerto serie UART para Faketec V3/V4...");
        FaketecSerial.begin(115200, SERIAL_8N1, 4, 5); // Pines RX=4, TX=5 (ajustar si es necesario)
    }
}

void USBHost_Update(void (*onMessageCallback)(String)) {
    if (sysConfig.connectionType == 0) {
        if (MeshUSB.isReady() && MeshUSB.available()) {
            String incoming = MeshUSB.readStringUntil('\n');
            incoming.trim();
            if (incoming.length() > 0) {
                onMessageCallback(incoming);
            }
        }
    } else {
        if (FaketecSerial.available()) {
            String incoming = FaketecSerial.readStringUntil('\n');
            incoming.trim();
            if (incoming.length() > 0) {
                onMessageCallback(incoming);
            }
        }
    }
}

void USBHost_Send(String message) {
    if (sysConfig.connectionType == 0) {
        if (MeshUSB.isReady()) {
            MeshUSB.println(message);
        }
    } else {
        FaketecSerial.println(message);
    }
}