#include "config.h"
#include "storage.h"
#include "web_manager.h"
#include "usb_host_mesh.h"
#include "bot_logic.h"

void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n[SYS] Arrancando MeshBot en ESP32-S3 Mini...");

    // 1. Cargar configuración persistente
    Storage_Init();

    // 2. Inicializar puerto de comunicación con el nodo (Heltec o Faketec)
    USBHost_Init();

    // 3. Levantar Wi-Fi, DNS y Portal Cautivo Web
    WebManager_Init();

    Serial.println("[SYS] Sistema completamente inicializado y operativo.");
}

void loop() {
    // Mantener activo el servidor DNS del portal cautivo
    WebManager_Update();

    // Escuchar permanentemente mensajes del nodo por USB/UART y derivarlos al bot
    USBHost_Update(Bot_ProcessMessage);
}