#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define str 5
#define dir 4
#define MS1 6
#define MS2 0
#define EN 3


void step_forward_default(int angle, int frequency, int direction)
{
	printf("Moving forward at default step mode.\n");
	digitalWrite(dir, direction); //Pull direction pin low to move "forward"
	int limit = (int) (angle * 0.556);
	int delay__ = (int) ((1/frequency) * 0.5 * 1000000);
	for(int  x = 0; x < limit; x++)  //Loop the forward stepping enough times for motion to be visible
	{
		digitalWrite(str,HIGH); //Trigger one step forward
		delayMicroseconds(delay__);
		digitalWrite(str,LOW); //Pull step pin low so it can be triggered again
		delayMicroseconds(delay__);
	}
}

void pin_setup(void){
	pinMode(str, OUTPUT);
	pinMode(dir, OUTPUT);
	pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);
	pinMode(EN, OUTPUT);
	pinMode(1, OUTPUT);
}

void pin_reset(void)
{
	digitalWrite(str, LOW);
	digitalWrite(dir, LOW);
	digitalWrite(MS1, LOW);
	digitalWrite(MS2, LOW);
	digitalWrite(EN, HIGH);
}

//int main(int argc, char* argv[]) {
//	wiringPiSetup();
//	pin_setup();
//	pin_reset();
//	digitalWrite(EN, LOW);
//	if (wiringPiSetupGpio() == -1)
//		exit (1);
//	int freq = atoi(argv[1]);
//	step_forward_default(freq, angle)
//	pwmSetMode(PWM_MODE_MS);
//	pinMode(18,PWM_OUTPUT);
//	pwmSetClock(200000);
//	pwmSetRange (1024);
//	int val = 0;
//	pwmWrite (1, val);
//	delay(10000);

