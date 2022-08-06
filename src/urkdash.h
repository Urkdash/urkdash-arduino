#include <Arduino.h>

#ifndef URKDASH_H
#define URKDASH_H

class Templates {
  public:
    bool output(int position, String value);
    bool input(int position);
    void process_incoming_message(String topic, String incoming);
    void callback(char* topic, byte* payload, unsigned int length);
    void send_data_to_broker();
    void check_mqtt_connection();
    void clear();
    void print_stats();
};
#endif 