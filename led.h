#include <wiringPi.h>
#include <stdio.h>

#define u32 unsigned int 
#define LED1 0
#define LED2 1
//B6 语音开  --- Pin2 PC9
//B7 语音关  --- Pin5 PC6
#define Ctr_SoundHigh 2
#define Ctr_SoundLow 5

void InitPin(u32 pin,u32 mode);
int SetPinHigh(u32 pin);//针脚取高
int SetPinLow(u32 pin);//针脚取低
int SetPinRevers(u32 pin);//针脚取反
