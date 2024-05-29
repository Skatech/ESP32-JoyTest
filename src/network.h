#ifndef network_h
#define network_h

#include <Arduino.h>
// #include <esp_wifi.h>
#include <WiFi.h>
// #include <NetBIOS.h>
#include <ESPmDNS.h>

#include "DeviceConfig.h"


bool initNetBIOS(const String& hostname) {
    // if(hostname.length()) {
    //     return NBNS.begin(hostname.c_str());
    // }
    // NBNS.end();
    if(hostname.length()) {
        return MDNS.begin(hostname.c_str()); // MDNS responder started
    }
    MDNS.end();
    return true;
}

bool initNetwork(const DeviceConfig& config) {
    // esp_wifi_set_ps(WIFI_PS_NONE);

    return (!WiFi.isConnected() || WiFi.disconnect()) && WiFi.mode(WIFI_STA)
        && WiFi.config(config.address, config.gateway, config.subnet, config.dns)
        && WiFi.hostname(config.hostname) && initNetBIOS(config.hostname);

    // return WiFi.disconnect() && WiFi.mode(WIFI_STA)
    //     && WiFi.config(config.address, config.gateway, config.subnet, config.dns)
    //     && WiFi.hostname(config.hostname) && initNetBIOS(config.hostname);
}

bool beginConnect(const String& ssid, const String& password) {
    if (WiFi.disconnect() && ssid.length()) {
        const auto status = WiFi.begin(ssid, password);
        return status == WL_DISCONNECTED || status == WL_CONNECTED;
    }
    return false;
}



bool watchConnection() {
    static bool connected = false;
    const auto status = WiFi.status();
    if (connected) {
        if (status != WL_CONNECTED) {
            connected = false;
            digitalWrite(LED_BUILTIN, HIGH); // ESP8266 invert(LOW)
            Serial.println("Connection LOST");
        }
    }
    else {
        if (status == WL_CONNECTED) {
            connected = true;
            digitalWrite(LED_BUILTIN, LOW);
            Serial.print("Connection estabilished, address: ");
            Serial.println(WiFi.localIP());
        }
    }
    return connected;
}

#endif