#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <Arduino.h>
#include "sensors.h"

float hum, lux;
double temp, press;

void setup() {
    Serial.begin(115200);
    sensorsBegin();
}
void loop() {
    getAM2320(hum);
    getBH1750(lux);
    getBMP180(temp, press);
    delay(1000);
}