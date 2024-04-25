#include <Arduino.h>
#include "Pixy2.h"
#include "Servo.h"

#define DEBUG_LOG 0

// put function declarations here:
Pixy2 pixy;
Servo Servo_1;
Servo Servo_2;
Servo Servo_3;

// Servo's pin
const unsigned short servo1_pin = 2;
const unsigned short servo2_pin = 3;
const unsigned short servo3_pin = 6;

// The selection of rotation angle for servo motors during classification.
#define RED 1
#define BLUE 2
#define YELLOW 3
#define GREEN 4
const unsigned short red_angel = 20;
const unsigned short blue_angel = 70;
const unsigned short yellow_angel = 120;
const unsigned short green_angel = 170;

// Servo_3 angle max & min value
const unsigned short servo3_angel_max = 110;
const unsigned short servo3_angel_min = 60;

short color_id;
short door_state = 1; //50 -> 150
bool run_finish = true;
long time, servo3_time;
short servo3_angel;

// function
void agitator();
void open_and_close_door();

void setup() {
  // put your setup code here, to run once:
#if DEBUG_LOG
  Serial.begin(9600);
  Serial.println("begin");
#endif

  pixy.init();

  Servo_1.attach(servo1_pin);
  Servo_2.attach(servo2_pin);
  Servo_3.attach(servo3_pin);
  servo3_angel = servo3_angel_max;

  Servo_1.write(20);
  Servo_2.write(50);
  Servo_3.write(servo3_angel);
  servo3_time = 0;
  pixy.setLamp(1,0);
}

void loop() {
  // put your main code here, to run repeatedly:
#if DEBUG_LOG
  //Serial.println("in loop");
#endif
  agitator();

  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks) {
#if DEBUG_LOG
    Serial.println("see something");
#endif
    switch (pixy.ccc.blocks[0].m_signature)
    {
    case RED:
#if DEBUG_LOG
      Serial.println("red");
#endif
      if (run_finish) {
        color_id = RED;
        run_finish = false;
      }
      break;

    case BLUE:
#if DEBUG_LOG
      Serial.println("blue");
#endif
      if (run_finish) {
        color_id = BLUE;
        run_finish = false;
      }
      break;

    case YELLOW:
#if DEBUG_LOG
      Serial.println("yellow");
#endif
      if (run_finish) {
        color_id = YELLOW;
        run_finish = false;
      }
      break;

    case GREEN:
#if DEBUG_LOG
      Serial.println("green");
#endif
      if (run_finish) {
        color_id = GREEN;
        run_finish = false;
      }
      break;

    default:
      break;
    }
  }

  if (run_finish == false) {
    switch (color_id)
    {
    case RED:
#if DEBUG_LOG
      Serial.println("RED");
#endif
      Servo_1.write(red_angel);
      color_id = 0;
      break;

    case BLUE:
#if DEBUG_LOG
      Serial.println("BLUE");
#endif
      Servo_1.write(blue_angel);
      color_id = 0;
      break;

    case YELLOW:
#if DEBUG_LOG
      Serial.println("YELLOW");
#endif
      Servo_1.write(yellow_angel);
      color_id = 0;
      break;

    case GREEN:
#if DEBUG_LOG
      Serial.println("GREEN");
#endif
      Servo_1.write(green_angel);
      color_id = 0;
      break;

    default:
      open_and_close_door();
      break;
    }
  }
}

// put function definitions here:
void agitator() {
  if (millis() - servo3_time >= 3000) {         // Execute once every 3s
    if (servo3_angel == servo3_angel_min) {       // agitator have two angle.
      servo3_angel = servo3_angel_max;            // Use a flag to determine servo3's angle
      Servo_3.write(servo3_angel);
      servo3_time = millis();
    }
    else if (servo3_angel == servo3_angel_max) {
      servo3_angel = servo3_angel_min;
      Servo_3.write(servo3_angel);
      servo3_time = millis();
    }
  }
}

void open_and_close_door() {                       // The door have three state, parameter door_state control it
  switch (door_state)                             // (1) Open door -> door_state == 1
  {                                           // (2) Close door-> door_state == 2
  case 1:                                     // (3) Waiting   -> door_state == 3 , waiting 2.5s
#if DEBUG_LOG
    Serial.println("open door");
#endif
    time = millis();
    Servo_2.write(150);
    door_state = 2;
    break;

  case 2:

    if (millis() - time >= 1500) {
#if DEBUG_LOG
      Serial.println("close door");
#endif
      Servo_2.write(50);
      time = millis();
      door_state = 3;
    }

  default:
    if (millis() - time >= 2500) {
      door_state = 1;
      run_finish = 1;
    }
    break;
  }
}