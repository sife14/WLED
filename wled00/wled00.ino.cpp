# 1 "C:\\Users\\sifem\\AppData\\Local\\Temp\\tmpn7mwm4zx"
#include <Arduino.h>
# 1 "C:/Users/sifem/Documents/Code/WledTouch/WLED/wled00/wled00.ino"
# 13 "C:/Users/sifem/Documents/Code/WledTouch/WLED/wled00/wled00.ino"
#include "wled.h"
void setup();
void loop();
#line 15 "C:/Users/sifem/Documents/Code/WledTouch/WLED/wled00/wled00.ino"
void setup() {
  WLED::instance().setup();
}

void loop() {
  WLED::instance().loop();
}