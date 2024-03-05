
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_APDS9960.h>
#include <Arduino_LSM9DS1.h>

#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

// Humidity and temperature sensor
float old_temp = 0;
float old_hum = 0;

// Gyroscope
float x, y, z;
int plusThreshold = 30, minusThreshold = -30;

void setup() {
  Serial.begin(115200);
  while (!Serial);

//Light sensor
if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor.");
  }

// Humidity and temperature sensor
  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
  // Atmospheric pressure and altitude sensor
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
  
  // Gyroscope 
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");

  // SCD30
  Wire.begin();
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }
}

void loop() {
  int r, g, b;
  //Light sensor
  if(APDS.colorAvailable()){
    APDS.readColor(r, g, b);
    Serial.print("RED = ");
    Serial.println(r);
    Serial.print("GREEN = ");
    Serial.println(g);
    Serial.print("BLUE = ");
    Serial.println(b);
  }


  /*
  HUMIDITY AND TEMPERATURE SENSOR
  */
  // read all the sensor values
  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();

  // check if the range values in temperature are bigger than 0,5 ºC
  // and if the range values in humidity are bigger than 1%
  if (abs(old_temp - temperature) >= 0.5 || abs(old_hum - humidity) >= 1 )
  {
    old_temp = temperature;
    old_hum = humidity;
    // print each of the sensor values
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity    = ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.println();
  }

  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");

  // print an empty line
  Serial.println();

  // wait 1 second to print again
  delay(1000);

  /*
ATMOSPHERIC PRESSURE AND ALTITUDE SENSOR
  */
 
   // read the sensor value
  float pressure = BARO.readPressure();
  float altitude = 44330 * ( 1 - pow(pressure/101.325, 1/5.255) );
  // print the sensor value
  Serial.print("Altitude according to kPa is = ");

  Serial.print(altitude);
  Serial.println(" m");


  // print an empty line

  Serial.println();

  // wait 1 second to print again
  delay(1000);

  /*
  Gyroscope
  */
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }

  if(y > plusThreshold)
  {
    Serial.println("Collision front");
    delay(500);
  }

  if(y < minusThreshold)
  {
    Serial.println("Collision back");
    delay(500);
  }

  if(x < minusThreshold)
  {
    Serial.println("Collision right");
    delay(500);
  }
    if(x > plusThreshold)
  {
    Serial.println("Collision left");
    delay(500);
  }
// SCD30
if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
  }
  else
    Serial.println("Waiting for new data");

  delay(500);
}
