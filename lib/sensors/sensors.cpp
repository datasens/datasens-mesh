#include "sensors.h"

AM2320 am2320;
SFE_BMP180 bmp180;
BH1750 bh1750;

void sensorsBegin(){
    am2320.begin();
    bmp180.begin();
    bh1750.begin();
}

void getAM2320 (float& hum){
    Serial.println("AM2320:");
    if(am2320.measure()){
        //Serial.print("Temperature: "); 
        //Serial.println(am2320.getTemperature());
        Serial.print("Humidity: ");
        hum = am2320.getHumidity();
        Serial.println(hum);
    }else{
        short int errorCode = am2320.getErrorCode();
        switch (errorCode){
            case 1:
                Serial.println("ERR: Sensor is offline");
                break;
            case 2:
                Serial.println("ERR: CRC validation failed");
                break;
        }
    }
}

void getBMP180(double& temp, double& press){
    char status;
    status = bmp180.startTemperature();
    Serial.println("BMP180:");
    if(status != 0){
        delay(status);
        delay(20);
        status = bmp180.getTemperature(temp);
        if(status !=0){
            Serial.print("Temperature: ");
            Serial.print(temp,2);
            Serial.println(" *C");

            status = bmp180.startPressure(3);
            if(status != 0){
                delay(status);
                delay(20);
                status = bmp180.getPressure(press,temp);
                if(status != 0){
                    Serial.print("Pressure: ");
                    Serial.print(press,2);
                    Serial.println(" mb");
                }else{
                    Serial.println("ERROR retrieving pressure measurement");
                }
            }else{
                Serial.println("ERROR starting pressure measurement");
            }
        }else{
            Serial.println("ERROR retrieving temperature measurement");
        }
    }else{
        Serial.println("ERROR starting temperature measurement");
    }
}

void getBH1750(float& lux){
    Serial.println("BH1750:");
    lux = bh1750.readLightLevel();
    if(lux < 0){
        Serial.println("ERROR connect BH1750!");
    }else{
        Serial.print("Light: ");
        Serial.print(lux);
        Serial.println(" lx");
    }
}