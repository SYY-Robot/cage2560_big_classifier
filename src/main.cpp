#include <Arduino.h>
#include <HUSKYLENS.h>
#include "Servo.h"
#include <Wire.h>

#define DEBUG 0

HUSKYLENS huskylens;
Servo Servo_1;  // 下方分料用舵機
Servo Servo_2;  // 中間檔球用舵機
Servo Servo_3;  // 上方理球用舵機

enum ColorID {
  COLOR_RED = 1,
  COLOR_YELLOW,
  COLOR_BLUE,
  COLOR_GREEN
};

enum DoorState {
  OPEN_DOOR = 1,
  CLOSE_DOOR,
  WAITING
};

const unsigned short SERVO_PIN[3] = { 2,3,6 };


const unsigned short ANGLE_RED = 20;
const unsigned short ANGLE_BLUE = 70;
const unsigned short ANGLE_YELLOW = 120;
const unsigned short ANGLE_GREEN = 170;

const unsigned short SERVO3_ANGLE_MAX = 110;
const unsigned short SERVO3_ANGLE_MIN = 60;

short colorID = 0;
short doorState = DoorState::OPEN_DOOR;//50 -> 180
bool runFinished = true;
long currentTime, servo3Time;
short servo3Angel;


void handleHUSKYLENSResults();
void handleColorDetection();
void handleDoorControl();

void setup() {
  Serial.begin(9600);
  Wire.begin();

  while (!huskylens.begin(Wire))
  {
    Serial.println(F("Begin failed!"));
    delay(100);
  }
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
  delay(10);
#if DEBUG
  Serial.println("begin");
#endif

  Servo_1.attach(SERVO_PIN[0]);
  Servo_2.attach(SERVO_PIN[1]);
  Servo_3.attach(SERVO_PIN[2]);
  servo3Angel = SERVO3_ANGLE_MAX;

  Servo_1.write(ANGLE_RED);
  Servo_2.write(50);
  Servo_3.write(servo3Angel);
  servo3Time = 0;
}

void loop() {
  if (millis() - servo3Time >= 3000) {
    if (servo3Angel == SERVO3_ANGLE_MIN) {
      servo3Angel = SERVO3_ANGLE_MAX;
      Servo_3.write(servo3Angel);
      servo3Time = millis();
    }
    else if (servo3Angel == SERVO3_ANGLE_MAX) {
      servo3Angel = SERVO3_ANGLE_MIN;
      Servo_3.write(servo3Angel);
      servo3Time = millis();
    }
  }
#if DEBUG
  Serial.println("in loop");
#endif

  if (!huskylens.request()) Serial.println(F("Fail to request objects from HUSKYLENS!"));
  else if (!huskylens.isLearned()) { Serial.println(F("Object not learned!")); }
  else if (!huskylens.available()) Serial.println(F("Object disappeared!"));
  else {
    handleHUSKYLENSResults();
  }
  if(!runFinished){
    handleColorDetection();
  }
}

void handleHUSKYLENSResults() {
#if DEBUG
  Serial.println("see someone");
#endif

  int MaxArea = 0;  // 紀錄最大面積數值
  short MaxAreaColorid = 0;  // 紀錄最大面積物體的ID

  for (int i = 0;i < huskylens.count(); i++) {  // 在huskylens中出現的已學習物體的數量
    HUSKYLENSResult result = huskylens.read();
    int area = result.width * result.height;

    //  紀錄最大面積和對應的顏色ID
    if (area > MaxArea) {
      MaxArea = area;
      MaxAreaColorid = result.ID;
    }
  }

#if DEBUG
  Serial.println(MaxAreaColorid);
  Serial.println(MaxArea);
#endif

  //  設置目標顏色ID
  if (runFinished) {
    colorID = MaxAreaColorid;
    runFinished = false;
  }
}
void handleColorDetection() {
  switch (colorID)
  {
  case ColorID::COLOR_RED:
#if DEBUG
    Serial.println("red");
#endif
    Servo_1.write(ANGLE_RED);
    colorID = 0;
    break;

  case ColorID::COLOR_YELLOW:
#if DEBUG
    Serial.println("yellow");
#endif
    Servo_1.write(ANGLE_YELLOW);
    colorID = 0;
    break;

  case ColorID::COLOR_BLUE:
#if DEBUG
    Serial.println("blue");
#endif
    Servo_1.write(ANGLE_BLUE);
    colorID = 0;
    break;

  case ColorID::COLOR_GREEN:
#if DEBUG
    Serial.println("green");
#endif
    Servo_1.write(ANGLE_GREEN);
    colorID = 0;
    break;


  default:
    handleDoorControl();
    break;
  }
}

void handleDoorControl() {
  switch (doorState)
  {
  case DoorState::OPEN_DOOR:
#if DEBUG
    Serial.println("open door");
#endif
    currentTime = millis();
    Servo_2.write(180);
    doorState = CLOSE_DOOR;
    break;

  case DoorState::CLOSE_DOOR:
    if (millis() - currentTime >= 1500) {
#if DEBUG
      Serial.println("close door");
#endif
      Servo_2.write(50);
      currentTime = millis();
      doorState = DoorState::WAITING;
    }
    break;

  case DoorState::WAITING:
    if (millis() - currentTime >= 2500) {
      doorState = DoorState::OPEN_DOOR;
      runFinished = true;
    }
    break;
  }
}