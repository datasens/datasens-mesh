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

void TaskMQTTreconnect(void *pvParameters);
void TaskPubMoreInertData(void *pvParameters);
void TaskPubLessInertData(void *pvParameters);
void TaskSyncTime(void *pvParameters);
void setup_wifi();
void MQTTreconnect();
void syncTime();
String addTime(double measure);

extern "C" {
    void app_main();
}

void app_main(){
    initArduino();

    Serial.begin(115200);
    sensorsBegin();
    setup_wifi();
    client.setServer(mqtt_server, 1883); // set MQTT server info

    xTaskCreate(TaskMQTTreconnect, "MQTTreconnect", 2048, NULL, 2, NULL);
    xTaskCreate(TaskPubLessInertData, "PubLessInertData", 4096, NULL, 3, NULL);
    xTaskCreate(TaskPubMoreInertData, "PubMoreInertData", 4096, NULL, 3, NULL);
    xTaskCreate(TaskSyncTime,"SyncTime", 2048, NULL, 1, NULL);
}

void TaskMQTTreconnect(void *pvParameters){
    while(1){
        MQTTreconnect();
    }
}

void TaskPubLessInertData(void *pvParameters){
    while(1){
        float lux;
        getBH1750(lux);
        client.publish(lux_topic, addTime(lux).c_str(), true);
        delay(1000);
    }
}

void TaskPubMoreInertData(void *pvParameters){
    while(1){
        float hum;
        double temp, press;
        getBMP180(temp, press);
        getAM2320(hum);
        client.publish(temperature_topic, addTime(temp).c_str(), true);
        client.publish(humidity_topic, addTime(hum).c_str(), true);
        client.publish(pressure_topic, addTime(press).c_str(), true);
        delay(2000);
    }
}

void TaskSyncTime(void *pvParameters){
    while(1){
        syncTime();
        delay(1000*60*30); // sync every 30 minutes
    }
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

void syncTime(){
    const char* ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 10800;
    const int daylightOffset_sec = 0;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

String addTime(double measure){
    struct tm timeinfo;
    char buffer[30];
    getLocalTime(&timeinfo);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S | ", &timeinfo);
    String date = String(buffer);
    String data = String(measure);
    return date + data;
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