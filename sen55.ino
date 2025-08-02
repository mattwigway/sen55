
#include <Wire.h>
#include <LiquidCrystal.h>

const uint8_t SEN55_ADDR = 0x69; // nice
const uint16_t READ_VALUES_CMD = 0x03c4;
const uint16_t START_MEASUREMENT_CMD = 0x0021;
const uint16_t FAN_CLEAN_CMD = 0x5607;
const uint16_t READ_SERIAL_NUMBER_CMD = 0xD033;
const uint16_t READ_STATUS_CMD = 0xD206;

struct decimal {
  uint16_t whole;
  uint8_t decimal;
};

// unlike every other I2C device in the world, SEN55 uses two byte commands?
void write16(uint16_t cmd) {
  Wire.write(cmd >> 8);
  Wire.write(cmd & 255);
}

uint16_t read16() {
  uint16_t msb = Wire.read();
  uint16_t lsb = Wire.read();
  uint8_t checksum = Wire.read();

  // TODO checksum
  return (msb << 8) | lsb;
}

decimal readDecimal(uint16_t scale) {
  uint16_t raw = read16();
  decimal out;

  out.whole = raw / scale;
  out.decimal = static_cast<uint8_t>(raw / (scale / 10) % 10);

  return out;
}

void printDecimal(decimal d) {
  Serial.print(d.whole);
  Serial.print(".");
  Serial.print(d.decimal);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(1000);

  // begin readings
  Wire.beginTransmission(SEN55_ADDR);
  //Wire.write(FAN_CLEAN_CMD);
  write16(START_MEASUREMENT_CMD);
  
  //Wire.write(READ_SERIAL_NUMBER_CMD);
  //Wire.requestFrom(SEN55_ADDR, 48);
  //while (Wire.available()) {
    //Serial.write(Wire.read());
 // }
  Wire.endTransmission();

  
 delay(1000);

  Wire.beginTransmission(SEN55_ADDR);
  write16(FAN_CLEAN_CMD);
  Wire.endTransmission();

}

void loop() {
  // put your main code here, to run repeatedly:
  // read from sensor
  Serial.println("reading");
  Wire.beginTransmission(SEN55_ADDR);
  write16(READ_VALUES_CMD);

  Wire.requestFrom(SEN55_ADDR, 24);
  decimal PM1 = readDecimal(10);
  decimal PM25 = readDecimal(10);
  decimal PM4 = readDecimal(10);
  decimal PM10 = readDecimal(10);
  decimal hum = readDecimal(100);
  decimal temp = readDecimal(111); // dpcs say 200 for celsius. Farenheit is 9/5 C + 32, 200/(9/5) = 111.11111
  temp.whole += 32; // handle the c/f offset
  decimal voc = readDecimal(10);
  decimal nox = readDecimal(10);
  Wire.endTransmission();

  Serial.print("PM1: ");
  printDecimal(PM1);
  Serial.print(" PM2.5: ");
  printDecimal(PM25);
  Serial.print(" PM4: ");
  printDecimal(PM4);
  Serial.print(" PM10: ");
  printDecimal(PM10);
  Serial.print(" Humidity: ");
  printDecimal(hum);
  Serial.print("% Temp: ");
  printDecimal(temp);
  Serial.print("F VOC: ");
  printDecimal(voc);
  Serial.print(" NOx: ");
  printDecimal(nox);
  
  delay(1000);
}
