#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include "urkdash.h"
#if defined(ESP8266) || defined(ESP32)
#include <WiFi.h>
#endif

// GLOBAL VARIABLES
HTTPClient http;
WiFiUDP ntpUDP;
WiFiClient espclient;
PubSubClient client(espclient);
NTPClient timeClient(ntpUDP, NTP_ADRESS, NTP_OFFSET, NTP_INTERVAL);
Splitter splitter;
String last_received_msg = "";
String last_received_topic = "";
long varsLastSend[20];
long lastReconnectAttemp = 0;
long lastStats = 0;
int widget_position;
int period = 10000;
unsigned long time_now = 0;
String device_id;
String device_pass;
DynamicJsonDocument mqtt_data_doc(4096);

// LIST OF FUNCTIONS
bool get_mqtt_credentials();
void check_mqtt_connection();
bool reconnect();
void info_devices();
String outputs();
String getDate();
void send_data_to_broker();
void callback(char *topic, byte *payload, unsigned int length);
void clear();
void print_stats();

// Splitter class
Splitter::Splitter()
{

}

String Splitter::split(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// TEMPLATE FUNCTIONS
void callback(char *topic, byte *payload, unsigned int length)
{
    String incoming = "";

    for (int i = 0; i < length; i++)
    {
        incoming += (char)payload[i];
    }

    incoming.trim();

    last_received_topic = topic;
    last_received_msg = incoming;

    String variable = splitter.split(topic, '/', 2);

    for (int i = 0; i < mqtt_data_doc["variables"].size(); i++)
    {

        if (mqtt_data_doc["variables"][i]["variable"] == variable)
        {

            DynamicJsonDocument doc(256);
            deserializeJson(doc, incoming);
            mqtt_data_doc["variables"][i]["last"] = doc;

            long counter = mqtt_data_doc["variables"][i]["counter"];
            counter++;
            mqtt_data_doc["variables"][i]["counter"] = counter;
        }
    }

    outputs();
}

void setup_ntp()
{
    timeClient.begin();
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
}

String Urkdash::receive_data(int position)
{
    widget_position = position;
    return outputs();
}

String outputs()
{
    varsLastSend[4] = 0;
    return mqtt_data_doc["variables"][widget_position]["last"]["value"];
}

void Urkdash::send_data(int position, bool save, String value)
{
    mqtt_data_doc["variables"][position]["last"]["value"] = value;
    mqtt_data_doc["variables"][position]["last"]["save"] = int(save);
    mqtt_data_doc["variables"][position]["last"]["lastSending"] = timeClient.getEpochTime();
    info_devices();
}

void Urkdash::map_data(int position, bool save, String lat, String lng)
{
    lat.concat("/");
    lat.concat(lng);
    mqtt_data_doc["variables"][position]["last"]["value"] = lat;
    mqtt_data_doc["variables"][position]["last"]["save"] = int(save);
    mqtt_data_doc["variables"][position]["last"]["lastSending"] = timeClient.getEpochTime();
}

void info_devices()
{
    mqtt_data_doc["info"]["ssid"] = WiFi.SSID();
    mqtt_data_doc["info"]["rssi"] = WiFi.RSSI();
    mqtt_data_doc["info"]["ip"] = WiFi.localIP().toString();
    mqtt_data_doc["info"]["mac"] = WiFi.macAddress();
    mqtt_data_doc["info"]["lastKeepAlive"] = getDate();
    mqtt_data_doc["info"]["lastRestart"] = getDate();
    mqtt_data_doc["info"]["firmwareVersion"] = "ESP32";
}

String getDate()
{
    String dateString = "";
    timeClient.update();
    dateString = timeClient.getFormattedTime();
    return dateString;
}

void Urkdash::setup_credentials(String dId, String webhook_pass)
{
    device_id = dId;
    device_pass = webhook_pass;
    client.setCallback(callback);
    clear();
}

bool get_mqtt_credentials()
{
    Serial.print("\n\n\nGetting MQTT Credentials from WebHook");
    delay(1000);

    String toSend = "dId=" + device_id + "&password=" + device_pass;

    http.begin(WEBHOOK_ENDPOINT);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int response_code = http.POST(toSend);

    if (response_code < 0)
    {
        Serial.print("\n\n         Error Sending Post Request :( ");
        http.end();
        return false;
    }

    if (response_code != 200)
    {
        Serial.print("\n\n         Error in response :(   e-> " + response_code);
        http.end();
        return false;
    }

    if (response_code == 200)
    {
        String responseBody = http.getString();

        Serial.print("\n\n         Mqtt Credentials Obtained Successfully :) ");
        deserializeJson(mqtt_data_doc, responseBody);
        http.end();
        delay(1000);
    }

    return true;
}

bool Urkdash::reconnect()
{
    if (!get_mqtt_credentials())
    {
        Serial.println("\n\n      Error getting mqtt credentials :( \n\n RESTARTING IN 10 SECONDS");
        delay(10000);
        ESP.restart();
    }

    client.setServer(MQTT_SERVER, MQTT_PORT); // Setting up Mqtt Server

    Serial.print("\n\n\nTrying MQTT Connection");

    String str_client_id = "device_" + device_id + "_" + random(1, 9999); // Generating Client ID With Device ID and Random Number

    const char *username = mqtt_data_doc["username"]; // Retrieving username from json
    const char *password = mqtt_data_doc["password"]; // Retrieving password from json

    String str_topic = mqtt_data_doc["topic"]; // Retrieving topic from json

    if (client.connect(str_client_id.c_str(), username, password))
    {
        Serial.print("\n\n         Mqtt Client Connected :) ");
        delay(2000);

        client.subscribe((str_topic + "+/actdata").c_str()); // Subscribing to topic +/actdata
    }
    else
    {
        Serial.print("\n\n         Mqtt Client Connection Failed :( ");
    }
    return true;
}

void send_data_to_broker()
{
    long now = millis();

    for (int i = 0; i < mqtt_data_doc["variables"].size(); i++)
    {

        if (mqtt_data_doc["variables"][i]["variableType"] == "output")
        {
            continue;
        }

        int freq = mqtt_data_doc["variables"][i]["variableSendFreq"];

        if (now - varsLastSend[i] > freq * 1000)
        {
            varsLastSend[i] = millis();

            String str_root_topic = mqtt_data_doc["topic"];
            String str_variable = mqtt_data_doc["variables"][i]["variable"];
            String topic = str_root_topic + str_variable + "/sdata";

            String toSend = "";

            serializeJson(mqtt_data_doc["variables"][i]["last"], toSend);

            client.publish(topic.c_str(), toSend.c_str());

            // STATS
            long counter = mqtt_data_doc["variables"][i]["counter"];
            counter++;
            mqtt_data_doc["variables"][i]["counter"] = counter;
        }
    }
    if (millis() >= time_now + period)
    {
        time_now += period;
        String str_root_topic = mqtt_data_doc["topic"];
        int str_variable = random(1, 9999);
        String topic = str_root_topic + String(str_variable) + "/idevice";
        String toSend = "";

        serializeJson(mqtt_data_doc["info"], toSend);

        client.publish(topic.c_str(), toSend.c_str());
    }
}

void Urkdash::check_mqtt_connection()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("\n\n         Ups WiFi Connection Failed :( ");
        Serial.println(" -> Restarting...");
        delay(15000);
        ESP.restart();
    }

    if (!client.connected())
    {
        long now = millis();

        if (now - lastReconnectAttemp > 5000)
        {
            lastReconnectAttemp = millis();
            if (reconnect())
            {
                lastReconnectAttemp = 0;
            }
        }
    }
    else
    {
        client.loop();
        send_data_to_broker();
        print_stats();
    }
}

void print_stats()
{
    long now = millis();

    if (now - lastStats > 2000)
    {
        lastStats = millis();
        clear();

        Serial.print("\n");
        Serial.print("\n╔══════════════════════════╗");
        Serial.print("\n║       SYSTEM STATS       ║");
        Serial.print("\n╚══════════════════════════╝");
        Serial.print("\n\n");
        Serial.print("\n\n");

        // Serial.print("#" + " \t Name" + " \t\t Var" + " \t\t Type" + " \t\t Count" + " \t\t Last V" + "\n\n");

        for (int i = 0; i < mqtt_data_doc["variables"].size(); i++)
        {

            String variableFullName = mqtt_data_doc["variables"][i]["variableFullName"];
            String variable = mqtt_data_doc["variables"][i]["variable"];
            String variableType = mqtt_data_doc["variables"][i]["variableType"];
            String lastMsg = mqtt_data_doc["variables"][i]["last"];
            long counter = mqtt_data_doc["variables"][i]["counter"];

            Serial.println(String(i) + " \t " + variableFullName.substring(0, 5) + " \t\t " + variable.substring(0, 10) + " \t " + variableType.substring(0, 5) + " \t\t " + String(counter).substring(0, 10) + " \t\t " + lastMsg);
        }

        Serial.print("\n\n Last Incomming Msg -> " + last_received_msg);
    }
}

void clear()
{
    Serial.write(27);    // ESC command
    Serial.print("[2J"); // clear screen command
    Serial.write(27);
    Serial.print("[H"); // cursor to home command
}