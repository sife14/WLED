# publish button state to mqtt

This usermod publishes a custom message to a custom topic when the beginning of a button press is detected.

## Installation 

Copy `mqtt_button_input.h` to the wled00 directory.  

Add `#include "usermod_touchbrightness.h"` to `usermods_list.cpp`.

Add `usermods.add(new TouchBrightnessControl());` to `usermods_list.cpp`.

