#include "bot_logic.h"
#include "config.h"
#include "usb_host_mesh.h"

String parseTelemetryTags(String responseTemplate) {
    String output = responseTemplate;
    output.replace("{heap}", String(ESP.getFreeHeap()) + " B");

    unsigned long totalSeconds = millis() / 1000;
    unsigned long mins = totalSeconds / 60;
    unsigned long secs = totalSeconds % 60;
    output.replace("{uptime}", String(mins) + "m " + String(secs) + "s");

    output.replace("{name}", String(sysConfig.botName));
    output.replace("{power}", sysConfig.powerMode == 0 ? "Rendimiento" : "Ahorro");
    return output;
}

void Bot_ProcessMessage(String msg) {
    Serial.printf("[BOT] Mensaje recibido: %s\n", msg.c_str());

    if (!sysConfig.autoReply) return;

    String lowerMsg = msg;
    lowerMsg.toLowerCase();
    lowerMsg.trim();

    if (lowerMsg == "!ping" || lowerMsg.indexOf("ping") != -1) {
        USBHost_Send("pong! [" + String(sysConfig.botName) + "] - Heap: {heap}");
        return;
    } 
    else if (lowerMsg == "!estado" || lowerMsg == "estado") {
        String status = "SYS OK | Bot: {name} | Uptime: {uptime} | Memoria: {heap}";
        USBHost_Send(parseTelemetryTags(status));
        return;
    }
    else if (lowerMsg == "!ayuda" || lowerMsg == "ayuda") {
        USBHost_Send("Comandos: !estado, !ping y frases clave configuradas.");
        return;
    }

    for (int i = 0; i < 10; i++) {
        if (sysConfig.commands[i].active && strlen(sysConfig.commands[i].keyword) > 0) {
            String kw = String(sysConfig.commands[i].keyword);
            kw.toLowerCase();
            
            if (lowerMsg.indexOf(kw) != -1) {
                String finalResponse = parseTelemetryTags(String(sysConfig.commands[i].response));
                USBHost_Send(finalResponse);
                break; 
            }
        }
    }
}