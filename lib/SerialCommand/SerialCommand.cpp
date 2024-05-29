#include "SerialCommand.h"

SerialCommand::SerialCommand(int reserve) {
    _input.reserve(reserve);
}

void SerialCommand::clear() {
    _input = "";
    _ready = false;
}

bool SerialCommand::update() {
    if (_ready) {
        clear();
    }
    while(!_ready && Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (_input.length()) {
                _ready = true;
            }
        }
        else if (c == 0x08) {
            if (_input.length()) {
                _input.remove(_input.length() - 1);
            }
        }
        else if (c > 0x1f) {
            _input += c;
        }
        else _input += '?';
    }
    return _ready;
}

const String& SerialCommand::value() const {
    return _input;
}

