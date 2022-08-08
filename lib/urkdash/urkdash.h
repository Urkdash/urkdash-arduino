#ifndef URKDASH_H
#define URKDASH_H

#define NTP_ADRESS "co.pool.ntp.org"
#define NTP_OFFSET 0
#define NTP_INTERVAL 60 * 1000
#define MQTT_SERVER "ceresdash.online"
#define MQTT_PORT 1883
#define WEBHOOK_ENDPOINT "http://ceresdash.online:3001/api/getdevicecredentials"

#include <Arduino.h>

class DashTemplate
{
public:
  // User functions
  void input(int position, String value);
  String output(int position);
  void setup_ntp();
  // Template functions
  bool get_mqtt_credentials();
  bool reconnect();
  void setup_credentials(String dId, String webhook_pass);
  void process_incoming_message(String topic, String incoming);
  /* void callback(char *topic, byte *payload, unsigned int length); */
  void send_data_to_broker();
  void check_mqtt_connection();
  void clear();
  void print_stats();

private:
};
#endif