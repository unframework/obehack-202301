// IKEA Obegraensad sketch 2023-01

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "arduino_secrets.h"
#include "src/button.h"
#include "src/effects.h"
#include "src/led.h"

/*

Sample arduino_secrets.h:

#define SECRET_SSID "mySSID"
#define SECRET_PASSWORD "password123"

*/
const char *ssid = SECRET_SSID;
const char *password = SECRET_PASSWORD;

const char *mDNSDomain = "esp8266";

WiFiUDP UDP;
unsigned int localUDPPort = 7007; // local port to listen on

unsigned char inputBuffer[256]; // pixel buffer as received over UDP

#define AMBIENCE_WAIT_MS 5000

// simple 16x16 render buffer
unsigned char renderBuffer[256];

unsigned long ambienceStartTime = 0;

void ambienceResetCountdown() {
  ambienceStartTime = millis() + AMBIENCE_WAIT_MS;
}

void ambienceYield() {
  const unsigned long currentTime = millis();

  if (currentTime > ambienceStartTime) {
    render16x16(renderBuffer);
    updateRenderQueue(renderBuffer);
  }

  updateButton();

  // when long-press is detected, shut down display to signal power-down
  if (longPressState.pressed) {
    stopLEDOutput();
  }

  // only when long-press is *released* do the actual power-down (this helps
  // avoid reset pin jitter due to button noise)
  if (longPressState.released) {
    ESP.deepSleep(0);
  }
}

void setup() {
  // button and reference pin
  initButton();

  // test pattern
  render16x16(renderBuffer);
  updateRenderQueue(renderBuffer);

  // start the hardware display loop
  initLEDOutput();

  // proceed with other initialization
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  ambienceYield();

  WiFi.begin(ssid, password);
  ambienceYield();

  unsigned int idleReportCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    ambienceYield();

    delay(1);

    idleReportCounter++;
    if ((idleReportCounter & 0x3ff) == 0) {
      idleReportCounter = 0;
      Serial.print(".");
    }
  }

  Serial.println(" connected");
  ambienceYield();

  UDP.begin(localUDPPort);
  ambienceYield();

  Serial.printf("Now listening at IP %s, UDP port %d\n",
                WiFi.localIP().toString().c_str(), localUDPPort);

  if (!MDNS.begin(mDNSDomain)) {
    ambienceYield();
    Serial.println("Error setting up MDNS responder!");
  } else {
    ambienceYield();
    Serial.println("mDNS responder started");
  }
}

void loop() {
  ambienceYield();

  // receive incoming UDP packet
  const int packetSize = UDP.parsePacket();

  if (packetSize) {
    // reset ambience countdown while we are receiving data
    ambienceResetCountdown();

    const int len = UDP.read(inputBuffer, 256);
    if (len == 256) {
      updateRenderQueue(inputBuffer);
    }
  }
}
