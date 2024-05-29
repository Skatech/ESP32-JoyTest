#ifndef ConfigCommandHelper_h
#define ConfigCommandHelper_h

#include <Arduino.h>
#include <WiFi.h>

class ConfigCommandHelper {
public:
    // returns value position in command string when command and suffix matching, otherwise zero
    static unsigned int getValueOffset(
        const String& command, const String& option, char suffix);
    // returns true when property change command is detected and performed, otherwise false
    static bool passPropertyChange(const String& command,
        String& property, const String& option);
    // returns true when property display or change command is detected and performed, otherwise false
    static bool passPropertyDisplayOrChange(const String& command,
        String& property, const String& option);
    static bool passPropertyChange(const String& command,
            int& property, const String& option);
    static bool passPropertyDisplayOrChange(const String& command,
            int& property, const String& option);
    static bool passPropertyChange(const String& command,
        IPAddress& property, const String& option);
    static bool passPropertyDisplayOrChange(const String& command,
        IPAddress& property, const String& option);
};

#endif
