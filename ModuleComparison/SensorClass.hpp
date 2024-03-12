#ifndef SENSORCLASS_HPP
#define SENSORCLASS_HPP
#include <stdint.h>
#include <Wire.h>

// Sensor data struct
typedef struct
{
    uint8_t humidity;
    int8_t temperature;
    int pressure;
    int co2;
}sensorData;




class SensorClass {
public:
    // Constructor
    SensorClass(){SensorClass::initialize();}

    // Destructor
    ~SensorClass();

    // Member functions
    
private:

    sensorData SPS30;
    sensorData DHT11;
    sensorData SCD30;

    void initialize();
    void readData();
    void processData();
    void printData();

};




void SensorClass::initialize() {
    // Initialize sensor
    


}

#endif // SENSORCLASS_HPP
