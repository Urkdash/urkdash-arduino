#include <Arduino.h>
#include "urkdash.h"

DashTemplate dash;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World!");
}

void loop() {
  delay(1000);
}

void plantilla(){
  dash.setup_ntp();
  dash.check_mqtt_connection();
  dash.send_data_to_broker();
  dash.print_stats();
}