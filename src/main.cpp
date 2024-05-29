#include <Arduino.h>
#include <LittleFS.h>
#include <esp32-adc-nowait.c>

// https://forum.arduino.cc/t/using-analogread-on-esp32-while-hosting-a-web-server/955790
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// AsyncWebServer server(80);
// AsyncWebSocket ws("/ws");

#define  LED_BUILTIN    2
#define  PIN_AXIS_X     A6
#define  PIN_AXIS_Y     A7
#define  PIN_BUTTON     32

#include "SerialCommand.h"
#include "DeviceConfig.h"
#include "network.h"
#include "webui.h"
#include "ConfigCommandHelper.h"

SerialCommand scmd(64);
DeviceConfig config;

uint16_t lastX, lastY, lastB;

unsigned long lastsend;
uint32_t between;
int period = 20;
int webui = 1;

// bool sendOrBroadcastJoyState(uint32_t addr = UINT32_MAX) {
//     if (WiFi.isConnected() && wsServer.count() > 0) {
//         String message = String(">joy:") + lastX + ',' + lastY + ',' + lastB;
//         if (addr == UINT32_MAX) {
//             if (wsServer.availableForWriteAll())
//                 wsServer.printfAll(message.c_str());
//         }
//         else if (wsServer.availableForWrite(addr))
//             wsServer.printf(addr, message.c_str());
//     }
//     return true;
// }

bool sendOrBroadcastJoyState(uint8_t addr = 0xff) {
    return webSocketSendOrBroadcastText(String(">joy:") + lastX + ',' + lastY + ',' + lastB, addr);
}

void processCommand(const String& cmd) {
    if (config.processCommand(cmd)) {
    }
    else if (cmd == "show-status") {
        Serial.println("Device MAC Address: " + WiFi.macAddress());
        Serial.println("Connection state: " + (WiFi.status() == WL_CONNECTED
            ? ("connected (" + WiFi.localIP().toString())
            : ("disconnected (" + String(WiFi.status()))) + ')');
        Serial.println(String("JoyState: ") + lastX + ',' + lastY + ',' + lastB);
        Serial.println(String("Upd Time: ") + between);
    }
    else if (cmd == "list-networks") {
        const int8_t count = WiFi.scanNetworks();
        for (int8_t n = 0; n < count; n++) {
            Serial.println(WiFi.SSID(n));
        }
    }
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, period, "period")){
    }
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, webui, "webui")){
    }
    else Serial.println(
        "Unknown command. Commands: show-status, list-networks, show-config, save-config, option?, option=VALUE");
    Serial.println();
}

void serialEvent() {
    if (scmd.update()) {
        processCommand(scmd.value());
        scmd.clear();
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(SERIAL_SPEED);
    Serial.println("\n\n\n\n\nESP32 - JoyTester\nInitializing:");
    Serial.println(String("Joy X-line: D") + PIN_AXIS_X);
    Serial.println(String("Joy Y-line: D") + PIN_AXIS_Y); 
    Serial.println(String("Joy S-line: D") + PIN_BUTTON); 

    Serial.print("  filesystem... ");
    if(LittleFS.begin()) {
        Serial.println("OK");
    }
    else {
        Serial.println("FAILED, SYSTEM HALTED");
        while(true);
    }

    Serial.print("  configuration... ");
    Serial.println(config.load()
        ? "OK"
        : "FAILED, defaults loaded");
    
    Serial.print("  network... ");
    if (initNetwork(config)) {
        Serial.println("OK");
    }
    else {
        Serial.println("FAILED");
        return;
    }

    Serial.print("  connecting... ");
    if (beginConnect(config.ssid, config.password)) {
        Serial.println("OK");
    }
    else {
        Serial.println("FAILED");
        return;
    }

    initWebUI();

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // Setup ADC
    adcAttachPin(PIN_AXIS_X);
    adcAttachPin(PIN_AXIS_Y);
    analogReadResolution(10);
}

void loop() {
    static unsigned long lastupd = 0;
    static uint8_t adcchan = 0;    
    static uint16_t x;

    unsigned long now = millis();

    if (adcchan == 0 || !adcBusy(adcchan)) {
        if (adcchan == PIN_AXIS_X) {
            x = adcEnd(adcchan);
            adcStart(adcchan = PIN_AXIS_Y);
        }
        else if (adcchan == PIN_AXIS_Y) {
            const uint16_t y = 1023 - adcEnd(adcchan);
            adcchan = 0;

            const uint16_t b = digitalRead(PIN_BUTTON) ? 0 : 1 + lastB;
            if (x != lastX || y != lastY || b != lastB) {
                lastX = x;
                lastY = y;
                lastB = b;

                between = now - lastsend;
                lastsend = now;

                sendOrBroadcastJoyState();
            }
        }
        else if (now - lastupd >= period) {
            lastupd = now;
            adcStart(adcchan = PIN_AXIS_X);
        }
    }

    if (watchConnection()) {
        if (webui)
            loopWebUI();
    }
}
