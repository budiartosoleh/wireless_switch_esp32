#include "wifi_handler.h"
#include <WiFi.h>
#include "secrets.h"


void connect_wifi(){
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while(WiFi.status() != WL_CONNECTED);
    delay(500);
    Serial.print(".");
    

}
void connect_wifi_enterprise(){
    int counter =0;
    WiFi.mode(WIFI_STA);

    WiFi.begin(EAP_WIFI_SSID, WPA2_AUTH_PEAP, EAP_USERNAME, EAP_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) {  //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());  //print LAN IP
}