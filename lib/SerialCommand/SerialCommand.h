#ifndef SerialCommand_h
#define SerialCommand_h

#include <Arduino.h>

class SerialCommand {
public:
    SerialCommand(int reserve);
    void clear();
    bool update();
    const String& value() const;

private:
    String _input;
    bool _ready;
};

#endif
