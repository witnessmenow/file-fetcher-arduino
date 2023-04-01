/*******************************************************************
    A sketch to fetch a image from the internet and save it on
    flash

    Parts:
    ESP32 D1 Mini stlye Dev board* - http://s.click.aliexpress.com/e/C6ds4my
    (or any ESP32 board)

    *  = Affilate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
#include <FS.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <ImageFetcher.h>

// Wifi network station credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "password"

#define IMAGE_NAME "/img.png"

WiFiClientSecure secured_client;
ImageFetcher imageFetcher(secured_client);

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

    bool gotImage = imageFetcher.getImage(imageUrl, &f);

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

    int returnStatus = getImage("https://media.formula1.com/image/upload/content/dam/fom-website/2018-redesign-assets/Track%20icons%204x3/Abu%20Dhab%20carbon.png.transform/3col/image.png");
    Serial.print("returnStatus: ");
    Serial.println(returnStatus);
}

void loop()
{
}