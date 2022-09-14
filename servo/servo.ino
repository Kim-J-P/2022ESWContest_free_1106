#include "WiFiEsp.h"
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
#include <Servo.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(12,13);         //아두이노 메가 rx1,tx1 핀 번호
#endif

char ssid[] = "KCCI_STC_S";            //와이파이 ID
char pass[] = "kcci098#";              //비밀번호

int status = WL_IDLE_STATUS;           // the Wifi radio's status

char server[] = "10.10.141.233";

WiFiEspClient client;
Servo left_Door;
Servo right_Door;

void setup()
{
  left_Door.attach(7);                 //server로부터 1을 받아오면 제어
  right_Door.attach(5);                //server로부터 2를 받아오면 제어
  Serial.begin(9600);                  // initialize serial for debugging
  Serial1.begin(9600);                 // initialize serial for ESP module
  WiFi.init(&Serial1);                 // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  printWifiStatus();

  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 5000)) {
    client.println("25:PASSWD");
    //client.println();
    Serial.println("Connected to server");
  }
}
void loop()
{
  char sensing[5] = {0};
  int index = 0;
  int Cage, lenth = 0, count = 0;

  if (client.available()) {
    while (client.available()) {
      sensing[index++] = client.read();
    }
    sensing[index] = '\0';
    lenth = strlen(sensing);
    Serial.print(sensing);
    count++;
  }

  // if the server's disconnected, stop the client
  delay(500);
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
  if (count == 1) {
    if (sensing[2] == '1' && sensing[1] == 'o') {
      left_Door.write(90);
      delay(10);
    }
    else if (sensing[2] == '1' && sensing[1] == 'x') {
      left_Door.write(0);
      delay(10);
    }
    if(sensing[2] == '2' && sensing[1] == 'o'){
      right_Door.write(90);
      delay(10);
    }
    else if(sensing[2] == '2' && sensing[1] == 'x'){
      right_Door.write(0);
      delay(10);
    }
    count = 0;
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
