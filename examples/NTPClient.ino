/*
   This file is part of the NTPClient library.

   Copyright 2016 - 2017 Sacha Telgenhof (stelgenhof@gmail.com). All rights reserved.

   For the full copyright and license information, please view the LICENSE
   file that was distributed with this source code.
 */

#include <ESP8266WiFi.h>
#include <NTPClient.h>

const char *WIFI_SSID = "*****"; //  Your WiFi network SSID (name)
const char *WIFI_PSK = "*****";  // Your WiFi network PSK (password)

// Event Handler when an IP address has been assigned
// Once connected to WiFi, start the NTP Client
void onSTAGotIP(WiFiEventStationModeGotIP event) {
  Serial.printf("Got IP: %s\n", event.ip.toString().c_str());
  NTP.init((char *)"pool.ntp.org", UTC0900);
  NTP.setPollingInterval(60); // Poll every minute
}

// Event Handler when WiFi is disconnected
void onSTADisconnected(WiFiEventStationModeDisconnected event) {
  Serial.printf("WiFi connection (%s) dropped.\n", event.ssid.c_str());
  Serial.printf("Reason: %d\n", event.reason);
}

void setup() {
  static WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

  Serial.begin(57600);
  Serial.println();

  NTP.onSyncEvent([](NTPSyncEvent_t ntpEvent) {
    switch (ntpEvent) {
    case NTP_EVENT_INIT:
      break;
    case NTP_EVENT_STOP:
      break;
    case NTP_EVENT_NO_RESPONSE:
      Serial.printf("NTP server not reachable.\n");
      break;
    case NTP_EVENT_SYNCHRONIZED:
      Serial.printf("Got NTP time: %s\n", NTP.getTimeDate(NTP.getLastSync()));
      break;
    }
  });

  gotIpEventHandler = WiFi.onStationModeGotIP(onSTAGotIP);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(onSTADisconnected);

  // Connecting to a WiFi network
  Serial.printf("Connecting to WiFi network: %s \n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  static int previousMillis = 0;

  // Update time status every 5 seconds
  if ((millis() - previousMillis) > 5000) {
    previousMillis = millis();

    // Output only when time is set
    if (timeStatus() != timeSet) {
      return;
    }

    Serial.printf("Current time: %s - First synchronized at: %s.\n",
                  NTP.getTimeDate(now()), NTP.getTimeDate(NTP.getFirstSync()));
  }
}
