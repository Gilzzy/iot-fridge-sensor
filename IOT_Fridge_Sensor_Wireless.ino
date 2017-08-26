#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <chibi.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

int temp = 0;
int humid = 0;
float finalTemp = 0.0;
float finalHumid = 0.0;  
byte first = 0;
byte stat = 0;
byte message[100];
byte newline[] = "\n";
char postData[100];
char tempString[100];
char humidString[100];

//Setup function - this is run every time we wakeup
void setup() {
  chibiInit();
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

}

//Nothing to do in the main loop, we are setup only then sleep
void loop() {
  int i = 0;
  Wire.begin();
  Wire.beginTransmission(39);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(1000);
  Wire.requestFrom(39, 4);
  if (4 == Wire.available()) {
    first = Wire.read();
    stat = first & 0xc0;
    if (stat == 0) {
      humid = ((first & 0x3f) << 8) | Wire.read();
      temp = ((Wire.read() << 8) | (Wire.read() & 0xfc)) >> 2;
      finalTemp = (temp / (pow(2, 14) - 2)) * 165 - 40;
      finalHumid = (humid / (pow(2, 14) - 2)) * 100;

      delay(1000);
      
      Serial.print(temp);
      Serial.print(humid);
      
      dtostrf(finalTemp, 4, 2, tempString);
      dtostrf(finalHumid, 4, 2, humidString);
      sprintf(postData, "{\"temperature\":%s,\"humidity\":%s}", tempString, humidString);
      sprintf(postData, "T:%s", tempString);
      
      chibiTx(BROADCAST_ADDR,  message, 12);
      chibiTx(BROADCAST_ADDR, newline, 12);
    }
  }
  
}
