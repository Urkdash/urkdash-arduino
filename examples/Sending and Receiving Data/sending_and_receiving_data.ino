/*
   Example of sending and receiving data from the dashboard.

    - This code is an example of how to use the Dashboard library with output/input widgets.
    - In this case the first widget has the zero position [0] and the second widget has the one position [1] (you get the idea).
    - Dashboard template: 
      - The first widget is an area chart.
      - The second widget is a line chart.
      - The third widget is a wifi signal strength chart.
      - The fourth widget is a button.
      - The fifth widget is a switch.

    NOTE: If you're using Platformio change the file for .cpp and add the <Arduino.h> Library
*/

// Libraries
#include <WiFi.h>
#include <WiFiManager.h>
#include <urkdash.h>

#define LED_PIN 2

String dev_id = "REPLACE_WITH_YOUR_DEVICE_ID";                 // your device id
String webhook_password = "REPLACE_WITH_YOUR_DEVICE_PASSWORD"; // your device password

Urkdash dash;

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
  pinMode(LED_PIN, OUTPUT);
  dash.setup_credentials(dev_id, webhook_password); // Setup Credentials
}

void data()
{
    // HISTORICAL WIDGETS
    int temperature = random(20, 30); 
    int humidity = random(-70, -35);  

    // WIFI WIDGET
    int wifi = WiFi.RSSI();

    // Send data to the dashboard
    dash.send_data(0, true, String(temperature)); 
    dash.send_data(1, true, String(humidity));    
    dash.send_data(2, false, String(wifi));     
    
    // BUTTON
    // Receiving data from the fourth widget
    if (dash.receive_data(3) == "restart")
    {
        Serial.println("Restarting...");
        delay(2000);
        ESP.restart();
    }

    // SWITCH
    // Receiving data from the fifth widget
    if (dash.receive_data(4) == "true")
    {
        Serial.println("Turning on LED...");
        delay(2000);
        digitalWrite(LED_PIN, HIGH);
    }
    else if (dash.receive_data(4) == "false")
    {
        Serial.println("Turning off LED...");
        delay(2000);
        digitalWrite(LED_PIN, LOW);
    }
}