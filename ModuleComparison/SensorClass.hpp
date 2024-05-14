#ifndef SENSORCLASS_HPP
#define SENSORCLASS_HPP
#include <stdint.h>
#include <Wire.h>
#include <sps30.h>
#include "MHZ19.h"                                        
#include "SparkFun_SCD30_Arduino_Library.h"
#include <Adafruit_SCD30.h>

#define SPS30_ID 1
#define MHZ19_ID 2
#define SCD30_ID 3
#define PPD42_ID 4

typedef struct
{
    float pm2_5;
    float pm10_0;
    float pcs_0_01cf; 
}ParticleData;

// Sensor data struct
typedef struct
{
    uint8_t humidity = NULL;
    int8_t temperature = NULL;
    int pressure = NULL;
    int co2 = NULL;
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
    
private:
    sensorData SPS30_struct;
    int16_t ret;
    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;

    sensorData SCD30_struct;
    Adafruit_SCD30 scd30;

    sensorData MHZ19_struct;
    MHZ19 myMHZ19; 
    
    

    sensorData PPD42_struct;
    int pin = 8; 
    unsigned long duration;
    unsigned long starttime;
    unsigned long sampletime_ms = 2000; 
    unsigned long lowpulseoccupancy = 0;
    float ratio = 0;
    float concentration = 0;
};

void SensorClass::initialize() {
    // SPS30
  sensirion_i2c_init();
  while (sps30_probe() != 0) {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
  }
  ret = sps30_start_measurement(); // Configures device ready for read every 1 second
  if (ret < 0) {
    Serial.print("error starting measurement\n");
  }
  delay(1000);

    // MHZ19
    myMHZ19.begin(Serial);
    myMHZ19.autoCalibration();

    // SCD30
    if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { delay(10); }
    }
    Serial.println("SCD30 Found!");

    // PPD42
    pinMode(pin, INPUT);
    starttime = millis(); 
}

void SensorClass::readData() {
    // Read data from sensor
    // SPS30
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

    // SCD30
    if (scd30.dataReady()) {
        if (!scd30.read()) {
            Serial.println("Error reading sensor data");
            return;
        }
    }

    // PPD42
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
}   

void SensorClass::processData() {
    // Process data from sensor
    // SPS30
    SPS30_struct.particles.pm2_5 = m.mc_2p5;
    SPS30_struct.particles.pm10_0 = m.mc_10p0;

    // MHZ19
    MHZ19_struct.co2 = myMHZ19.getCO2(); 
    MHZ19_struct.temperature = myMHZ19.getTemperature();

    // SCD30
    if (scd30.dataReady()) {
        SCD30_struct.temperature = scd30.temperature;
        SCD30_struct.co2 = scd30.CO2;
        SCD30_struct.humidity = scd30.relative_humidity;
    } else {
        Serial.println("SCD30 - Data not available");
    }

    // PPD42
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
    if (input.particles.pm2_5 != NULL) {
        Serial.print("PM  2.5: ");
        Serial.println(input.particles.pm2_5);
    }

    if (input.particles.pm10_0 != NULL) {
        Serial.print("PM 10.0: ");
        Serial.println(input.particles.pm10_0);
    }

    if (input.particles.pcs_0_01cf != NULL) {
        Serial.print("Concentration: ");
        Serial.print(input.particles.pcs_0_01cf);
        Serial.println(" pcs/0.01cf"); // Print concentration with unit
    }

    // CO2 and temperature sensor:
    if (input.co2 != NULL) {
        Serial.print("CO2 (ppm): ");                      
        Serial.println(input.co2);
    }

    if (input.temperature != NULL) {
        Serial.print("Temperature (C): ");
        Serial.println(input.temperature);
    }

    // pressure sensor:
    if (input.pressure != NULL) {
        Serial.print("Pressure (Pa): ");
        Serial.println(input.pressure);
    } 
}

sensorData SensorClass::getsensorData(int sensorID) {
    switch (sensorID) {
        case 1:
            return SPS30_struct;
        case 2:
            return MHZ19_struct;
        case 3:
            return SCD30_struct;
        case 4:
            return PPD42_struct;
        default:
            break;
    }
}

#endif // SENSORCLASS_HPP