# Touch Brightness Control

Toggle On/Off with a long press (800ms)
Switch through 5 brightness levels (defined in usermod_touchbrightness.h, values 0-255) with a short (100ms) touch

### Note for ESP3266:
Since the ESP8266 doesn't natively support touch, it isn't working that reliable. Therefore by default, you can only turn on an off the light via touch. However this can be changed in the `usermod_touchbrightness.h` file by changing `changeBrightness` to true.


## Installation 

Copy 'usermod_touchbrightness.h' to the wled00 directory.  
in 'usermod_list.cpp' add this:

> #include "usermod_touchbrightness.h"
above "void registerUsermods()"

and

> usermods.add(new TouchBrightnessControl());
inside the "registerUsermods()" function
