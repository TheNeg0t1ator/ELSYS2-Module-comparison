#ifndef SENSORCLASS_HPP
#define SENSORCLASS_HPP
#include <stdint.h>
#include <Wire.h>
#include <sps30.h>
#include "MHZ19.h"                                        
#include "SparkFun_SCD30_Arduino_Library.h"
//#include <Adafruit_SCD30.h>
#include <SoftwareSerial.h>
#include <Wire.h>

const byte RX_pin = 2;	
const byte TX_pin = 3;
SoftwareSerial mySerial(RX_pin, TX_pin); 
#define PPD_PIN1 4


#define SPS30_ID 1
#define MHZ19_ID 2
#define SCD30_ID 3
#define PPD42_ID 4

//#define Use_SPS30
#define Use_MHZ19
//#define Use_SCD30
//#define Use_PPD42


typedef struct
{
    float pm2_5;
    float pm10_0;
    float pcs_0_01cf; 
}ParticleData;

// Sensor data struct
typedef struct
{
    uint8_t humidity = (uint8_t)0;
    int8_t temperature = (uint8_t)0;
    int pressure = (uint8_t)0;
    int co2 = (uint8_t)0;
    ParticleData particles;
}sensorData;

class SensorClass {
public:
    // Constructor
    SensorClass() {};

    // Destructor
    ~SensorClass() {};

    // Member functions
    void initialize(); // Initialize sensors
    void readData(); // Read data from sensors
    void processData(); // Process data from sensors
    void printData(sensorData input); // Print data from sensor
    sensorData getsensorData(int sensorID);
    // sensorid 1 = SPS30, 2 = MHZ19, 3 = SCD30 4 = PPD42

private:
    sensorData SPS30_struct;
    sensorData SCD30_struct;
    sensorData MHZ19_struct;
    sensorData PPD42_struct;

    void initializeSPS30();
    void initializeMHZ19();
    void initializeSCD30();
    void initializePPD42();

    void readDataSPS30();
    void readDataMHZ19();
    void readDataSCD30();
    void readDataPPD42();

    void processDataSPS30();
    void processDataMHZ19();
    void processDataSCD30();
    void processDataPPD42();

    
    int16_t ret;
    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;

    //Adafruit_SCD30 scd30;
    SCD30 scd30;

    MHZ19 myMHZ19; 
    
    int PPD_pin = PPD_PIN1; 
    unsigned long duration;
    unsigned long starttime;
    unsigned long sampletime_ms = 2000; 
    unsigned long lowpulseoccupancy = 0;
    float ratio = 0;
    float concentration = 0;
};

void SensorClass::initialize() {
    #ifdef Use_SPS30
    initializeSPS30();
    #endif
    #ifdef Use_MHZ19
    initializeMHZ19();
    #endif
    #ifdef Use_SCD30
    initializeSCD30();
    #endif
    #ifdef Use_PPD42
    initializePPD42();
    #endif
}

void SensorClass::readData() {
    #ifdef Use_SPS30
    readDataSPS30();
    #endif
    #ifdef Use_MHZ19
    readDataMHZ19();
    #endif
    #ifdef Use_SCD30
    readDataSCD30();
    #endif
    #ifdef Use_PPD42
    readDataPPD42();
    #endif
}

void SensorClass::processData() {
    #ifdef Use_SPS30
    processDataSPS30();
    #endif
    #ifdef Use_MHZ19
    processDataMHZ19();
    #endif
    #ifdef Use_SCD30
    processDataSCD30();
    #endif
    #ifdef Use_PPD42
    processDataPPD42();
    #endif
}

sensorData SensorClass::getsensorData(int sensorID) {
    switch (sensorID) {
        case SPS30_ID:
            return SPS30_struct;
        case MHZ19_ID:
            return MHZ19_struct;
        case SCD30_ID:
            return SCD30_struct;
        case PPD42_ID:
            return PPD42_struct;
        default:
            break;
    }
}

void SensorClass::initializeSPS30() {
    // SPS30 initialization code
    sensirion_i2c_init();
  while (sps30_probe() != 0) {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
  }
  ret = sps30_start_measurement(); // Configures device ready for read every 1 second
  if (ret < 0) {
    Serial.print("error starting measurement\n");
  }
}

void SensorClass::initializeMHZ19() {
    // MHZ19 initialization code
    myMHZ19.begin(mySerial);
    myMHZ19.autoCalibration();
}

void SensorClass::initializeSCD30() {
    // SCD30 initialization code
    //  if (!scd30.begin(0x61, &Wire,SCD30_ID)) {
    // Serial.println("Failed to find SCD30 chip");
    // while (1) { delay(10); }
    // }
    Wire.begin();
    if (scd30.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

    
}

void SensorClass::initializePPD42() {
    // PPD42 initialization code
    pinMode(PPD_pin, INPUT);
    starttime = millis(); 
}

void SensorClass::readDataSPS30() {
    // SPS30 read data code
    sps30_start_measurement(); // Start of loop start fan to flow air past laser sensor
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

void SensorClass::readDataMHZ19() {
    // MHZ19 read data code
    
}

void SensorClass::readDataSCD30() {
    // SCD30 read data code
    /*
    if (scd30.dataReady()) {
        if (!scd30.read()) {
            Serial.println("Error reading sensor data");
            return;
        }
    }*/


}

void SensorClass::readDataPPD42() {
    // PPD42 read data code
    duration = pulseIn(PPD_pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
}

void SensorClass::processDataSPS30() {
    // SPS30 process data code
    SPS30_struct.particles.pm2_5 = m.mc_2p5;
    SPS30_struct.particles.pm10_0 = m.mc_10p0;
}

void SensorClass::processDataMHZ19() {
    // MHZ19 process data code
    //mySerial.print("test");
    //MHZ19_struct.co2 = myMHZ19.getCO2(); 
    //MHZ19_struct.temperature = myMHZ19.getTemperature();
}

void SensorClass::processDataSCD30() {
    // SCD30 process data code
    /*
    if (scd30.dataReady()) {
        SCD30_struct.temperature = scd30.temperature;
        SCD30_struct.co2 = scd30.CO2;
        SCD30_struct.humidity = scd30.relative_humidity;
    } else {
        Serial.println("SCD30 - Data not available");
    }*/
    if (scd30.dataAvailable()) {
    SCD30_struct.temperature = scd30.getTemperature();
    SCD30_struct.co2 = scd30.getCO2();
    SCD30_struct.humidity = scd30.getHumidity();
      } else{
    Serial.println("Waiting for new data");}
}

void SensorClass::processDataPPD42() {
    // PPD42 process data code
    if ((millis() - starttime) >= sampletime_ms) { // if the sample time == 2s
        ratio = lowpulseoccupancy / (sampletime_ms * 10.0);  
        concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; 
        PPD42_struct.particles.pcs_0_01cf = concentration; // Store concentration in pcs/0.01cf
        lowpulseoccupancy = 0;
        starttime = millis();
    }
}

void SensorClass::printData(sensorData input) {
    // Print data from sensor
    // particle sensor:
    if (input.particles.pm2_5 != 0) {
        Serial.print("PM  2.5: ");
        Serial.println(input.particles.pm2_5);
    }

    if (input.particles.pm10_0 != 0) {
        Serial.print("PM 10.0: ");
        Serial.println(input.particles.pm10_0);
    }

    if (input.particles.pcs_0_01cf != 0) {
        Serial.print("Concentration: ");
        Serial.print(input.particles.pcs_0_01cf);
        Serial.println(" pcs/0.01cf"); // Print concentration with unit
    }

    // CO2 and temperature sensor:
    if (input.co2 != 0) {
        Serial.print("CO2 (ppm): ");                      
        Serial.println(input.co2);
    }

    if (input.temperature != 0) {
        Serial.print("Temperature (C): ");
        Serial.println(input.temperature);
    }

    // pressure sensor:
    if (input.pressure != 0) {
        Serial.print("Pressure (Pa): ");
        Serial.println(input.pressure);
    } 
}


#endif // SENSORCLASS_HPP
