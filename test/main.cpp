#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <SPL06-007.h>
#include "Adafruit_SHTC3.h"
#include "PMS.h"
#include "Colors.h"
#include "urkdash.h"

#define LED_PIN 2

const char *wifi_ssid = "Redmi";         // your network SSID (name)
const char *wifi_password = "Redmi2020"; // your network password

String dev_id = "12345";
String webhook_password = "0J699GBOeZ";

DashTemplate dash;
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

PMS pms(Serial);
PMS::DATA datas;

String topic;
String incoming;

float temperature;
float hum;
double pressure;
int PM01Value;
int PM2_5Value;
int PM10Value;
int wifi_intensity;

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
  Serial.begin(9600);       // Serial Monitor Begin
  pinMode(LED_PIN, OUTPUT); // LED Pin Mode
  dash.setup_credentials(dev_id, webhook_password);
  dash.clear();     // clear the screen
  setup_wifi();     // setup wifi connection
  dash.setup_ntp(); // setup ntp connection
  Wire.begin();
  SPL_init();
  shtc3.begin();
}

void loop()
{
  data();
  dash.check_mqtt_connection(); // check mqtt connection
  dash.send_data_to_broker();   // plantilla
}

void data()
{
  sensors_event_t humidity, temp;

  shtc3.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data

  temperature = temp.temperature;
  hum = humidity.relative_humidity;
  pressure = get_pressure();
  wifi_intensity = WiFi.RSSI();

  if (pms.read(datas))
  {

    PM01Value = datas.PM_AE_UG_1_0;
    PM2_5Value = datas.PM_AE_UG_2_5;
    PM10Value = datas.PM_AE_UG_10_0;
  }

  dash.input(0, String(temperature));
  dash.input(1, String(temperature));
  dash.input(2, String(hum));
  dash.input(3, String(pressure));
  dash.input(4, String(PM01Value));
  dash.input(5, String(PM2_5Value));
  dash.input(6, String(PM10Value));
  dash.input(7, String(wifi_intensity));
}
