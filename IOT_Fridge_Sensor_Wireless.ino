#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <chibi.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

char postData[100];
char temp1String[100];
char temp2String[100];

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress FridgeThermometer, FreezerThermometer;

void setup(void)
{
  chibiInit();
  
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Fridge Sensor");

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //FridgeThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //FreezerThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  // 
  // method 1: by index
  if (!sensors.getAddress(FridgeThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(FreezerThermometer, 1)) Serial.println("Unable to find address for Device 1"); 

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them.  It might be a good idea to 
  // check the CRC to make sure you didn't get garbage.  The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to FridgeThermometer
  //if (!oneWire.search(FridgeThermometer)) Serial.println("Unable to find address for FridgeThermometer");
  // assigns the seconds address found to FreezerThermometer
  //if (!oneWire.search(FreezerThermometer)) Serial.println("Unable to find address for FreezerThermometer");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(FridgeThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(FreezerThermometer);
  Serial.println();

  // set the resolution to 9 bit
  sensors.setResolution(FridgeThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(FreezerThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(FridgeThermometer), DEC); 
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(FreezerThermometer), DEC); 
  Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  //Serial.print(" Temp F: ");
  //Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to send sensor information about a device
void sendSensorData(DeviceAddress deviceAddress)
{
  //Serial.print("Device Address: ");
  //printAddress(deviceAddress);
  //Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  //Serial.println("DONE");
    
  float temp1 = sensors.getTempC(FridgeThermometer);
  float temp2 = sensors.getTempC(FreezerThermometer);
  char rssi = chibiGetRSSI();
  
  //Serial.print("Temp Fridge: ");
  //Serial.print(temp1);
  //Serial.println();
  //Serial.print("Temp Freezer: ");
  //Serial.print(temp2);
  //Serial.println();
  
  dtostrf(temp1, 4, 2, temp1String);
  dtostrf(temp2, 4, 2, temp2String);
  sprintf(postData, "T1:%s T2:%s\n", temp1String, temp2String);
  Serial.print(postData);
  //Serial.println();
  
  //n = sizeof(postData);
  //Serial.print(n);
  //Serial.println();
  
  chibiTx(BROADCAST_ADDR, (uint8_t*) postData, 20);
  
  delay(1000);
  // print the device information
  //printData(FridgeThermometer);
  //printData(FreezerThermometer);
}

