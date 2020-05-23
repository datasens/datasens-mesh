#ifndef _SENSORS_H
#define _SENSORS_H

#include <Arduino.h>
#include <AM2320.h>
#include <SFE_BMP180.h>
#include <BH1750.h>

void sensorsBegin();
void getAM2320 (float& hum); // func of read data from AM2320 sensor
void getBMP180(double& temp, double& press); // func of read data from BMP180 sensor
void getBH1750(float& lux); // func of read data from BH1750 sensor

#endif
