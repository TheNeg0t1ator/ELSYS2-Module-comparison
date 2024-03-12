#ifndef SENSORCLASS_HPP
#define SENSORCLASS_HPP
#include <stdint.h>
#include <Wire.h>
#include <sps30.h>


typedef struct
{
    float pm2_5;
    float pm10_0;
}ParticleData;

// Sensor data struct
typedef struct
{
    uint8_t humidity;
    int8_t temperature;
    int pressure;
    int co2;
    ParticleData particles;
}sensorData;




class SensorClass {
public:
    // Constructor
    SensorClass(){};

    // Destructor
    ~SensorClass(){};

    // Member functions
    
private:

    sensorData SPS30;
    int16_t ret;
    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;




    sensorData DHT11;
    sensorData SCD30;

    void initialize();
    void readData();
    void processData();
    void printData();

};




void SensorClass::initialize() {
    // Initialize sensor
    //SPS30
    sensirion_i2c_init();
    while (sps30_probe() != 0) {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
    }
    
}


void SensorClass::readData() {
    // Read data from sensor
    //SPS30
    ret = sps30_start_measurement(); // Start of loop start fan to flow air past laser sensor
    delay(4000); //Wait 1 second while fan is active before read data

    do {
    ret = sps30_read_data_ready(&data_ready); // Reads the last data from the sensor
    if (ret < 0) {
        Serial.print("error reading data-ready flag: ");
        Serial.println(ret);
    } else if (!data_ready)
        Serial.print("data not ready, no new measurement available\n");
    else
        break;
    delay(100); /* retry in 100ms */
    } while (1);
    ret = sps30_read_measurement(&m); // Ask SPS30 for measurments over I2C, returns 10 sets of data
}   




void SensorClass::processData() {
    // Process data from sensor
    //SPS30
    SPS30.particles.pm2_5 = m.mc_2p5;
    SPS30.particles.pm10_0 = m.mc_10p0;
}

void SensorClass::printData() {
    // Print data from sensor
    //SPS30
    Serial.print("PM  2.5: ");
    Serial.println(SPS30.particles.pm2_5);
    Serial.print("PM 10.0: ");
    Serial.println(SPS30.particles.pm10_0);
    Serial.println();
}


#endif // SENSORCLASS_HPP
