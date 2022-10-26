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
#include <urkdash.h>

#define LED_PIN 2

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