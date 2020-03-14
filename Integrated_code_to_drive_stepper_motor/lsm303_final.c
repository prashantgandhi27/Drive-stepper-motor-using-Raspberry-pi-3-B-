/*
 * lsm303.c
 *
 *  Created on: May 13, 2019
 *      Author: Jay
 */
/*

 * lsm.c
 *
 *  Created on: May 9, 2019
 *      Author: Jay
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

////Accelerometer #defines
//#define ACCELEROMETER_ADD 0x19
#define MAGNETOMETER_ADD 0x1E
//#define CTRL_REG1_A 0x20
//#define CTRL_REG4_A 0x23
//#define OUT_X_L_A 0x028
//#define OUT_X_H_A 0x029
//#define OUT_Y_L_A 0x02A
//#define OUT_Y_H_A 0x02B
//#define OUT_Z_L_A 0x02C
//#define OUT_Z_H_A 0x02D

//Magnetometer #defines
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

//typedef struct {
//    int16_t x_axis;
//    int16_t y_axis;
//    int16_t z_axis;
//} accelerometer_t;

typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} magnetometer_t;

int lsm_setup(void){
    int fm;
    fm = wiringPiI2CSetup(MAGNETOMETER_ADD);
    wiringPiI2CWriteReg8(fm, CRA_REG_M, 0b00010100);
    wiringPiI2CWriteReg8(fm, CRB_REG_M, 0b00100000);
    wiringPiI2CWriteReg8(fm, MR_REG_M, 0b00000000);
}
int lsm_read(int fm)
{ //, fm;
//    accelerometer_t acc_raw = {0};
    magnetometer_t mag_raw = { 0 };
    float heading = 0;

//    wiringPiSetup();
//    fa = wiringPiI2CSetup(ACCELEROMETER_ADD);
//
//        acc_raw.x_axis = wiringPiI2CReadReg8(fa, OUT_X_L_A) & 0xff00;
//        acc_raw.x_axis |= (wiringPiI2CReadReg8(fa, OUT_X_H_A) << 8);
//        acc_raw.y_axis = wiringPiI2CReadReg8(fa, OUT_Y_L_A) & 0xff00;
//        acc_raw.y_axis |= (wiringPiI2CReadReg8(fa, OUT_Y_H_A) << 8);
//        acc_raw.z_axis = wiringPiI2CReadReg8(fa, OUT_Z_L_A) & 0xff00;

    mag_raw.x_axis = wiringPiI2CReadReg8(fm, OUT_X_L_M) & 0x00ff;
    mag_raw.x_axis |= (wiringPiI2CReadReg8(fm, OUT_X_H_M) << 8);
    mag_raw.y_axis = wiringPiI2CReadReg8(fm, OUT_Y_L_M) & 0x00ff;
    mag_raw.y_axis |= (wiringPiI2CReadReg8(fm, OUT_Y_H_M) << 8);
    mag_raw.z_axis = wiringPiI2CReadReg8(fm, OUT_Z_L_M) & 0x00ff;
    mag_raw.z_axis |= (wiringPiI2CReadReg8(fm, OUT_Z_H_M) << 8);

//        printf("Acc_x: %d, Acc_y: %d, Acc_z: %d\n", acc_raw.x_axis, acc_raw.y_axis, acc_raw.z_axis);
    printf("Mag_x: %d, Mag_y: %d, Mag_z: %d\n", mag_raw.x_axis, mag_raw.y_axis, mag_raw.z_axis);

    heading = atan2(mag_raw.x_axis, mag_raw.y_axis) * 180 / PI;
    printf("heading: %f\n", heading);
    return (int)heading;
}



