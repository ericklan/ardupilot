
#include "AP_ADC_HIL.h"
#include <AP_HAL.h>
extern const AP_HAL::HAL& hal;

/*
 *       AP_ADC_HIL.cpp
 *       Author: James Goppert
 *
 *       License:
 *       This library is free software; you can redistribute it and/or
 *       modify it under the terms of the GNU Lesser General Public
 *       License as published by the Free Software Foundation; either
 *       version 2.1 of the License, or (at your option) any later version.
 */

const uint8_t AP_ADC_HIL::sensors[6] = {1,2,0,4,5,6};
const int8_t AP_ADC_HIL::sensorSign[6]  = { 1, -1, -1,-1,  1,  1};
const float AP_ADC_HIL::gyroBias[3]     = {1665,1665,1665};
const float AP_ADC_HIL::accelBias[3]    = {2025,2025,2025};
// gyroScale = 1/[GyroGain*pi/180]   GyroGains (0.4,0.41,0.41)
const float AP_ADC_HIL::gyroScale[3] = {143.239, 139.746, 139.746};
const float AP_ADC_HIL::accelScale[3] = {418,418,418}; // adcPerG
    
uint16_t  AP_ADC_HIL::_count; // number of samples captured

AP_ADC_HIL::AP_ADC_HIL()
{
    // gyros set to zero for calibration
    setGyro(0,0);
    setGyro(1,0);
    setGyro(2,0);

    // accels set to zero for calibration
    setAccel(0,0);
    setAccel(1,0);
    setAccel(2,0);

    // set diff press and temp to zero
    setGyroTemp(0);
    setPressure(0);
}

void AP_ADC_HIL::Init()
{
    hal.scheduler->register_timer_process( AP_ADC_HIL::read );
}

// Read one channel value
float AP_ADC_HIL::Ch(unsigned char ch_num)
{
    return adcValue[ch_num];
}

// Read 6 channel values
uint32_t AP_ADC_HIL::Ch6(const uint8_t *channel_numbers, float *result)
{
    _count = 0;
    
    for (uint8_t i=0; i<6; i++) {
        result[i] = Ch(channel_numbers[i]);
    }
    uint32_t now = hal.scheduler->micros();
    uint32_t ret = now - _last_ch6_time;
    _last_ch6_time = now;
    return ret;
}

// Set one channel value
void AP_ADC_HIL::setHIL(int16_t p, int16_t q, int16_t r, int16_t gyroTemp,
                        int16_t aX, int16_t aY, int16_t aZ, int16_t diffPress)
{
    // gyros
    setGyro(0,p);
    setGyro(1,q);
    setGyro(2,r);

    // temp
    setGyroTemp(gyroTemp);

    // accel
    setAccel(0,aX);
    setAccel(1,aY);
    setAccel(2,aZ);

    // differential pressure
    setPressure(diffPress);
}

// see if new data is available
bool AP_ADC_HIL::new_data_available(const uint8_t *channel_numbers)
{
    return true;
}

// Get minimum number of samples read from the sensors
uint16_t AP_ADC_HIL::num_samples_available(const uint8_t *channel_numbers)
{
    return _count;
}
