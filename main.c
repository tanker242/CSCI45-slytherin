#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>
//#include <math.h>
//#include <unistd.h>
#include <softPwm.h>
#define PCF 120
#define IR 0
#define RELAY 3
#define LedPinRed 1  
#define LedPinGreen  4
#define LedPinBlue   5
int armed=0;
#define threshold 35
float root(float n) {
    n=1.0f/n;
    long i;
    float x, y;
    x=n*0.5f;
    y=n;
    i=*(long*)&y;
    i=0x5f3759df-(i>>1);
    y=*(float*)&i;
    y=y*(1.5f-(x*y*y));
    return y;
}
void trigger(){
	armed^=1;
	if(armed) softPwmWrite(LedPinRed,0xff);
	else softPwmWrite(LedPinRed,0x00); 
	//printf("ARM v%i\n",armed);
	delay(150);
}
int sound_detect(){
	int temp;
	int store=0;
	float out;
	for(int k=0;k<100;k++){
		temp=analogRead(PCF+0)-127;
		store+=temp*temp;
	}
	out=store;
	out/=100;
	out=root(out);
	return (out>threshold);
}
int main(){
	int alarm=0;
	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}
	if(wiringPiISR(IR, INT_EDGE_FALLING, &trigger) == -1){
		printf("setup ISR failed !");
		return 1;
			digitalWrite(RELAY,LOW);
	}
	pcf8591Setup (PCF, 0x48);
	pinMode(RELAY, OUTPUT);
	softPwmCreate(LedPinRed,  0, 100);
	softPwmCreate(LedPinGreen,0, 100);
	softPwmCreate(LedPinBlue, 0, 100);
	digitalWrite(RELAY,HIGH);
	while(1){
		//armed^=ir_detect();
		if(armed&&!alarm) alarm=sound_detect();
		if(armed&&alarm){
			//printf("INTRUDER ALERT! RED SPY IS IN BASE");
			//TODO:TRIGGER SOUND
			digitalWrite(RELAY,LOW);
			delay(100);
			alarm=0;
			digitalWrite(RELAY,HIGH);
			delay(100);//400);
			delay(5000);
			//END TODO
		}
		delay(1);
		//usleep(100);
	}
}
