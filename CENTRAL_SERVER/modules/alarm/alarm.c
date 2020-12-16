#include <stdlib.h>
#include "alarm.h"

void turn_on_alarm(){
    system("omxplayer audio.mp3 > log_omxplayer.txt");
}

void turn_down_alarm(){
    system("pkill omxplayer");
}
