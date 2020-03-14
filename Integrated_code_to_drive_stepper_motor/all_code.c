/*
 * adc.c
 *
 *  Created on: May 13, 2019
 *      Author: Jay
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <math.h>

#define INPUT_ACQ_TIME      5000000
#define SPI_FREQ            1000000

#define str 5
#define dir 4
#define MS1 6
#define MS2 0
#define EN 3

//Magnetometer #defines
#define MAGNETOMETER_ADD 0x1E
#define CRA_REG_M 0x00
#define CRB_REG_M 0x01
#define MR_REG_M 0x02
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Z_H_M 0x05
#define OUT_Z_L_M 0x06
#define OUT_Y_H_M 0x07
#define OUT_Y_L_M 0x08
#define PI (22/7)

typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} magnetometer_t;

static int myFd ;
magnetometer_t mag_raw = { 0 };
float heading = 0;
int fm;


int lsm_setup(void) {
    fm = wiringPiI2CSetup(MAGNETOMETER_ADD);
    wiringPiI2CWriteReg8(fm, CRA_REG_M, 0b00010100);
    wiringPiI2CWriteReg8(fm, CRB_REG_M, 0b00100000);
    wiringPiI2CWriteReg8(fm, MR_REG_M, 0b00000000);
    return fm;
}

int lsm_read(int fm) {
    mag_raw.x_axis = wiringPiI2CReadReg8(fm, OUT_X_L_M) & 0x00ff;
    mag_raw.x_axis |= (wiringPiI2CReadReg8(fm, OUT_X_H_M) << 8);
    mag_raw.y_axis = wiringPiI2CReadReg8(fm, OUT_Y_L_M) & 0x00ff;
    mag_raw.y_axis |= (wiringPiI2CReadReg8(fm, OUT_Y_H_M) << 8);
    mag_raw.z_axis = wiringPiI2CReadReg8(fm, OUT_Z_L_M) & 0x00ff;
    mag_raw.z_axis |= (wiringPiI2CReadReg8(fm, OUT_Z_H_M) << 8);

    heading =atan2(mag_raw.x_axis, mag_raw.y_axis) * 180 / PI;
 
    return heading;
}

void step_forward_default(int angle, int frequency, int direction) {
    digitalWrite(dir, direction); //Pull direction pin low to move "forward"
    int limit = (angle * 200)/360;
    int delay__ = (int) ((1/frequency) * 1000000);    
    for(int  x = 0; x < limit; x++)  //Loop the forward stepping enough times for motion to be visible
    {
        digitalWrite(str,HIGH); //Trigger one step forward
      	usleep(delay__); 
        digitalWrite(str,LOW); //Pull step pin low so it can be triggered again
      	usleep(delay__); 
    }
}


void pin_setup(void) {
    pinMode(str, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(EN, OUTPUT);
    pinMode(1, OUTPUT);
}

void pin_reset(void) {
    digitalWrite(str, LOW);
    digitalWrite(dir, LOW);
    digitalWrite(MS1, LOW);
    digitalWrite(MS2, LOW);
    digitalWrite(EN, HIGH);
}

void spiSetup (int spiChannel) {
    if ((myFd = wiringPiSPISetup (spiChannel, SPI_FREQ)) < 0) {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}

int myAnalogRead(int spiChannel,int channelConfig,int analogChannel) {
    if(analogChannel<0 || analogChannel>7){
        return -1;
    }
    unsigned char buffer[3] = {1};
    buffer[1] = (channelConfig+analogChannel) << 4;
    wiringPiSPIDataRW(spiChannel, buffer, 3);
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
}

void _delay(int number_of_seconds) {
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds)
        ;
}

float compensate_value(void) {
    float uncompensated_digital_value = 0,
          compensated_digital_value = 0,
          g_x = 0,
          new_raw_value = 0;

    int raw_value = 0;
    raw_value = myAnalogRead(0,8,0);
    uncompensated_digital_value = ((3.3/1023)*raw_value);
    g_x = (-3.495*uncompensated_digital_value) + 1.032;
    new_raw_value = raw_value + g_x;
    compensated_digital_value = (3.3/1023)*new_raw_value;
    return compensated_digital_value;
}

int main(int agrc, char *argv[]) {
    wiringPiSetup();
    spiSetup(0);
     fm = lsm_setup();
    pin_setup();
    pin_reset();
    digitalWrite(EN, LOW);
    char *char_sampling_rate;
    char *_pwm_freq;
    int int_sampling_rate;
    int pwm_freq = 0;
    float adc_output = 0;
    int volt_to_angle = 0;
    int lsm_init_angle = lsm_read(fm);
    int previous_lsm = 0;
    int previous_angle = 0;
    // command line argument will accept sampling rate
    if (agrc < 2) {
        int_sampling_rate = 1000;
        pwm_freq = 500;
    }
    else {
        char_sampling_rate = argv[1];
        _pwm_freq = argv[2];
        int_sampling_rate = atoi(char_sampling_rate);
        pwm_freq = atoi(_pwm_freq);
    }
    printf("Sampling Rate given: %dHz, PWM_frequency given: %dHz\n", int_sampling_rate, pwm_freq);

    while(1 {
        lsm_angle = lsm_read(fm) - lsm_init_angle;
        adc_output = compensate_value();
        volt_to_angle = (int) ((adc_output / 3.3) * 360);
        int difference_angle = abs(volt_to_angle - previous_angle);
        if(difference_angle == 0);
        else if( volt_to_angle < previous_angle) {
            step_forward_default(difference_angle, pwm_freq, 1);
            printf("Mag_x: %d, Mag_y: %d, Mag_z: %d, heading: %f\n", mag_raw.x_axis, mag_raw.y_axis, mag_raw.z_axis, heading);
	}
        else if(volt_to_angle > previous_angle) {
        printf("Mag_x: %d, Mag_y: %d, Mag_z: %d, heading: %f\n", mag_raw.x_axis, mag_raw.y_axis, mag_raw.z_axis, heading);
		step_forward_default(difference_angle, pwm_freq, 0);
        }
        previous_angle = volt_to_angle;
    }
    return 0;
}

