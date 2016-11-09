/*
   A device that controls a neopixel LED using sensor values from 4 flex sensors
   sent via UDP from another device.

   /dev/cu.usbserial-A50285BI
*/
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;

IPAddress ipLocal(192, 168, 1, 22);
IPAddress ipTarget(192, 168, 1, 23);
IPAddress gateway(192, 168, 1, 9); //same as Access Point IP address
IPAddress subnet(255, 255, 255, 0);

unsigned int portLocal = 4210;  // local port to listen on
unsigned int portTarget = 7778;

char incomingPacket[255];
char outgoingPacket[255];
char replyPacket[] = "OK";

#define NEOPIXELPIN 2 //data pin
#define NUMPIXELS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.config(ipLocal, gateway, subnet);
  WiFi.begin("Roving Spectres", "spanglei");

  Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED) { //connecting is 6, connected is 3
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("connected");

  Udp.begin(portLocal);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), portLocal);

  strip.begin();
}

void loop() {

  udp_listen();

  update_neopixels();
}

void udp_listen() {

  int packetSize = Udp.parsePacket();

  if (packetSize) {

    //    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());

    int len = Udp.read(incomingPacket, 255);

    if (len > 0) {

      incomingPacket[len] = 0;

      //      Serial.print("rcvd: "); Serial.println(incomingPacket);

      // send back a reply, to the IP address and port we got the packet from
      //    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      //    Udp.write(replyPacket);
      //    Udp.endPacket();
    }
  }
}

void update_neopixels() {

  String myString = String(incomingPacket);

  String myValues[4]; int index = 0;

  int strLen = myString.length();

  for (int i = 0; i < strLen; i++) {

    char myChar = myString.charAt(i);

    if ( myChar < 58 && myChar > 47 ) {
      myValues[index] += myChar;
    } else {
      index++;
    }
  }

  //  Serial.print("[0]: "); Serial.println(myValues[0]);

  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color( myValues[0].toInt(), myValues[1].toInt(), myValues[2].toInt() ) );
  }

  strip.setBrightness( 255 - myValues[3].toInt() );
  strip.show();
}

