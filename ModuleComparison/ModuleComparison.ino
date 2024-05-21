#include "SensorClass.hpp"


SensorClass sensors;

void setup() {
  Serial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("starting init\n\r");
  sensors.initialize();
  Serial.print("init complete\n\r");
  

}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  sensors.readData();
  sensors.processData();
  #ifdef Use_SPS30
    sensors.printData(sensors.getsensorData(SPS30_ID));
  #endif

  #ifdef Use_MHZ19
    sensors.printData(sensors.getsensorData(MHZ19_ID));
  #endif

  #ifdef Use_SCD30
    sensors.printData(sensors.getsensorData(SCD30_ID));
  #endif

  #ifdef Use_PPD42
    sensors.printData(sensors.getsensorData(PPD42_ID));
  #endif



  delay(1000);  // Delay for 1 second
}
