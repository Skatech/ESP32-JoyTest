#include <Arduino.h>
#include <ArduinoJson.h>

#include <FS.h>
#include <LittleFS.h>

#include "DeviceConfig.h"
#include "ConfigCommandHelper.h"

#define CONFIG_CAPACITY 320
#define CONFIGURATION_FILE_PATH "/config/device-config.json"

const String DeviceConfig::OPT_SSID = "ssid";
const String DeviceConfig::OPT_PASSWORD = "password";
const String DeviceConfig::OPT_HOSTNAME = "hostname";
const String DeviceConfig::OPT_ADDRESS = "address";
const String DeviceConfig::OPT_GATEWAY = "gateway";
const String DeviceConfig::OPT_SUBNET = "subnet";
const String DeviceConfig::OPT_DNS = "dns";

bool readJsonFile(const char *path, JsonDocument& doc) {
    File file = LittleFS.open(path, "r");
    if (file) {
        DeserializationError result = deserializeJson(doc, file);
        file.close();
        return result == DeserializationError::Ok;
    }
    return false;
}

bool writeJsonFile(const char *path, const JsonDocument& doc) {
    File file = LittleFS.open(path, "w");
    if (file) {
        const size_t size = serializeJson(doc, file);
        file.close();
        return size > 0;
    }
    return false;
}

bool DeviceConfig::load() {
    StaticJsonDocument<CONFIG_CAPACITY> doc;
    if (readJsonFile(CONFIGURATION_FILE_PATH, doc)) {
        ssid = doc[OPT_SSID].as<String>();
        password = doc[OPT_PASSWORD].as<String>();
        hostname = doc[OPT_HOSTNAME].as<String>();
        address.fromString(doc[OPT_ADDRESS].as<String>());
        gateway.fromString(doc[OPT_GATEWAY].as<String>());
        subnet.fromString(doc[OPT_SUBNET].as<String>());
        dns.fromString(doc[OPT_DNS].as<String>());
        return true;
    }
    else {
        ssid = password = hostname = "";
    }
    return false;
}

bool DeviceConfig::save() {
    StaticJsonDocument<CONFIG_CAPACITY> doc;
    doc[OPT_SSID] = ssid;
    doc[OPT_PASSWORD] = password;
    doc[OPT_HOSTNAME] = hostname;
    doc[OPT_ADDRESS] = address.toString();
    doc[OPT_GATEWAY] = gateway.toString();
    doc[OPT_SUBNET] = subnet.toString();
    doc[OPT_DNS] = dns.toString();
    
    Serial.print(doc.memoryUsage()); Serial.print(" bytes capacity used of "); Serial.println(doc.capacity());
    String s; serializeJson(doc, s); Serial.println(s);

    return writeJsonFile(CONFIGURATION_FILE_PATH, doc);
}

bool DeviceConfig::processCommand(const String& cmd) {
    if (cmd == "show-config") {
        const auto log = [] (const String& s, const String& v) {
            Serial.print(s); Serial.print(": "); Serial.println(v); };
        const auto loga = [] (const String& s, const IPAddress& v) {
            Serial.print(s); Serial.print(": "); Serial.println(v); };
        log(OPT_SSID, ssid);
        log(OPT_PASSWORD, password);
        log(OPT_HOSTNAME, hostname);
        loga(OPT_ADDRESS, address);
        loga(OPT_GATEWAY, gateway);
        loga(OPT_SUBNET, subnet);
        loga(OPT_DNS, dns);
    }
    else if (cmd == "save-config") {
        if (!save()) {
            Serial.println("Configuration writing FAILED");
        };
    }
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, ssid, OPT_SSID)) {}
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, password, OPT_PASSWORD)) {}
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, hostname, OPT_HOSTNAME)) {}
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, address, OPT_ADDRESS)) {}
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, gateway, OPT_GATEWAY)) {}
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, subnet, OPT_SUBNET)) {}
    else if (ConfigCommandHelper::passPropertyDisplayOrChange(cmd, dns, OPT_DNS)) {}
    else return false;
    return true;
}
