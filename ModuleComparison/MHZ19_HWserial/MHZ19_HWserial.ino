#include <Arduino.h>
#include "MHZ19.h"                                        

#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             

unsigned long getDataTimer = 0;

void setup() {
    Serial.begin(BAUDRATE);                                // Begin Hardware Serial for both MHZ19 and debug
    
    myMHZ19.begin(Serial);                                 // Pass the Hardware Serial to MHZ19 object
    myMHZ19.autoCalibration();                             // Turn auto calibration ON (OFF with autoCalibration(false))
}

void loop() {
    if (millis() - getDataTimer >= 2000) {
        int CO2 = myMHZ19.getCO2();                        // Request CO2 (as ppm)
        Serial.print("CO2 (ppm): ");                      
        Serial.println(CO2);      

        int8_t Temp = myMHZ19.getTemperature();            // Request Temperature (as Celsius)
        Serial.print("Temperature (C): ");
        Serial.println(Temp);

        getDataTimer = millis();
    }
}
