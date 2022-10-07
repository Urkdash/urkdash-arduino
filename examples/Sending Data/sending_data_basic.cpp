/*
   Basic example of sending data to the dashboard.

    - This code is an example of how to use the Dashboard library with input widgets.
    - In this case the first widget has the zero position [0] and the second widget has the one position [1] (you get the idea).
    - Dashboard template: 
      - The first widget is an column chart.
      - The second widget is a line chart.
      - The third widget is a wifi signal strength chart.

    NOTE: This .cpp file is used for PlatformIO IDE (Visual Studio Code).
*/

// Libraries
#include <Arduino.h>
#include <WiFi.h>
#include "Colors.h"
#include "urkdash.h"

#define LED_PIN 2

const char *wifi_ssid = "YOUR_SSID";         // your network SSID (name)
const char *wifi_password = "YOUR_PASSWORD"; // your network password

String dev_id = "YOUR_DEVICE_ID";                 // your device id
String webhook_password = "YOUR_DEVICE_PASSWORD"; // your device password

DashTemplate dash;

// Functions
void setup_wifi();
void data();

void setup_wifi()
{
    Serial.print(underlinePurple + "\n\n\nWiFi Connection in Progress" + fontReset + Purple);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);

    int counter = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        counter++;

        if (counter > 10)
        {
            Serial.print("  ⤵" + fontReset);
            Serial.print(Red + "\n\n         Ups WiFi Connection Failed :( ");
            Serial.println(" -> Restarting..." + fontReset);
            delay(2000);
            ESP.restart();
        }

        Serial.print("  ⤵" + fontReset);
    }

    // Print the IP address
    Serial.println(boldGreen + "\n\n         WiFi Connection -> SUCCESS :)" + fontReset);
    Serial.print("\n         Local IP -> ");
    Serial.print(boldBlue);
    Serial.print(WiFi.localIP());
    Serial.println(fontReset);

    // Print WiFi Signal Quality Strength
    Serial.print("\n         WiFi Intensity -> ");
    Serial.print(boldBlue);
    Serial.print(WiFi.RSSI());
    Serial.print(" dBm");
    Serial.println(fontReset);

    // Print WiFi Signal Status
    if (WiFi.RSSI() < -90)
    {
        Serial.print("\n         WiFi Status -> ");
        Serial.print(boldRed);
        Serial.print("Unusable");
    }
    else if (WiFi.RSSI() < -80)
    {
        Serial.print("\n         WiFi Status -> ");
        Serial.print(boldYellow);
        Serial.print("Not Good");
    }
    else if (WiFi.RSSI() < -70)
    {
        Serial.print("\n         WiFi Status -> ");
        Serial.print(boldGreen);
        Serial.print("Okay");
    }
    else if (WiFi.RSSI() < -60)
    {
        Serial.print("\n         WiFi Status -> ");
        Serial.print(boldGreen);
        Serial.print("Good");
    }
    else
    {
        Serial.print("\n         WiFi Status -> ");
        Serial.print(boldGreen);
        Serial.print("Excellent");
    }
}

void setup()
{
    Serial.begin(9600);                               // Serial Monitor Begin
    pinMode(LED_PIN, OUTPUT);                         // LED Pin Mode
    dash.setup_credentials(dev_id, webhook_password); // Setup Credentials
    dash.clear();                                     // clear the screen
    setup_wifi();                                     // setup wifi connection
    dash.setup_ntp();                                 // setup ntp connection
}

void loop()
{
    data();                       // get data from the dashboard
    dash.check_mqtt_connection(); // check mqtt connection
    dash.send_data_to_broker();   // send data to broker
}

void data()
{
    // Line chart (it could be any output chart but for historical charts you have to save the data)
    int temperature = random(20, 30); // random value (fake data)
    int humidity = random(-70, -35);  // random value (fake data)
    // Wifi Signal Strength Widget
    int wifi = WiFi.RSSI(); // WiFi Signal Strength

    // Send data to the dashboard
    dash.send_data(0, true, String(temperature)); // send data to the first widget with the position [0] and save it (true input)
    dash.send_data(1, true, String(humidity));    // send data to the second widget with the position [1] and save it (true input)
    dash.send_data(2, false, String(wifi));       // send data to the third widget with the position [2] and don't save it (false input)
}