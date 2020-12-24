//for 7seg
#include <InvertedTM1638.h>
#include <TM1638.h>
#include <TM1638QYF.h>
#include <TM1640.h>
#include <TM16XX.h>
#include <TM16XXFonts.h>
//for rtc
#include <DS3231.h>
#include <Streaming.h>
#include <SPI.h>
#include <Wire.h>
//for oled
//#include <Streaming.h>
#include <iomanip>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//7segDefs
#define TM1638_STB D5
#define TM1638_CLK D6
#define TM1638_DIO D7
// OLED i2c
#define OLED_RESET -1
#define OLED_SCREEN_I2C_ADDRESS 0x3C
TM1638 module(TM1638_DIO, TM1638_CLK, TM1638_STB);
DS3231 rtc;
Adafruit_SSD1306 display(OLED_RESET);
int hour;
int minute;
int second;
double pi = 3.14159;
bool h12;
bool PM;
RifTime t;
char buffer[24];
int raiseP(int base, int expo)
{
    int acc = 1;
    for (int i = 0; i < expo; i++)
    {
        acc *= base;
    }
    return acc;
}

int char_to_int(char *c)
{
    switch (*c)
    {
    case '0':
        return 0;
        break;
    case '1':
        return 1;
        break;
    case '2':
        return 2;
        break;
    case '3':
        return 3;
        break;
    case '4':
        return 4;
        break;
    case '5':
        return 5;
        break;
    case '6':
        return 6;
        break;
    case '7':
        return 7;
        break;
    case '8':
        return 8;
        break;
    case '9':
        return 9;
        break;

    default:
        return 0;
        break;
    }
}

int timehandler_nd(char *displayedTime)
{
    int arr[6];
    int counter = 0;
    for (int i = 0; i < 8; i++)
    {
        if (i != 2 && i != 5)
        {
            arr[counter] = char_to_int(displayedTime + i);
            counter++;
        }
    }
    int total = 0;
    for (int i = 0; i < 6; i++)
    {
        total += arr[5 - i] * raiseP(10, i);
    }
    return total;
}

void show_clock(int t)
{
    int h = t / 10000;
    int m = (t - h) / 100;
    int s = t - m - h;
    double hAngle = pi * ((h % 12) / 12);
    double mAngle = pi * (m / 60);
    double sAngle = pi * (s / 60);
    double hourX = 16 * sin(hAngle);
    double hourY = 16 * cos(hAngle);
}

void setup()
{
    // set up the 7-segement display
    module.clearDisplay();
    module.setupDisplay(true, 2);
    // set up a pattern on the LEDs
    module.setLEDs(0x0001);
    //set up for rtc
    Wire.begin();
    Serial.begin(115200);
    //Serial.begin(115200);
    Serial << endl
           << "Hello World" << endl;
    // -- OLED --------------
    display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);

    display.display();
    delay(2000);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1); // - a line is 21 chars in this size
    display.setTextColor(WHITE);
}
void loop()
{
    rtc.getTime(t);
    Serial << rtc.toString(buffer) << endl;
    Serial << rtc.getTemperature() << " C" << endl;
    Serial << "Timestamp: " << rtc.getTimestamp() << endl;
    char *displayedTime = buffer + 9;
    Serial << displayedTime;
    display.clearDisplay();
    display.setCursor(0, 0);

    display << "Now is the winter" << endl;
    display << "of our discontent" << endl;
    display << "made glorious summer" << endl;
    display << "by this son of York." << endl;

    display.display();

    delay(5000);
    display.clearDisplay();
    display.display();
    delay(5000);
    module.setDisplayToDecNumber(timehandler_nd(displayedTime), 0, false);
    for (int i = 0x0001; i < 0x0100; i = i * 2)
    {
        module.setLEDs(i);
        delay(100);
    }
    delay(200);
}
