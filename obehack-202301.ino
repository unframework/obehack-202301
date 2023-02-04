// IKEA Obegraensad sketch 2023-01

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

const char *ssid = "********";
const char *password = "********";
const char *mDNSDomain = "esp8266";

WiFiUDP UDP;
unsigned int localUDPPort = 7007; // local port to listen on

char incomingPacket[256]; // buffer for incoming packets
char replyPacket[] =
    "Hi there! Got the message :-)"; // a reply string to send back

// ESP8266 Feather pinout
#define LEDARRAY_CLA 4
#define LEDARRAY_CLK 5
#define LEDARRAY_DI 2
#define LEDARRAY_EN 16

#define MASK_CLA (1 << LEDARRAY_CLA)
#define MASK_CLK (1 << LEDARRAY_CLK)
#define MASK_DI (1 << LEDARRAY_DI)

#define ROWS 16
#define COLS 16

// actual LED layout corresponding to the shift register queue is complex and
// snaking, this is the LUT
const unsigned char positions[ROWS * COLS] = {
    // clang-format off
    0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x4f, 0x4e, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x8f, 0x8e, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x88, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xaf, 0xae, 0xad, 0xac, 0xab, 0xaa, 0xa9, 0xa8, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xcf, 0xce, 0xcd, 0xcc, 0xcb, 0xca, 0xc9, 0xc8, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0xc2, 0xc1, 0xc0, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    // clang-format on
};

unsigned int pwmFrame = 0;

// per brightness level, how many frames are on out of a 64-frame full duty
// cycle (note that the progression is very non-linear)
unsigned int pwmDutyCounts[] = {0, 1, 3, 7, 12, 24, 40, 64};

void setup() {
  // LED panel
  pinMode(LEDARRAY_CLA, OUTPUT);
  pinMode(LEDARRAY_CLK, OUTPUT);
  pinMode(LEDARRAY_DI, OUTPUT);
  pinMode(LEDARRAY_EN, OUTPUT);

  // initial state
  digitalWrite(LEDARRAY_CLA, LOW);
  digitalWrite(LEDARRAY_CLK, LOW);
  digitalWrite(LEDARRAY_DI, LOW);

  // turn on display
  digitalWrite(LEDARRAY_EN, LOW);

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
  // receive incoming UDP packet
  const int packetSize = UDP.parsePacket();

  if (packetSize) {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize,
                  UDP.remoteIP().toString().c_str(), UDP.remotePort());

    const int len = UDP.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }

    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    // UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    // UDP.write(replyPacket);
    // UDP.endPacket();
  }

  return; // @todo this

  const int frameDuty = pwmFrame & 63;

  for (int idx = 0; idx < ROWS * COLS; idx++) {
    const int pos = positions[idx];
    const int col = pos & 15;
    const int row = pos >> 4;

    const int on = (col + row) & 1;
    const int value = on ? (col >> 1) : 0;

    const int pwmDuty = pwmDutyCounts[value];

    // toggle on the frame if its PWM duty cycle is on
    if ((frameDuty) < pwmDuty) {
      GPOS = MASK_DI; // fast direct write set
    } else {
      GPOC = MASK_DI; // fast direct write clear
    }

    GPOS = MASK_CLK; // fast direct write set
    GPOC = MASK_CLK; // fast direct write clear

    // legacy slow write
    // digitalWrite(LEDARRAY_DI, ((frame & 15) == 0 && on) ? HIGH : LOW);
    // digitalWrite(LEDARRAY_CLK, HIGH);
    // digitalWrite(LEDARRAY_CLK, LOW);
  }

  digitalWrite(LEDARRAY_CLA, HIGH);
  digitalWrite(LEDARRAY_CLA, LOW);

  pwmFrame += 1;
}
