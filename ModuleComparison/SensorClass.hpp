#ifndef SENSORCLASS_HPP
#define SENSORCLASS_HPP
#include <stdint.h>
#include <Wire.h>
#include <sps30.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>  
#include "SparkFun_SCD30_Arduino_Library.h"
#include <PPD42> 
 

#define SPS30_ID 1
#define MHZ19_ID 2
#define SCD30_ID 3
#define PPD42_ID 4

typedef struct
{
    float pm2_5;
    float pm10_0;
}ParticleData;

// Sensor data struct
typedef struct
{
    uint8_t humidity = nullptr;
    int8_t temperature = nullptr;
    int pressure = nullptr;
    int co2 = nullptr;
    ParticleData particles;
}sensorData;


class SensorClass {
public:
    // Constructor
    SensorClass(){};

    // Destructor
    ~SensorClass(){};

    // Member functions

    void initialize();// Initialize sensors
    void readData();// Read data from sensors
    void processData();// Process data from sensors
    void printData(sensorData input);// Print data from sensor
    sensorData getsensorData(int sensorID);
    // sensorid 1 = SPS30, 2 = MHZ19, 3 = SCD30 4 = PPD42

    SoftwareSerial mySerial(xx, xx); // RX, TX
    
private:

    sensorData SPS30;
    int16_t ret;
    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;

    sensorData SCD30;
    SCD30 SCD30_class;


    sensorData MHZ19;
    MHZ19 myMHZ19;  

    sensorData PPD42;
    PPD42 shinyei1 = PPD42(xx, xx);



};

void SensorClass::initialize (){
    //SPS30
    sensirion_i2c_init();
    while (sps30_probe() != 0) {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
    }

    // MHZ19
    myMHZ19.begin(mySerial);
    myMHZ19.autoCalibration();

    //scd30
    if (SCD30_class.begin() == false)
    {
    Serial.println("SCD30 - Sensor not found. Please check wiring. Freezing...");
    }

    // PPD42
    shinyei1.setPeriod(30); // 30seconds
    shinyei1.begin();
}

void SensorClass::readData() {
    // Read data from sensor
    //SPS30
    ret = sps30_start_measurement(); // Start of loop start fan to flow air past laser sensor
    delay(4000); //Wait 1 second while fan is active before read data

    do {
    ret = sps30_read_data_ready(&data_ready); // Reads the last data from the sensor
    if (ret < 0) {
        Serial.print("SPS30 - error reading data-ready flag: ");
        Serial.println(ret);
    } else if (!data_ready)
        Serial.print("SPS30 - data not ready, no new measurement available\n");
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

    // MHZ19
    MHZ19.co2 = myMHZ19.getCO2(); 
    MHZ19.temperature = myMHZ19.getTemperature();


    // SCD30
    if (SCD30_class.dataAvailable())
    {
    SCD30.temperature = SCD30_class.getTemperature();
    SCD30.co2 = SCD30_class.getCO2();
    SCD30.humidity = SCD30_class.getHumidity();
    }
    else
    {   
    Serial.println("SCD30 - Data not available");
    }
    
}

void SensorClass::printData(sensorData input) {
    // Print data from sensor
    // particle sensor:
    if (input.particles.pm2_5 != nullptr)
    {
    Serial.print("PM  2.5: ");
    Serial.println(input.particles.pm2_5);
    }

    if (input.particles.pm10_0 != nullptr)
    {
    Serial.print("PM 10.0: ");
    Serial.println(input.particles.pm10_0);
    }

    // CO2 and temperature sensor:
    if (input.co2 != nullptr)
    {
    Serial.print("CO2 (ppm): ");                      
    Serial.println(input.co2);
    }

    if (input.temperature != nullptr)
    {
    Serial.print("Temperature (C): ");
    Serial.println(input.temperature);
    }

    // pressure sensor:
    if (input.pressure != nullptr)
    {
    Serial.print("Pressure (Pa): ");
    Serial.println(input.pressure);
    } 
    
    // PPD442
    shinyei1.readPM25();
    shinyei1.readPM10();
}

sensorData SensorClass::getsensorData(int sensorID)
{
    switch (sensorID)
    {
    case 1:
        return SPS30;
        break;
    case 2:
        return MHZ19;
        break;
    case 3:
        return SCD30;
        break;
    case 4:
        return PPD42;
        break;
    default:
        break;
    }
}

#endif // SENSORCLASS_HPP
