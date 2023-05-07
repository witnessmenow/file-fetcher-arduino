/*******************************************************************
    A sketch to fetch a file from the internet and save it on
    flash

    Parts:
    ESP32 D1 Mini stlye Dev board* - http://s.click.aliexpress.com/e/C6ds4my
    (or any ESP32 board)

       = Affilate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
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

// Wifi network station credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "password"

#define FILE_NAME "/races.json"

WiFiClientSecure secured_client;
FileFetcher fileFetcher(secured_client);

int getFile(char *fileUrl)
{
    // In this example I reuse the same filename
    // over and over
    if (SPIFFS.exists(FILE_NAME) == true)
    {
        Serial.println("Removing existing file");
        SPIFFS.remove(FILE_NAME);
    }

    fs::File f = SPIFFS.open(FILE_NAME, "w+");
    if (!f)
    {
        Serial.println("file open failed");
        return -1;
    }

    bool gotFile = fileFetcher.getFile(fileUrl, &f);

    // Make sure to close the file!
    f.close();

    return gotFile;
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

    int returnStatus = getFile("https://raw.githubusercontent.com/sportstimes/f1/main/_db/f1/2023.json");
    Serial.print("returnStatus: ");
    Serial.println(returnStatus);

    if (returnStatus)
    {
        File file2 = SPIFFS.open(FILE_NAME);
        if (!file2)
        {
            Serial.println("Failed to open file for reading");
            return -1;
        }

        Serial.println("File Content:");
        while (file2.available())
        {
            Serial.write(file2.read());
        }
        file2.close();
    }
}

void loop()
{
}