#include "led.h"

void InitPin(u32 pin,u32 mode){
    pinMode(pin,mode);
}

int SetPinHigh(u32 pin){
    digitalWrite(pin,1);
    return digitalRead(pin)==1?1:0;
}

int SetPinLow(u32 pin){
    digitalWrite(pin,0);
    g_LedFlag = 0;
    return digitalRead(pin)==0?1:0;
}

int SetPinRevers(u32 pin){
    int t = digitalRead(pin);
    if(t > 0){
        digitalWrite(pin,0);
        return digitalRead(pin)==0?1:0;
    }
    else if(t == 0){
        digitalWrite(pin,1);
        return digitalRead(pin)==1?1:0;
    }
}
