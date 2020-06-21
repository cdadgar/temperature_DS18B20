/*
 *  DS18B20 1-wire temperature sensor 
 *  
 *  packaging:
 *  like a transistor:
 *  1 - gnd
 *  2 - DQ
 *  3 - VDD
 *  
 *  sealed in stainless steel rod:
 *  yellow - DQ (data line)
 *  red - VDD (3.3 or 5V)
 *  black - gnd
 *  
 *  use a 4.7 Kohm pull resistor between DQ and VDD
 *  
 *  note: if using the transistor packaging, don't place it too close to the cpu, since
 *  the cpu emits heat, and your readings will be off.
 *  
 *  multiple sensors can be added on the same 1-wire bus
 *  
 *  not sure of the maximum length of the wire between the sensor and the cpu
 *  
 *  
 * libraries:
 * OneWire - https://github.com/PaulStoffregen/OneWire (git)
 * DallasTemperature - https://github.com/milesburton/Arduino-Temperature-Control-Library (git)
 */

 
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2  // D4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer;
unsigned long lastTempRequest = 0;
int delayInMillis = 0;
#define resolution 12
#define TEMP_ERROR -999
float lastTemp = TEMP_ERROR;

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  Serial.print(F("\n\n DS18B20 temperature test\n\n"));

  setupTemperature();
}


bool setupTemperature(void) {
  sensors.begin();
  if (sensors.getDeviceCount() != 1) {
    Serial.println("Unable to locate temperature sensor");
    return false;
  }
  if (!sensors.getAddress(thermometer, 0)) {
    Serial.println("Unable to find address for temperature sensor"); 
    return false;
  }
  sensors.setResolution(thermometer, resolution);
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  delayInMillis = 750 / (1 << (12 - resolution)); 
  lastTempRequest = millis(); 

  return true;
}


void loop(void)
{
  unsigned long time = millis();
  checkTemperature(time);
}

void checkTemperature(unsigned long time) {
  if (time - lastTempRequest >= delayInMillis) // waited long enough??
  {
//  Serial.println(F("checking temperature"));
    checkTemperature();
        
    sensors.requestTemperatures(); 
    lastTempRequest = millis(); 
  }
}


void checkTemperature(void) {
  float tempF = sensors.getTempF(thermometer);
  tempF = (float)round(tempF*10)/10.0;
  if ( tempF == lastTemp )
    return;
    
  lastTemp = tempF;

  Serial.print("Temperature is ");
  Serial.println(tempF);
}
