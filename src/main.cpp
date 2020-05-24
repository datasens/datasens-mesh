#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "sensors.h"

#define wifi_ssid "wifi_name"
#define wifi_password "wifi_password"

#define mqtt_server "IPaddress"
#define mqtt_user "your_username"
#define mqtt_password "your_password"

#define humidity_topic "esp32/test/humidity"
#define temperature_topic "esp32/test/temperature"
#define pressure_topic "esp32/test/pressure"
#define lux_topic "esp32/test/lux"

WiFiClient espClient;
PubSubClient client(espClient);

float hum, lux;
double temp, press;

void TaskMoreInertSensor(void *pvParameters);
void TaskLessInertSensors(void *pvParameters);
void TaskMQTTreconnect(void *pvParameters);
void TaskPubMoreInertData(void *pvParameters);
void TaskPubLessInertData(void *pvParameters);
void setup_wifi();
void MQTTreconnect();

extern "C" {
    void app_main();
}

void app_main(){
    initArduino();

    Serial.begin(115200);
    sensorsBegin();
    setup_wifi();
    client.setServer(mqtt_server, 1883); // set MQTT server info

    xTaskCreate(TaskMQTTreconnect, "MQTTreconnect", 2048, NULL, 1, NULL);
    xTaskCreate(TaskMoreInertSensor, "Lux", 2048, NULL, 3, NULL);
    xTaskCreate(TaskLessInertSensors, "TempHumPress", 2048, NULL, 3, NULL);
    xTaskCreate(TaskPubMoreInertData, "PubMoreInertData", 2048, NULL, 2, NULL);
    xTaskCreate(TaskPubLessInertData, "PubLessInertData", 2048, NULL, 2, NULL);
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
        delay(2000);
    }
}

void TaskMQTTreconnect(void *pvParameters){
    while(1){
        MQTTreconnect();
    }
}

void TaskPubMoreInertData(void *pvParameters){
    while(1){
        if(lux != 0){
            client.publish(lux_topic, String(lux).c_str(), true);
        }
        delay(500);
    }
}

void TaskPubLessInertData(void *pvParameters){
    while(1){
        client.publish(temperature_topic, String(temp).c_str(), true);
        client.publish(humidity_topic, String(hum).c_str(), true);
        client.publish(pressure_topic, String(press).c_str(), true);
        delay(2000);
    }
}

void setup_wifi(){
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);
    WiFi.begin(wifi_ssid, wifi_password);

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void MQTTreconnect(){
    while(!client.connected()){
        Serial.print("Attempting MQTT connection...");
        if(client.connect("ESP32")){
            Serial.println("connected");
        }else{
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}