#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
#include <SPI.h>
#include <Wire.h>
//#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000
};



static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.brightness(100);
  display.display();
}

boolean showdate;
boolean showSpeed;
uint32_t ticks = 0;
void loop()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  //myloc10.008629554662656, 76.32637991425928
  static const double MY_HOME_LAT = 9.962908, MY_HOME_LON = 76.305941;

  Serial.print("Satellites:");
  Serial.println(gps.satellites.value());

  display.setCursor(0, 0);
  display.print("Sats:");
  display.print(gps.satellites.value());
  if (!gps.location.isValid()) 
  {
    Serial.print("Waitng for GPS data..");
    display.setCursor(0, 20);
    display.print("Waitng for GPS data..");
   
  }
  

  if (gps.date.isValid()) {
    Serial.print("Date-Time: ");
    char sd[32];
    sprintf(sd, "%02d/%02d/%02d", gps.date.day(), gps.date.month(), gps.date.year() % 100);
    Serial.print(sd);
    Serial.print(" ");
    char st[32];




    sprintf(st, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());

    Serial.println(st);
    if (showdate) {
      //IST time from UTC
      uint8_t h = gps.time.hour();
      uint8_t m = gps.time.minute();
      if (m + 30 >= 60)
      {
        m = m - 30;
        h++;
      }
      else
      {
        m += 30;
      }
      if (h + 5 >= 24)
      {
        h = h - 19;
      }
      else
      {
        h += 5;
      }
      display.setCursor(0, 10);
      display.print("DT:");
      display.print(sd);
      display.print(" ");
      sprintf(st, "%02d:%02d:%02d", h, m, gps.time.second());
      display.print(st);
    }
  }

  if (gps.location.isValid())
  {
    Serial.print("lat: ");
    Serial.print(gps.location.lat());
    Serial.print(" lng: ");
    Serial.println(gps.location.lng());
    if (!showdate)
    {
      display.setCursor(0, 10);
      display.print("lat:");
      display.print(gps.location.lat());
      display.setCursor(64, 10);
      display.print("lng:");
      display.print(gps.location.lng());
    }
  }

  if (gps.speed.isValid())
  {
    display.setCursor(64, 0);
    display.print("v:");
    display.print(gps.speed.kmph());
    
    display.print(" kmh");

    Serial.print(gps.speed.kmph());
    Serial.println(" kmh");
  }
  else
  {
    Serial.print("---");
    Serial.println("kmh");
    display.setCursor(64, 0);
    display.print("v:");
    display.print("--");
    display.print(" kmh");
  }

  if (gps.location.isValid())
  {
    unsigned long distanceKmToMyHome =
      (unsigned long)TinyGPSPlus::distanceBetween(
        gps.location.lat(),
        gps.location.lng(),
        MY_HOME_LAT,
        MY_HOME_LON) / 1000;

    Serial.print("Origin: ");
    Serial.print(distanceKmToMyHome);
    Serial.println(" kms away");

    display.setCursor(0, 20);
    display.print("Home:");
    display.print(distanceKmToMyHome);
    display.print("km");

  }

  if (gps.course.isValid())
  {
    double courseToMyHome =
      TinyGPSPlus::courseTo(
        gps.location.lat(),
        gps.location.lng(),
        MY_HOME_LAT,
        MY_HOME_LON);


    const char *cardinalToMyHome = TinyGPSPlus::cardinal(courseToMyHome);
    Serial.print("Heading: ");

    Serial.println(cardinalToMyHome);
    display.setCursor(64, 20);
    display.print("Dir:");
    display.print(cardinalToMyHome);

  }
  if (ticks % 5 == 0)
  {
    if(showSpeed)
    {
      showSpeed=false;
      }
      else
      {
        showSpeed=true;
        }
  }
  if(showSpeed)
  {
    if (gps.speed.isValid())
    {
      display.clearDisplay();
      display.setTextSize(3);
      display.setCursor(10, 5);
      uint32_t spd=(uint32_t)gps.speed.kmph();
      //uint32_t spd=(uint32_t)(22.22f);
      display.print(spd);
      display.setTextSize(1);
      display.setCursor(70, 20);
      display.print(" kmh");
    }
    }
  display.display();
  smartDelay(1000);
  ticks++;
  if (showdate)
  {
    showdate = false;
  }
  else
  {
    showdate = true;
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
