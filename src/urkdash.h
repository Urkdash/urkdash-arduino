#ifndef URKDASH_H
#define URKDASH_H

#define NTP_ADRESS "co.pool.ntp.org"
#define NTP_OFFSET 0
#define NTP_INTERVAL 60 * 1000

#include <Arduino.h>

class DashTemplate {
  public:
    bool output(int position, String value);
    bool input(int position);
    void setup_ntp();
    void process_incoming_message(String topic, String incoming);
    void callback(char* topic, byte* payload, unsigned int length);
    void send_data_to_broker();
    void check_mqtt_connection();
    void clear();
    void print_stats();
};
#endif 