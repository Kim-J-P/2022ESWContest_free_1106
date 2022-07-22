#include <Wire.h>

const int MPU_ADDR = 0x68; // I2C 통신 주소
// GyX, GyY, GyZ 값의 범위 : -32768 ~ +32767 (16비트 정수범위)
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 가속도, 온도, 자이로 Raw Data 저장 변수
double angleAcX, angleAcY, angleAcZ; 
double angleGyX, angleGyY, angleGyZ;
double angleFiX, angleFiY, angleFiZ; // 상보 필터 적용한 각도

const double RADIAN_TO_DEGREE =  180 / 3.14159;
const double DEG_PER_SEC = 131;
const double ALPHA = 0.996;

unsigned long now = 0;
unsigned long past = 0;
double dt = 0;

double averAcX, averAcY, averAcZ;
double averGyX, averGyY, averGyZ;
//-------------------------------------------------------변수 끝
void caliSensor() {
  double sumAcX = 0 , sumAcY = 0, sumAcZ = 0;
  double sumGyX = 0 , sumGyY = 0, sumGyZ = 0;
  getData(); 
  for (int i=0;i<500;i++) {
    getData();
    sumAcX+=AcX;  sumAcY+=AcY;  sumAcZ+=AcZ;
    sumGyX+=GyX;  sumGyY+=GyY;  sumGyZ+=GyZ;
    delay(10);
  }
  averAcX=sumAcX/500;  averAcY=sumAcY/500;  averAcZ=sumAcY/500;
  averGyX=sumGyX/500;  averGyY=sumGyY/500;  averGyZ=sumGyZ/500;
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
  Wire.requestFrom(MPU_ADDR, 14, true);  // AcX 주소 이후의 14byte의 데이터를 요청
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
//-------------------------------------------------------함수 끝
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initSensor(); // 자이로센서 초기 설정 함수
  caliSensor(); // 가속도 및 자이로 센서의 초기 값을 조정하는 함수.
  past = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  getData(); // 가속도, 자이로 Raw data 저장
  getDT(); // loop문 한 사이클당 걸리는 시간 계산

  //가속도 센서의 각도 계산
  angleAcX = atan(AcX / sqrt(pow(AcY,2)+pow(AcZ,2)));
  angleAcX *= RADIAN_TO_DEGREE;
  angleAcY = atan(AcY / sqrt(pow(AcX,2)+pow(AcZ,2)));
  angleAcY *= RADIAN_TO_DEGREE;
  angleAcZ = atan(sqrt(pow(AcX,2)+pow(AcY,2)) / AcZ);

  //자이로 센서의 각도 계산(센서값 보정을 위해 caliSensor에서 구한 값을 빼준다.) 
  angleGyX += ((GyX - averGyX) / DEG_PER_SEC) * dt;  //각속도로 변환
  angleGyY += ((GyY - averGyY) / DEG_PER_SEC) * dt;
  angleGyZ += ((GyZ - averGyZ) / DEG_PER_SEC) * dt;

  angleFiX = (ALPHA * angleGyX) + ((1-ALPHA) * angleAcX);
  angleFiY = (ALPHA * angleGyY) + ((1-ALPHA) * angleAcY);

  Serial.print("Filterd X : ");
  Serial.print(angleFiX);
  Serial.print("\t\t");
  Serial.print("Filterd Y : ");
  Serial.println(angleFiY);
}
