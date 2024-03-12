#include "SensorClass.hpp"

SensorClass sensors();

void setup() {
  sensors.initialize();

}

void loop() {
  sensors.readData();
  sensors.processData();
  sensors.printData();
  delay(1000);  // Delay for 1 second
}
