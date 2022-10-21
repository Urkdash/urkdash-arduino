/*
   Basic example of receiving data from the dashboard.

    - This code is an example of how to use the Dashboard library with output widgets.
    - In this case the first widget has the zero position [0] and the second widget has the one position [1] (you get the idea).
    - Dashboard template: 
      - The first widget is a button.
      - The second widget is a switch.

    NOTE: This .cpp file is used for PlatformIO IDE (Visual Studio Code).
*/

// Libraries
#include <Arduino.h>
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
    // BUTTON
    // Receiving data from the first widget
    if (dash.receive_data(0) == "restart")
    {
        Serial.println("Restarting...");
        delay(2000);
        ESP.restart();
    }

    // SWITCH
    // Receiving data from the second widget
    if (dash.receive_data(1) == "true")
    {
        Serial.println("Turning on LED...");
        delay(2000);
        digitalWrite(LED_PIN, HIGH);
    }
    else if (dash.receive_data(1) == "false")
    {
        Serial.println("Turning off LED...");
        delay(2000);
        digitalWrite(LED_PIN, LOW);
    }
}