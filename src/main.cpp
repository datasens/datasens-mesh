#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <Arduino.h>
#include "sensors.h"

float hum, lux;
double temp, press;

void TaskMoreInertSensor(void *pvParameters);
void TaskLessInertSensors(void *pvParameters);
extern "C" {
    void app_main();
}

void app_main(){
    initArduino();
    Serial.begin(115200);
    sensorsBegin();

    xTaskCreate(TaskMoreInertSensor, "Lux", 2048, NULL, 1, NULL);
    xTaskCreate(TaskLessInertSensors, "TempHumPress", 2048, NULL, 2, NULL);
}

void TaskMoreInertSensor(void *pvParameters){
    while(1){
        getBH1750(lux);
        delay(500);
    }
}

void TaskLessInertSensors(void *pvParameters){
    while(1){
        getBMP180(temp, press);
        getAM2320(hum);
        delay(1000);
    }
}