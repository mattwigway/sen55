
#include <Wire.h>
#include <LiquidCrystal.h>

const uint8_t SEN55_ADDR = 0x69;
const uint16_t READ_VALUES_CMD = 0x03c4;
const uint16_t START_MEASUREMENT_CMD = 0x0021;
const uint16_t FAN_CLEAN_CMD = 0x5607;
const uint16_t READ_SERIAL_NUMBER_CMD = 0xD033;
const uint16_t READ_STATUS_CMD = 0xD206;

const LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// custom characters to save space
const byte PM_PIXELS[8] = {
  0b11100,
  0b10010,
  0b11100,
  0b10000,
  0b10001,
  0b11011,
  0b10101,
  0b10001
};

const byte PM = 0;

const byte PM25_PIXELS[8] = {
  0b11000,
  0b00100,
  0b01000,
  0b11101,
  0b00110,
  0b00110,
  0b00001,
  0b00110
};

const byte PM25 = 1;

const byte PM10_PIXELS[8] = {
  0b00100,
  0b01100,
  0b00100,
  0b00000,
  0b01100,
  0b10010,
  0b10010,
  0b01100  
};

const byte PM10 = 2;

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

float readScale(float scale) {
  float raw = static_cast<float>(read16());
  return raw / scale;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);
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

  // custom characters to save display real estate
  lcd.createChar(PM, PM_PIXELS);
  lcd.createChar(PM25, PM25_PIXELS);
  lcd.createChar(PM10, PM10_PIXELS);

}

void loop() {  
  // read from sensor
  Serial.println("reading");
  Wire.beginTransmission(SEN55_ADDR);
  write16(READ_VALUES_CMD);

  Wire.requestFrom(SEN55_ADDR, 24);
  float pm1 = readScale(10.0);
  float pm25 = readScale(10.0);
  float pm4 = readScale(10.0);
  float pm10 = readScale(10.0);
  float hum = readScale(100.0);
  // convert c to f
  float tempf = readScale(200.0) * 9.0 / 5.0 + 32;
  float voc = readScale(10.0);
  float nox = readScale(10.0);
  Wire.endTransmission();

  lcd.clear();


  // just make it big so we don't run out of space even if we read wonky values
  char buf[100];

  // pm1
  lcd.setCursor(0, 0);
  lcd.write(PM);
  lcd.print("1 ");
  dtostrf(pm1, -7, 1, buf);
  lcd.print(buf);
  Serial.print("PM1");
  Serial.println(buf);

  lcd.setCursor(10, 0);
  lcd.write(PM);
  lcd.write(PM25);
  lcd.print(" ");
  dtostrf(pm25, -7, 1, buf);
  lcd.print(buf);
  Serial.print("PM2.5");
  Serial.println(buf);

  lcd.setCursor(0, 1);
  lcd.write(PM);
  lcd.print("4 ");
  dtostrf(pm4, -7, 1, buf);
  lcd.print(buf);
  Serial.print("PM4");
  Serial.println(buf);

  lcd.setCursor(10, 1);
  lcd.write(PM);
  lcd.write(PM10);
  lcd.print(" ");
  dtostrf(pm10, -7, 1, buf);
  lcd.print(buf);
  Serial.print("PM10");
  Serial.println(buf);

  lcd.setCursor(0, 2);
  dtostrf(tempf, 7, 2, buf);
  lcd.print(buf);
  lcd.print("F");
  Serial.print(buf);
  Serial.println("F");

  lcd.setCursor(10, 2);
  dtostrf(hum, 7, 2, buf);
  lcd.print(buf);
  lcd.print("%");
  Serial.print(buf);
  Serial.println("%");

  lcd.setCursor(0, 3);
  dtostrf(voc, -5, 1, buf);
  lcd.print("VOC ");
  lcd.print(buf);
  Serial.println("VOC ");
  Serial.print(buf);

  lcd.setCursor(10, 3);
  dtostrf(nox, -5, 1, buf);
  lcd.print("NOx ");
  lcd.print(buf);
  Serial.println("NOx ");
  Serial.print(buf);

  delay(1000);
}
