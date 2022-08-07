#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include "IoTicosSplitter.h"
#include "urkdash.h"
#include "Colors.h"
#if defined(ESP8266) || defined(ESP32)
#include <WiFi.h>
#endif

// GLOBAL VARIABLES
HTTPClient http;
WiFiClient espclient;
PubSubClient client(espclient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADRESS, NTP_OFFSET, NTP_INTERVAL);
IoTicosSplitter splitter;
String last_received_msg = "";
String last_received_topic = "";
long varsLastSend[20];
long lastReconnectAttemp = 0;
long lastStats = 0;

DynamicJsonDocument mqtt_data_doc(2048);

// LIST OF FUNCTIONS
bool get_mqtt_credentials();
void check_mqtt_connection();
bool reconnect();
void process_sensors();
void process_actuators();
void set_callback();
void send_data_to_broker();
void callback(char *topic, byte *payload, unsigned int length);
void process_incoming_msg(String topic, String incoming);
void print_stats();
void clear();

DashTemplate::DashTemplate()
{
}

void DashTemplate::setup_ntp()
{
    timeClient.begin();
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
}

bool DashTemplate::input(int position)
{
    if (mqtt_data_doc["variables"][position]["value"] == "true")
    {
        mqtt_data_doc["variables"][position]["value"] = "";
        return true;
    }
    else if (mqtt_data_doc["variables"][position]["value"] == "false")
    {
        mqtt_data_doc["variables"][position]["value"] = "";
        return false;
    }
    else
    {
        return mqtt_data_doc["variables"][position]["value"];
    }
}

bool DashTemplate::output(int position, String value)
{
    mqtt_data_doc["variables"][position]["value"] = value;
    return true;
}

bool DashTemplate::get_mqtt_credentials()
{                                                         
    Serial.print(underlinePurple + "\n\n\nGetting MQTT Credentials from WebHook" + fontReset + Purple + "  ⤵");
    delay(1000);

    String toSend = "dId=" + dId + "&password=" + webhook_pass;

    http.begin(WEBHOOK_ENDPOINT);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int response_code = http.POST(toSend);

    if (response_code < 0)
    {
        Serial.print(boldRed + "\n\n         Error Sending Post Request :( " + fontReset);
        http.end();
        return false;
    }

    if (response_code != 200)
    {
        Serial.print(boldRed + "\n\n         Error in response :(   e-> " + fontReset + " " + response_code);
        http.end();
        return false;
    }

    if (response_code == 200)
    {
        String responseBody = http.getString();

        Serial.print(boldGreen + "\n\n         Mqtt Credentials Obtained Successfully :) " + fontReset);
        deserializeJson(mqtt_data_doc, responseBody);
        http.end();
        delay(1000);
    }

    return true;
}

bool DashTemplate::reconnect()
{
    if (!get_mqtt_credentials())
    {
        Serial.println(boldRed + "\n\n      Error getting mqtt credentials :( \n\n RESTARTING IN 10 SECONDS");
        Serial.println(fontReset);
        delay(10000);
        ESP.restart();
    }

    client.setServer(MQTT_SERVER, MQTT_PORT); // Setting up Mqtt Server

    Serial.print(underlinePurple + "\n\n\nTrying MQTT Connection" + fontReset + Purple + "  ⤵");

    String str_client_id = "device_" + dId + "_" + random(1, 9999); // Generating Client ID With Device ID and Random Number

    const char *username = mqtt_data_doc["username"]; // Retrieving username from json
    const char *password = mqtt_data_doc["password"]; // Retrieving password from json

    String str_topic = mqtt_data_doc["topic"]; // Retrieving topic from json

    if (client.connect(str_client_id.c_str(), username, password))
    {
        Serial.print(boldGreen + "\n\n         Mqtt Client Connected :) " + fontReset);
        delay(2000);

        client.subscribe((str_topic + "+/actdata").c_str()); // Subscribing to topic +/actdata
    }
    else
    {
        Serial.print(boldRed + "\n\n         Mqtt Client Connection Failed :( " + fontReset);
    }
    return true;
}

void DashTemplate::process_incoming_message(String topic, String incoming)
{
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
}

void DashTemplate::callback(char *topic, byte *payload, unsigned int length)
{
    String incoming = "";

    for (int i = 0; i < length; i++)
    {
        incoming += (char)payload[i];
    }

    incoming.trim();

    process_incoming_msg(String(topic), incoming);
}


void DashTemplate::send_data_to_broker()
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
}

void DashTemplate::check_mqtt_connection()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(Red + "\n\n         Ups WiFi Connection Failed :( ");
        Serial.println(" -> Restarting..." + fontReset);
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
        process_sensors();
        send_data_to_broker();
        print_stats();
    }
}

void DashTemplate::print_stats()
{
    long now = millis();

    if (now - lastStats > 2000)
    {
        lastStats = millis();
        clear();

        Serial.print("\n");
        Serial.print(Purple + "\n╔══════════════════════════╗" + fontReset);
        Serial.print(Purple + "\n║       SYSTEM STATS       ║" + fontReset);
        Serial.print(Purple + "\n╚══════════════════════════╝" + fontReset);
        Serial.print("\n\n");
        Serial.print("\n\n");

        Serial.print(boldCyan + "#" + " \t Name" + " \t\t Var" + " \t\t Type" + " \t\t Count" + " \t\t Last V" + fontReset + "\n\n");

        for (int i = 0; i < mqtt_data_doc["variables"].size(); i++)
        {

            String variableFullName = mqtt_data_doc["variables"][i]["variableFullName"];
            String variable = mqtt_data_doc["variables"][i]["variable"];
            String variableType = mqtt_data_doc["variables"][i]["variableType"];
            String lastMsg = mqtt_data_doc["variables"][i]["last"];
            long counter = mqtt_data_doc["variables"][i]["counter"];

            Serial.println(String(i) + " \t " + variableFullName.substring(0, 5) + " \t\t " + variable.substring(0, 10) + " \t " + variableType.substring(0, 5) + " \t\t " + String(counter).substring(0, 10) + " \t\t " + lastMsg);
        }

        Serial.print(boldGreen + "\n\n Free RAM -> " + fontReset + ESP.getFreeHeap() + " Bytes");

        Serial.print(boldGreen + "\n\n Last Incomming Msg -> " + fontReset + last_received_msg);
    }
}

void DashTemplate::clear()
{
    Serial.write(27);    // ESC command
    Serial.print("[2J"); // clear screen command
    Serial.write(27);
    Serial.print("[H"); // cursor to home command
}