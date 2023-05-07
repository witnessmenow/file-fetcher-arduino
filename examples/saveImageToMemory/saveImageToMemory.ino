/*******************************************************************
    A sketch to fetch a image from the internet and store it in memory

    Useful for smaller images

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

// ----------------------------
// Standard Libraries
// ----------------------------

#include <WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <FileFetcher.h>

// Wifi network station credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "password"

WiFiClientSecure secured_client;
FileFetcher fileFetcher(secured_client);

bool getImage(char *imageUrl)
{

    uint8_t *imageFile; // pointer that the library will store the image at (uses malloc)
    int imageSize;      // library will update the size of the image
    bool gotImage = fileFetcher.getFile(imageUrl, &imageFile, &imageSize);

    if (gotImage)
    {
        Serial.print("Got Image");

        // imageFile is now a pointer to memory that contains the image file
        // imageSize is the size of the image

        // Use it however you need to!

        free(imageFile); // Make sure to free the memory!
    }

    return gotImage;
}

void setup()
{
    Serial.begin(115200);
    Serial.println();

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

    int returnStatus = getImage("https://i.imgur.com/ewrhVKU.png");
    Serial.print("returnStatus: ");
    Serial.println(returnStatus);
}

void loop()
{
}