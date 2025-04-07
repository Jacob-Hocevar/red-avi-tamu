#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <IPAddress.h>

unsigned int PORT = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP udp;
byte MAC_ADDRESS[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xD5};
IPAddress REMOTE(192, 168, 1, 174);
IPAddress LOCAL(192, 168, 1, 175);
IPAddress GATEWAY(192, 168, 1, 1);
IPAddress SUBNET(255, 255, 255, 0);

void setup() {
  Ethernet.begin(MAC_ADDRESS, LOCAL, GATEWAY, SUBNET);
  Serial.begin(115200);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERR: Ethernet board disconnected");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("ERR: Ethernet cable disconnected");
  }
  udp.begin(PORT);
}

void loop() {
  while (Ethernet.linkStatus() != LinkON) {
    Serial.println("ERR: Ethernet cable disconnected");
    delay(500);
  }

  if (Serial.available()) {
    udp.beginPacket(REMOTE, PORT);
    while (Serial.available()) {
      udp.write(Serial.read());
    }
    udp.write('\0');
    udp.endPacket();
  }

  int packetSize = udp.parsePacket();
  if (packetSize) {
    udp.read(packetBuffer, packetSize);
    Serial.print(packetBuffer);
  }
}