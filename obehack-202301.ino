// IKEA Obegraensad sketch 2023-01

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "src/effects.h";
#include "src/led.h";

const char *ssid = "********";
const char *password = "********";
const char *mDNSDomain = "esp8266";

WiFiUDP UDP;
unsigned int localUDPPort = 7007; // local port to listen on

unsigned char inputBuffer[256]; // pixel buffer as received over UDP

#define AMBIENCE_WAIT_MS 5000

// simple 16x16 render buffer
unsigned char renderBuffer[256];

unsigned long lastReceivedFrameTime = 0;

void setup() {
  // test pattern
  render16x16(renderBuffer);
  updateRenderQueue(renderBuffer);

  lastReceivedFrameTime = millis();

  // start the hardware display loop
  initLEDOutput();

  // proceed with other initialization
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  UDP.begin(localUDPPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n",
                WiFi.localIP().toString().c_str(), localUDPPort);

  if (!MDNS.begin(mDNSDomain)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
}

void loop() {
  const unsigned long currentTime = millis();

  // receive incoming UDP packet
  const int packetSize = UDP.parsePacket();

  if (packetSize) {
    // reset ambience countdown while we are receiving data
    lastReceivedFrameTime = currentTime;

    const int len = UDP.read(inputBuffer, 256);
    if (len == 256) {
      updateRenderQueue(inputBuffer);
    }
  }

  if (currentTime - lastReceivedFrameTime > AMBIENCE_WAIT_MS) {
    render16x16(renderBuffer);
    updateRenderQueue(renderBuffer);
  }
}
