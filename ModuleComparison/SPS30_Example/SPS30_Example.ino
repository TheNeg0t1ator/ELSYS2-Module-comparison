#include "../SensorClass.hpp"

SensorClass sensors;
Serial.begin(9600);

void setup() {
  sensors.initialize();

}

void loop() {
  sensors.readData();
  sensors.processData();
  sensors.printData(sensors.getsensorData(SPS30_ID));
  sensors.printData(sensors.getsensorData(MHZ19_ID));
  sensors.printData(sensors.getsensorData(SCD30_ID));
  sensors.printData(sensors.getsensorData(PPD42_ID));



  delay(1000);  // Delay for 1 second
}
