#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "workhour.h"
#include "config.h"

const char* ssid = WIFI_NAME;
const char* password = WIFI_PASSWORD;
const char* mdns_name = MDNS_NAME;

#define RELAY 14  // D5 - GND
#define RESET 12  // D6 - GND
#define LED 2

WiFiClient client;
ESP8266WebServer server(80);

WorkHour WH(RELAY, RESET, LED);
unsigned long previousMillis = 0;
const long interval = 1000;

void handle_req() {
  String s = "<html>";
  s += "<h1>Sum time: " + String(WH.sum_time) + "</h1>";
  s += "<h2>Relay status: " + String(WH.saved_relay_status) + "</h2>";
  if (WH.saved_relay_status == 1) {
    unsigned long work_time = WH.calc_work_time();
    s += "<h2>Relay work time: " + String(work_time) + "</h2>";
  }
  s += "</html>";
  server.send(200, "text/html", s);
}

void setup(void) {

  Serial.begin(115200);
  WH.setup();

  Serial.print("Connecting to:");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(mdns_name)) {
    Serial.println("MDNS responder started: " + String(mdns_name) + ".local");
  }

  server.on("/", handle_req);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    WH.log_time();
  }
  WH.check_reset();
}