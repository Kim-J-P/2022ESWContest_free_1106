#include <Wire.h>
#include <DynamixelShield.h>
//--------------------------------------------------------Dynamixel
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
//--------------------------------------------------------Esp8266
#include <Servo.h>
//--------------------------------------------------------Servo
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
#include <SoftwareSerial.h>
SoftwareSerial soft_serial(7, 8); // DYNAMIXELShield UART RX/TX
#define DEBUG_SERIAL soft_serial
#elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAM_ZERO)
#define DEBUG_SERIAL SerialUSB
#else
#define DEBUG_SERIAL Serial
#endif
//---------------------------------------------------------Dynamixel define
#include "WiFiEsp.h"
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(12, 13); // RX, TX
#endif
//---------------------------------------------------------Esp define
const uint8_t DXL_ID = 1; // y
const uint8_t DXL_ID2 = 2; // y
const uint8_t DXL_ID3 = 3; // x
const uint8_t DXL_ID4 = 4; // x

const float DXL_PROTOCOL_VERSION = 2.0;

DynamixelShield dxl;
DynamixelShield dxl2;
DynamixelShield dxl3;
DynamixelShield dxl4;

double cur_gyro_X;
double cur_gyro_Y;
using namespace ControlTableItem;
//-----------------------------------------------------다이나믹셀 쉴드 변수 끝
const int MPU_ADDR = 0x68; // I2C 통신 주소
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 가속도, 온도, 자이로 Raw Data 저장 변수
double angleAcX, angleAcY, angleAcZ;
double angleGyX, angleGyY, angleGyZ; // GyX, GyY, GyZ 값의 범위 : -32768 ~ +32767 (16비트 정수범위)
double angleFiX, angleFiY, angleFiZ; // 상보 필터 적용한 각도

const double RADIAN_TO_DEGREE =  180 / 3.14159;
const double DEG_PER_SEC = 131;
const double ALPHA = 0.95;

unsigned long now = 0;
unsigned long past = 0;
double dt = 0;

double averAcX, averAcY, averAcZ;
double averGyX, averGyY, averGyZ;
//-------------------------------------------------------자이로 센서 변수 끝
char ssid[] = "KCCI_STC_S";            // your network SSID (name)
char pass[] = "kcci098#";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "10.10.141.233";
// Initialize the Ethernet client object
WiFiEspClient client;
//-------------------------------------------------------Esp 변수 끝
Servo CageDoor;

//-------------------------------------------------------Servo 변수 끝
void caliSensor() {
  double sumAcX = 0 , sumAcY = 0, sumAcZ = 0;
  double sumGyX = 0 , sumGyY = 0, sumGyZ = 0;
  getData();
  for (int i = 0; i < 500; i++) {
    getData();
    sumAcX += AcX;  sumAcY += AcY;  sumAcZ += AcZ;
    sumGyX += GyX;  sumGyY += GyY;  sumGyZ += GyZ;
    delay(10);
  }
  averAcX = sumAcX / 500;  averAcY = sumAcY / 500;  averAcZ = sumAcY / 500;
  averGyX = sumGyX / 500;  averGyY = sumGyY / 500;  averGyZ = sumGyZ / 500;
}

void getDT() {
  now = millis();
  dt = (now - past) / 1000.0;
  past = now;
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);   // AcX 레지스터 위치(주소)를 지칭합니다
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true); // AcX 주소 이후의 14byte의 데이터를 요청
  AcX = Wire.read() << 8 | Wire.read(); //두 개의 나뉘어진 바이트를 하나로 이어 붙여서 각 변수에 저장
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // 슬레이브의 주소값 지정
  Wire.write(0x6B); // MPU6050과 통신을 시작하기 위한 설정
  Wire.write(0);
  Wire.endTransmission(true); //데이터 전송
}

void complementaryCalc() {
  //가속도 센서의 각도 계산
  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
  angleAcZ = atan(sqrt(pow(AcX, 2) + pow(AcY, 2)) / AcZ);

  //자이로 센서의 각도 계산
  angleGyX += ((GyX - averGyX) / DEG_PER_SEC) * dt;
  angleGyY += ((GyY - averGyY) / DEG_PER_SEC) * dt;
  angleGyZ += ((GyZ - averGyZ) / DEG_PER_SEC) * dt;

  angleFiX = (ALPHA * angleGyX) + ((1 - ALPHA) * angleAcX);
  angleFiY = (ALPHA * angleGyY) + ((1 - ALPHA) * angleAcY);
}

//-------------------------------------------------------자이로 관련 함수 끝
void setup() {
  // put your setup code here, to run once:
  CageDoor.attach(7);
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true);
  }
  while ( status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
  }
  if (client.connect(server, 5000)) {
    client.print("[25:PASSWD]");
  }
  
  DEBUG_SERIAL.begin(57600);//다이나믹셀 UART 통신
  dxl.begin(57600);
  dxl2.begin(57600);
  dxl3.begin(57600);
  dxl4.begin(57600);

  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  dxl.ping(DXL_ID); //1번째 다이나믹셀 정보 얻기
  dxl2.ping(DXL_ID2); //2번째 다이나믹셀 정보 얻기
  dxl3.ping(DXL_ID3); //1번째 다이나믹셀 정보 얻기
  dxl4.ping(DXL_ID4); //2번째 다이나믹셀 정보 얻기

  dxl.torqueOff(DXL_ID);
  dxl.setOperatingMode(DXL_ID, OP_EXTENDED_POSITION);
  dxl.torqueOn(DXL_ID);
  dxl2.torqueOff(DXL_ID2);
  dxl2.setOperatingMode(DXL_ID2, OP_EXTENDED_POSITION);
  dxl2.torqueOn(DXL_ID2);
  dxl3.torqueOff(DXL_ID3);
  dxl3.setOperatingMode(DXL_ID3, OP_EXTENDED_POSITION);
  dxl3.torqueOn(DXL_ID3);
  dxl4.torqueOff(DXL_ID4);
  dxl4.setOperatingMode(DXL_ID4, OP_EXTENDED_POSITION);
  dxl4.torqueOn(DXL_ID4);

  initSensor(); // 자이로센서 초기 설정 함수
  caliSensor(); // 가속도 및 자이로 센서의 초기 값을 조정하는 함수.
  past = millis();
  //-------------------------------------------------------dynamixel setup
}

int flag = 0;
char sensing[30] = {0};
int index = 0, read_length = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if (client.available()) {
    while (client.available()) {
      sensing[index++] = client.read();
    }
    sensing[index] = '\0';
    read_length = strlen(sensing);
    
    // if the server's disconnected, stop the client
    if (!client.connected()) {
      client.stop();
    }
    if (read_length == 4) {
      CageDoor.write(90);
      delay(10);
    }
    else if (read_length == 5) {
      CageDoor.write(0);
      delay(10);
    }
    read_length = 0;
    index = 0;
    memset(sensing, 0, sizeof(sensing));
  }
  else {
    getData(); // 가속도, 자이로 Raw data 저장
    getDT(); // loop문 한 사이클당 걸리는 시간 계산

    complementaryCalc();

    dxl.setGoalPosition(DXL_ID, -1 * angleFiY, UNIT_DEGREE);
    dxl.setGoalPosition(DXL_ID2, -1 * angleFiY, UNIT_DEGREE);
    dxl.setGoalPosition(DXL_ID3, angleFiX, UNIT_DEGREE);
    dxl.setGoalPosition(DXL_ID4, -1 * angleFiX, UNIT_DEGREE);
  }
}
