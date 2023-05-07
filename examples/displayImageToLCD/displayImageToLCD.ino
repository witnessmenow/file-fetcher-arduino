/*******************************************************************
    A sketch to fetch a PNG image from the internet, save it to
    flash and display it on an 320 x 240 LCD

    Parts:
    ESP32 With Built in 320x240 LCD with Touch Screen (ESP32-2432S028R)
    https://github.com/witnessmenow/Spotify-Diy-Thing#hardware-required

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
// ----------------------------
// Standard Libraries
// ----------------------------

#include <FS.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <FileFetcher.h>

#include <PNGdec.h>
// A library for decoding PNG

// Can be installed from the library manager (Search for "PNGdec")
// https://github.com/bitbank2/PNGdec

#include <TFT_eSPI.h>
// A library for writing to the LCD
// NOTE: This library requires you to add config file to it
// steps are described here: https://github.com/witnessmenow/Spotify-Diy-Thing#display-config

// Can be installed from the library manager (Search for "TFT_eSPI")
// https://github.com/Bodmer/TFT_eSPI

TFT_eSPI tft = TFT_eSPI();
PNG png;

// Wifi network station credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "password"

#define IMAGE_NAME "/img.png"

WiFiClientSecure secured_client;
FileFetcher fileFetcher(secured_client);

void PNGDraw(PNGDRAW *pDraw)
{
    uint16_t usPixels[320];

    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    tft.pushImage(0, pDraw->y, pDraw->iWidth, 1, usPixels);
}

fs::File myfile;

void *myOpen(const char *filename, int32_t *size)
{
    myfile = SPIFFS.open(filename);
    *size = myfile.size();
    return &myfile;
}
void myClose(void *handle)
{
    if (myfile)
        myfile.close();
}
int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length)
{
    if (!myfile)
        return 0;
    return myfile.read(buffer, length);
}
int32_t mySeek(PNGFILE *handle, int32_t position)
{
    if (!myfile)
        return 0;
    return myfile.seek(position);
}

int displayImage(char *imageFileUri)
{
    tft.fillScreen(TFT_BLACK);
    unsigned long lTime = millis();
    lTime = millis();
    Serial.println(imageFileUri);
    int rc = png.open((const char *)imageFileUri, myOpen, myClose, myRead, mySeek, PNGDraw);
    if (rc == PNG_SUCCESS)
    {
        Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        rc = png.decode(NULL, 0);
        png.close();
    }
    else
    {
        Serial.print("error code: ");
        Serial.println(rc);
    }

    Serial.print("Time taken to decode and display Image (ms): ");
    Serial.println(millis() - lTime);

    return rc;
}

int getImage(char *imageUrl)
{
    // In this example I reuse the same filename
    // over and over
    if (SPIFFS.exists(IMAGE_NAME) == true)
    {
        Serial.println("Removing existing image");
        SPIFFS.remove(IMAGE_NAME);
    }

    fs::File f = SPIFFS.open(IMAGE_NAME, "w+");
    if (!f)
    {
        Serial.println("file open failed");
        return -1;
    }

    bool gotImage = fileFetcher.getFile(imageUrl, &f);

    // Make sure to close the file!
    f.close();

    return gotImage;
}

void setup()
{
    Serial.begin(115200);
    Serial.println();

    // Initialise SPIFFS, if this fails try .begin(true)
    // NOTE: I believe this formats it though it will erase everything on
    // spiffs already! In this example that is not a problem.
    // I have found once I used the true flag once, I could use it
    // without the true flag after that.
    bool spiffsInitSuccess = SPIFFS.begin(false) || SPIFFS.begin(true);
    if (!spiffsInitSuccess)
    {
        Serial.println("SPIFFS initialisation failed!");
        while (1)
            yield(); // Stay here twiddling thumbs waiting
    }

    // attempt to connect to Wifi network:
    Serial.print("Connecting to Wifi SSID ");
    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    secured_client.setInsecure(); // You should probably use a cert for this!

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    int returnStatus = getImage("https://i.imgur.com/ewrhVKU.png");
    Serial.print("returnStatus: ");
    Serial.println(returnStatus);
    if (returnStatus)
    {
        displayImage(IMAGE_NAME);
    }
}

void loop()
{
}