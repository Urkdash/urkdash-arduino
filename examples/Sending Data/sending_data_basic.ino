/*
   Basic example of receiving data from the dashboard.

    - This code is an example of how to use the Dashboard library with input widgets.
    - In this case the first widget has the zero position [0] and the second widget has the one position [1] (you get the idea).

    NOTE: If you're using Platformio change the file for .cpp and add the <Arduino.h> Library
*/

// Libraries
#include <WiFi.h>
#include <urkdash.h>

const char *wifi_ssid = "YOUR_SSID";         // your network SSID (name)
const char *wifi_password = "YOUR_PASSWORD"; // your network password

String dev_id = "REPLACE_WITH_YOUR_DEVICE_ID";                 // your device id
String webhook_password = "REPLACE_WITH_YOUR_DEVICE_PASSWORD"; // your device password

Urkdash dash;

void setup_wifi();
void data();

void setup_wifi()
{
  Serial.print("\n\n\nWiFi Connection in Progress");
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
          Serial.print("  ⤵");
          Serial.print("\n\n         Ups WiFi Connection Failed :( ");
          Serial.println(" -> Restarting...");
          delay(2000);
          ESP.restart();
      }

        Serial.print("  ⤵");
    }
  
  // Print the IP address
  Serial.println("\n\n         WiFi Connection -> SUCCESS :)");
  Serial.print("\n         Local IP -> ");
  Serial.print(WiFi.localIP());

  // Print WiFi Signal Quality Strength
  Serial.print("\n         WiFi Intensity -> ");
  Serial.print(WiFi.RSSI());
  Serial.print(" dBm");
}

void setup()
{
  Serial.begin(9600);                               // Serial Monitor Begin
  setup_wifi();                                     // setup wifi connection
  dash.setup_credentials(dev_id, webhook_password); // Setup Credentials
}

void loop()
{
  data();                       // get data from the dashboard
  dash.check_mqtt_connection(); // check mqtt connection
}

void data()
{
  // Data example
  int temp = random(0, 100);
  int hum = random(0, 100);

  // Send data to dashboard
  dash.send_data(0, true, String(temp));
  dash.send_data(1, true, String(hum));
}