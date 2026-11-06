#include "web_manager.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "storage.h"

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}
  bool canHandle(AsyncWebServerRequest *request) { return true; }
  void handleRequest(AsyncWebServerRequest *request) { request->redirect("http://192.168.4.1/"); }
};

void WebManager_Init() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("MESPbot-Setup");

    dnsServer.start(DNS_PORT, "*", apIP);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
        html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
        html += "<title>Configuración MeshBot</title>";
        html += "<style>";
        html += "body { background-color: #121212; color: #e0e0e0; font-family: sans-serif; padding: 10px; text-align: center; }";
        html += ".card { background: #1e1e1e; padding: 15px; border-radius: 8px; max-width: 500px; margin: auto; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }";
        html += "input, select { width: 92%; padding: 8px; margin: 6px 0; background: #2d2d2d; border: 1px solid #444; color: #fff; border-radius: 4px; }";
        html += "input[type='submit'] { background: #00acc1; color: white; border: none; font-weight: bold; cursor: pointer; padding: 12px; margin-top: 15px; }";
        html += "fieldset { border: 1px solid #444; border-radius: 6px; margin: 10px 0; padding: 10px; text-align: left; }";
        html += "legend { color: #00acc1; font-weight: bold; font-size: 0.9em; }";
        html += "</style></head><body>";
        html += "<div class='card'>";
        html += "<h2>MeshBot Config</h2>";
        html += "<form action='/save' method='POST'>";
        
        html += "<fieldset><legend>Ajustes Generales & Conexión</legend>";
        html += "Nombre del Bot:<br><input type='text' name='name' value='" + String(sysConfig.botName) + "'><br>";
        html += "Tipo de Hardware:<br><select name='conn_type'>";
        html += "<option value='0'" + String(sysConfig.connectionType == 0 ? " selected" : "") + ">Heltec T114 (USB Nativo)</option>";
        html += "<option value='1'" + String(sysConfig.connectionType == 1 ? " selected" : "") + ">Faketec V3 / V4 (UART Serial)</option>";
        html += "</select><br>";
        html += "Modo de Energía:<br><select name='power'>";
        html += "<option value='0'" + String(sysConfig.powerMode == 0 ? " selected" : "") + ">Alto Rendimiento</option>";
        html += "<option value='1'" + String(sysConfig.powerMode == 1 ? " selected" : "") + ">Ahorro de Batería</option>";
        html += "</select><br>";
        html += "Respuesta Automática:<br><select name='autoreply'>";
        html += "<option value='1'" + String(sysConfig.autoReply ? " selected" : "") + ">Habilitado</option>";
        html += "<option value='0'" + String(!sysConfig.autoReply ? " selected" : "") + ">Deshabilitado</option>";
        html += "</select></fieldset>";

        html += "<fieldset><legend>Comandos Avanzados & Telemetría</legend>";
        html += "<p style='font-size: 0.75em; color: #888;'>Tags: <b>{heap}</b>, <b>{uptime}</b>, <b>{name}</b>, <b>{power}</b></p>";
        for (int i = 0; i < 10; i++) {
            String activeChecked = sysConfig.commands[i].active ? "checked" : "";
            html += "<div style='margin-bottom: 8px; border-bottom: 1px solid #333; padding-bottom: 5px;'>";
            html += "<b>Cmd #" + String(i+1) + "</b> Activo: <input type='checkbox' name='act_" + String(i) + "' value='1' " + activeChecked + "><br>";
            html += "Frase clave: <input type='text' name='kw_" + String(i) + "' value='" + String(sysConfig.commands[i].keyword) + "' placeholder='ej. clima'><br>";
            html += "Respuesta: <input type='text' name='rs_" + String(i) + "' value='" + String(sysConfig.commands[i].response) + "' placeholder='ej. Clima OK {heap}'>";
            html += "</div>";
        }
        html += "</fieldset>";

        html += "<input type='submit' value='Guardar Configuración y Reiniciar'>";
        html += "</form></div></body></html>";
        
        request->send(200, "text/html", html);
    });

    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("name", true)) {
            strlcpy(sysConfig.botName, request->getParam("name", true)->value().c_str(), sizeof(sysConfig.botName));
        }
        if (request->hasParam("conn_type", true)) {
            sysConfig.connectionType = request->getParam("conn_type", true)->value().toInt();
        }
        if (request->hasParam("power", true)) {
            sysConfig.powerMode = request->getParam("power", true)->value().toInt();
        }
        if (request->hasParam("autoreply", true)) {
            sysConfig.autoReply = (request->getParam("autoreply", true)->value() == "1");
        }

        for (int i = 0; i < 10; i++) {
            String aKey = "act_" + String(i);
            String kKey = "kw_" + String(i);
            String rKey = "rs_" + String(i);

            sysConfig.commands[i].active = request->hasParam(aKey, true);
            
            if (request->hasParam(kKey, true)) {
                strlcpy(sysConfig.commands[i].keyword, request->getParam(kKey, true)->value().c_str(), sizeof(sysConfig.commands[i].keyword));
            }
            if (request->hasParam(rKey, true)) {
                strlcpy(sysConfig.commands[i].response, request->getParam(rKey, true)->value().c_str(), sizeof(sysConfig.commands[i].response));
            }
        }

        Storage_Save();

        request->send(200, "text/html", "<h3>¡Configuración guardada con éxito! Reiniciando dispositivo...</h3>");
        delay(2000);
        ESP.restart();
    });

    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server.begin();
}

void WebManager_Update() {
    dnsServer.processNextRequest();
}
