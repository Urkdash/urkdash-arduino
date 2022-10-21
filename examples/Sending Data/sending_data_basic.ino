/*
   Basic example of receiving data from the dashboard.

    - This code is an example of how to use the Dashboard library with input widgets.
    - In this case the first widget has the zero position [0] and the second widget has the one position [1] (you get the idea).

    NOTE: This .ino file is used for Arduino IDE and not for PlatformIO.
*/

// Libraries
#include <WiFi.h>
#include <WiFiManager.h>
#include <urkdash.h>

String dev_id = "REPLACE_WITH_YOUR_DEVICE_ID";                 // your device id
String webhook_password = "REPLACE_WITH_YOUR_DEVICE_PASSWORD"; // your device password

DashTemplate dash;

void setup_wifi();
void data();

void setup_wifi()
{
  WiFi.mode(WIFI_STA);

  Serial.begin(9600);

  WiFiManager wm;

  bool res;

  res = wm.autoConnect("AutoConnectAP", "password");

  if (!res)
  {
    Serial.println("Failed to connect");
    ESP.restart();
  }
  else
  {
    Serial.println("\n\n         WiFi Connection -> SUCCESS :)");
    delay(1000);
    // Print the IP address
    Serial.print("\n\n         Local IP -> ");
    Serial.println(WiFi.localIP());
    delay(1000);
    // Print WiFi Signal Quality Strength
    Serial.print("\n         WiFi Intensity -> ");
    Serial.print(WiFi.RSSI());
    Serial.print(" dBm");
    delay(2000);
  }
}

void setup()
{
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