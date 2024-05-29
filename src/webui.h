#ifndef webui_h
#define webui_h

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

WebServer webServer(80);
WebSocketsServer wsServer(81);
// WebSocketsServer wsServer(80, "/ws");

bool sendOrBroadcastJoyState(uint8_t addr);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_DISCONNECTED) {
        Serial.printf("[%u] Disconnected!\n", num);
    }
    else if (type == WStype_CONNECTED) {
        IPAddress ip = wsServer.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        wsServer.sendTXT(num, "Connected");
        sendOrBroadcastJoyState(num);
    }
    else if (type == WStype_TEXT) {
        Serial.printf("[%u] Received text: %s\n", num, payload);
        // String echoMessage = "Received:  " + String((char*)payload);
        // wsServer.sendTXT(num, echoMessage);
        wsServer.broadcastTXT((char*)payload);
    }
    else if (type == WStype_BIN) {
            Serial.printf("[%u] get binary length: %u\n", num, length);
            // hexdump(payload, length);
            // webSocket.sendBIN(num, payload, length);
    }
    else if (type == WStype_PONG) {
    }
    else {
        Serial.printf("[%u] Unsupported message type: %u\n", num, type);
        wsServer.sendTXT(num, "Unsupported message type");
    }
}

bool webSocketSendOrBroadcastText(const String& payload, uint8_t addr = 0xff) {
    String& data = const_cast<String&>(payload);
    if (WiFi.isConnected()) { //wsServer.connectedClients() > 0
        return (addr == 0xff)
            ? wsServer.broadcastTXT(data)
            : wsServer.sendTXT(addr, data);
    }
    return false;
}

void initWebUI() {
    wsServer.onEvent(webSocketEvent);
    wsServer.begin();

    webServer.on("/", HTTP_GET, []() {        
        webServer.sendHeader("Location", "http://" + webServer.hostHeader() + "/index.html", true);
        webServer.send(302, "text/plain", "Redirecting to index page");
    });
    webServer.serveStatic("/", LittleFS, "/webui/", "no-cache"); //"max-age=3600" or "no-cache"
    webServer.begin();
}

void loopWebUI() {
    webServer.handleClient();
    wsServer.loop();
}

#endif