#ifndef URKDASH_H
#define URKDASH_H

#define NTP_ADRESS "co.pool.ntp.org"
#define NTP_OFFSET 0
#define NTP_INTERVAL 60 * 1000
#define MQTT_SERVER "20.38.170.0"
#define MQTT_PORT 1883
#define WEBHOOK_ENDPOINT "http://20.38.170.0:3001/api/getdevicecredentials"

#include <Arduino.h>

class DashTemplate {
  public:
    // User functions
    bool output(int position, String value);
    bool input(int position);
    void setup_ntp();
    // Template functions
    bool get_mqtt_credentials();
    bool reconnect();
    void process_incoming_message(String topic, String incoming);
    void callback(char* topic, byte* payload, unsigned int length);
    void send_data_to_broker();
    void check_mqtt_connection();
    void clear();
    void print_stats();
  private:
    String dId;
    String webhook_pass;
};
#endif 