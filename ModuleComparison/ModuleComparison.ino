#include "SensorClass.hpp"
#include "Oled.hpp"
#include "LCD.hpp"

LCD_Display lcd;
SensorClass sensors;
sensorData averagedData;

void setup() {
  Serial.begin(9600);
  
  lcd.init_display();
  lcd.print(0,0,"Starting init");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.print("starting init\n\r");
  
  sensors.initialize();
  
  Serial.print("init complete\n\r");
  
  lcd.print(0,1,"Init complete");
  delay(2000);
  lcd.clear();
  
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  sensors.readData();
  sensors.processData();
  averagedData = getAveragedData(&sensors);
  //

  #ifdef Use_MHZ19
    sensors.printData(sensors.getsensorData(MHZ19_ID));
    #if !defined(Use_SCD30) || !defined(Use_PPD42)
      lcd.writeString(0,0,"CO2: %d", sensors.getsensorData(MHZ19_ID).co2);
      lcd.writeString(0,1,"Temp: %d         ", sensors.getsensorData(MHZ19_ID).temperature);
    #endif
  #endif

  #ifdef Use_SCD30
    sensors.printData(sensors.getsensorData(SCD30_ID));
    #if !defined(Use_MHZ19) || !defined(Use_PPD42)
      lcd.writeString(0,0,"CO2: %d", sensors.getsensorData(SCD30_ID).co2);
      lcd.writeString(0,1,"Temp: %d", sensors.getsensorData(SCD30_ID).temperature);
      //lcd.writeString(0,0,"Humidity: %d", sensors.getsensorData(SCD30_ID).humidity);
    #endif
  #endif

  #ifdef Use_PPD42
    sensors.printData(sensors.getsensorData(PPD42_ID));
    #if  !defined(Use_MHZ19) || !defined(Use_SCD30)
      lcd.writeString(0,0,"2.5: %d", sensors.getsensorData(PPD42_ID).particles.pm2_5);
      lcd.writeString(0,1,"10: %d", sensors.getsensorData(PPD42_ID).particles.pm10_0);
    #endif
  #endif

  #if defined(Use_MHZ19) && defined(Use_SCD30) && defined(Use_PPD42)
    lcd.clear();
    lcd.writeString(0,0,"CO2:%4d", averagedData.co2);
    lcd.writeString(9,0, "H:%3d", sensors.getsensorData(SCD30_ID).humidity);
    lcd.writeString(0,1,"T: %2d", averagedData.temperature);
    //lcd.writeString(6,0,"RH: %3d", averagedData.humidity);
    // delay(2000);  // Delay 
    // lcd.clear();
    lcd.writeString(9,1,"P: ");
    lcd.get_lcd().print(averagedData.particles.pcs_0_01cf);
    delay(2000);  // Delay 
    
  #endif
  
}
