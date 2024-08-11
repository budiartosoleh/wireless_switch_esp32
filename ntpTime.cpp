#include <Arduino.h>
#include "wifi_handler.h"

#include "WiFiUdp.h"
#include "NTPClient.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to WiFi...");
  connect_wifi();
  Serial.println("Connecter !!");
  timeClient.begin();
  timeClient.setTimeOffset(7*3600);
  timeClient.update();
 
}

void loop() {
  Serial.print("Current time : ");
  Serial.print(timeClient.getFormattedTime());
  delay(10000);
}
