#include "ConfigCommandHelper.h"

unsigned int ConfigCommandHelper::getValueOffset(
        const String& command, const String& option, char suffix) {
    const auto optlen = option.length();
    return (command.length() > optlen &&
            command.startsWith(option) && suffix == command.charAt(optlen))
        ? optlen + 1
        : 0;
}

bool ConfigCommandHelper::passPropertyChange(const String& command,
        String& property, const String& option) {
    const auto offset = getValueOffset(command, option, '=');
    if (offset > 0)  {
        property = command.substring(offset);
        return true;
    }
    return false;
}

bool ConfigCommandHelper::passPropertyDisplayOrChange(const String& command,
        String& property, const String& option) {
    if (passPropertyChange(command, property, option)) {
        return true;
    }
    else if (getValueOffset(command, option, '?') == command.length()) {
        Serial.println(property);
        return true;
    }
    return false;
}

bool ConfigCommandHelper::passPropertyChange(const String& command,
        int& property, const String& option) {
    const auto offset = getValueOffset(command, option, '=');
    if (offset > 0)  {
        property = command.substring(offset).toInt();
        return true;
    }
    return false;
}

bool ConfigCommandHelper::passPropertyDisplayOrChange(const String& command,
        int& property, const String& option) {
    if (passPropertyChange(command, property, option)) {
        return true;
    }
    else if (getValueOffset(command, option, '?') == command.length()) {
        Serial.println(property);
        return true;
    }
    return false;
}

bool ConfigCommandHelper::passPropertyChange(const String& command,
        IPAddress& property, const String& option) {
    const auto offset = getValueOffset(command, option, '=');
    if (offset > 0)  {
        property.fromString(command.substring(offset));
        return true;
    }
    return false;
}

bool ConfigCommandHelper::passPropertyDisplayOrChange(const String& command,
        IPAddress& property, const String& option) {
    if (passPropertyChange(command, property, option)) {
        return true;
    }
    else if (getValueOffset(command, option, '?') == command.length()) {
        Serial.println(property);
        return true;
    }
    return false;
}
