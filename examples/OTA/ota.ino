// Libraries
//#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32httpUpdate.h>
//#include <ESPmDNS.h>
#include <Update.h>
#include <urkdash.h>
#include <HTTPClient.h>

#define firmware_version "1.0.0"

const char *wifi_ssid = "urkdash";        // your network SSID (name)
const char *wifi_password = "urkdash123"; // your network password
const char *host = "esp32";               // your device name

String dev_id = "MgC4P41Nor";                                                     // your device id
String webhook_password = "jKXyMMA3yX";                                           // your device password
String Urkdash_url = "http://192.168.234.8:3001/api/download?fileName=" + dev_id; // your urkdash url

Urkdash dash;

HTTPClient myhttp;

void setup_wifi();
void data();
bool ota_update();

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
  ota_update();                                     // check for ota update
}

void loop()
{
  dash.check_mqtt_connection(); // check mqtt connection
  data();                       // send data to urkdash
}

bool ota_update()
{
  myhttp.begin(Urkdash_url);
  myhttp.addHeader("Content-Type", "application/json");

  int httpCode = myhttp.GET();

  if (httpCode < 0)
  {
    Serial.printf("[HTTP] GET... failed, error: %s (code: %d) ", myhttp.errorToString(httpCode).c_str(), httpCode);
    myhttp.end();
    return false;
  }

  if (httpCode > 0)
  {
    if (httpCode == HTTP_CODE_OK)
    {
      t_httpUpdate_return ret = ESPhttpUpdate.update(Urkdash_url);

      switch (ret)
      {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
      }
    }
  }
  else
  {
    Serial.println("You are up to date!");
  }

  return true;
}

void data()
{
  String data1 = "Nice";
  String data2 = "Bad";
  String data3 = "Very bad";
  int data4 = random(0, 100);
  int data5 = random(0, 100);
  int data6 = random(0, 100);
  int data7 = random(20, 23);
  int data8 = WiFi.RSSI();

  dash.send_data(0, true, data1);
  dash.send_data(1, true, data2);
  dash.send_data(2, true, data3);
  dash.send_data(3, true, String(data4));
  dash.send_data(4, true, String(data5));
  dash.send_data(5, true, String(data6));
  dash.send_data(6, true, String(data7));
  dash.send_data(7, true, String(data8));
}