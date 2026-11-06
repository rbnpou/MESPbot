#include "storage.h"
#include <Preferences.h>
#include "config.h"

Preferences preferences;
SystemConfig sysConfig;

void Storage_Init() {
    preferences.begin("bot-conf", false);
    
    // Cargar configuración general
    strlcpy(sysConfig.botName, preferences.getString("name", "MeshBot-S3").c_str(), sizeof(sysConfig.botName));
    sysConfig.powerMode = preferences.getInt("power", 0);
    sysConfig.autoReply = preferences.getBool("autoreply", true);
    sysConfig.connectionType = preferences.getInt("conn_type", 0);

    // Cargar los 10 comandos personalizados
    for (int i = 0; i < 10; i++) {
        String kKey = "kw_" + String(i);
        String rKey = "rs_" + String(i);
        String aKey = "ac_" + String(i);

        strlcpy(sysConfig.commands[i].keyword, preferences.getString(kKey.c_str(), "").c_str(), sizeof(sysConfig.commands[i].keyword));
        strlcpy(sysConfig.commands[i].response, preferences.getString(rKey.c_str(), "").c_str(), sizeof(sysConfig.commands[i].response));
        sysConfig.commands[i].active = preferences.getBool(aKey.c_str(), false);
    }
    
    preferences.end();
}

void Storage_Save() {
    preferences.begin("bot-conf", false);
    
    // Guardar configuración general
    preferences.putString("name", sysConfig.botName);
    preferences.putInt("power", sysConfig.powerMode);
    preferences.putBool("autoreply", sysConfig.autoReply);
    preferences.putInt("conn_type", sysConfig.connectionType);

    // Guardar los 10 comandos personalizados
    for (int i = 0; i < 10; i++) {
        String kKey = "kw_" + String(i);
        String rKey = "rs_" + String(i);
        String aKey = "ac_" + String(i);

        preferences.putString(kKey.c_str(), sysConfig.commands[i].keyword);
        preferences.putString(rKey.c_str(), sysConfig.commands[i].response);
        preferences.putBool(aKey.c_str(), sysConfig.commands[i].active);
    }
    
    preferences.end();
}