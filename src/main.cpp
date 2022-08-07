#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Colors.h"
#include "urkdash.h"

#define LED_PIN 2

const char *wifi_ssid = "FamiliaLariosMedina ";        // your network SSID (name)
const char *wifi_password = "familialariosmedina2071"; // your network password

String dev_id = "645421";
String webhook_password = "z6VjoNDVOk";

DashTemplate dash;

String topic;
String incoming;

// Functions
void setup_wifi();
void msg(String topic, String msg);
void callback(char *topic, byte *payload, unsigned int length);
void plantilla_dash();
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
  Serial.begin(9600);       // Serial Monitor Begin
  pinMode(LED_PIN, OUTPUT); // LED Pin Mode
  dash.setup_credentials(dev_id, webhook_password);
  dash.clear();             // clear the screen
  setup_wifi();             // setup wifi connection
  dash.setup_ntp(); // setup ntp connection
  // client.setCallback(callback); // Set callback function
}

void loop()
{
  dash.check_mqtt_connection(); // check mqtt connection
  plantilla_dash();             // plantilla
  data();                       // data
}

void callback(char *topic, byte *payload, unsigned int length)
{

  String incoming = "";

  for (int i = 0; i < length; i++)
  {
    incoming += (char)payload[i];
  }

  incoming.trim();

  dash.process_incoming_message(String(topic), incoming);
}

void plantilla_dash()
{
  dash.process_incoming_message(String(topic), incoming);
  callback;
  dash.send_data_to_broker();
}

void data()
{
  int area = random(20, 30);
  int wifi = random(-70, -35);
  int tank = random(0, 100);

  dash.input(0, String(area));
  dash.input(1, String(wifi));
  dash.input(2, String(tank));
}