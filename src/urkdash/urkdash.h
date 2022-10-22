#ifndef URKDASH_H
#define URKDASH_H

#define NTP_ADRESS "co.pool.ntp.org"
#define NTP_OFFSET 0
#define NTP_INTERVAL 60 * 1000
#define MQTT_SERVER "urkdash.com"
#define MQTT_PORT 1883
#define WEBHOOK_ENDPOINT "http://urkdash.com:3001/api/getdevicecredentials"

#include <Arduino.h>

class DashTemplate
{
public:
  // User functions
  void send_data(int position, bool save, String value);
  void map_data(int position, bool save, String lat, String lng); // Map widget data
  String receive_data(int position);
  // Template functions
  bool get_mqtt_credentials();
  bool reconnect();
  void setup_credentials(String dId, String webhook_pass);
  void check_mqtt_connection();
  void print_stats();

private:
};

class Splitter {
public:

	Splitter();
	String split(String data, char separator, int index);

private:

};
#endif