# ESP32-async-ADC
The set of functions allow to use non-blocking/async ADC with version of Arduino core for ESP32 upper than 1.0.6.

### ALL CODE WAS "STOLEN" FROM ORIGINAL REPOSITORY https://github.com/espressif/arduino-esp32
### Please inform me if this code/publication/action violate any Copyright.
 
 Otherwise, fill free to use this code.
  
 Functions **adcStart/adcBusy/adcEnd** was removed after 1.0.4 RELEASE of "Arduino core for the esp32".
 But these functions are very useful for ASYNC/NO-WAIT ADC reading.
 I just copied code from Realease 1.0.4 of ESP32 Arduino to use it in my sketch.
  
 Just put both files to folder of your Arduino sketch and put #include "esp32-adc-nowait.h" in begin of your .ino file.
 
