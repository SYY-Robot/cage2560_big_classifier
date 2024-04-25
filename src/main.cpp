#include <Arduino.h>
#include "Pixetto.h"
#include "Servo.h"
#include "Adafruit_NeoPixel.h"

#define rxPin 19
#define txPin 18

Pixetto ss(rxPin, txPin);
Servo Servo_1;
Servo Servo_2;
Servo Servo_3;

const unsigned short servo1_pin = 2;
const unsigned short servo2_pin = 3;
const unsigned short servo3_pin = 6;

const unsigned short red_angel = 20;
const unsigned short blue_angel = 70;
const unsigned short yellow_angel = 120;
const unsigned short green_angel = 170;

const unsigned short servo3_angel_max = 110;
const unsigned short servo3_angel_min = 60;

short color_id;
short door_state = 1;//50 ->150
bool run_finish = 1;
long time, servo3_time;
short servo3_angel;



//Adafruit_NeoPixel strip9 = Adafruit_NeoPixel(3, 9, NEO_GRB + NEO_KHZ800);
//void colorled(int number, String c);


void setup() {
  Serial.begin(9600);
  ss.begin();
  ss.enableFunc(Pixetto::FUNC_COLOR_DETECTION);
  delay(10);
  Serial.println("begin");

  Servo_1.attach(servo1_pin);
  Servo_2.attach(servo2_pin);
  Servo_3.attach(servo3_pin);
  servo3_angel = servo3_angel_max;

  Servo_1.write(20);
  Servo_2.write(50);
  Servo_3.write(servo3_angel);
  servo3_time = 0;

  //strip9.begin();
  //strip9.show();
}

void loop() {
  if (millis() - servo3_time >= 3000) {
    if (servo3_angel == servo3_angel_min) {
      servo3_angel = servo3_angel_max;
      Servo_3.write(servo3_angel);
      servo3_time = millis();
    }
    else if (servo3_angel == servo3_angel_max) {
      servo3_angel = servo3_angel_min;
      Servo_3.write(servo3_angel);
      servo3_time = millis();
    }
  }
  Serial.println("in loop");
  if (ss.isDetected()) {
    Serial.println("see someone");
    if (ss.getFuncID() == Pixetto::FUNC_COLOR_DETECTION) {
      Serial.println("color");
      switch (ss.getTypeID())
      {
      case Pixetto::COLOR_RED:
        Serial.println("red");
        if (run_finish) {
          color_id = Pixetto::COLOR_RED;
          run_finish = 0;
        }
        break;

      case Pixetto::COLOR_YELLOW:
        Serial.println("yellow");
        if (run_finish) {
          color_id = Pixetto::COLOR_YELLOW;
          run_finish = 0;
        }
        break;

      case Pixetto::COLOR_BLUE:
        Serial.println("blue");
        if (run_finish) {
          color_id = Pixetto::COLOR_BLUE;
          run_finish = 0;
        }
        break;

      case Pixetto::COLOR_GREEN:
        Serial.println("green");
        if (run_finish) {
          color_id = Pixetto::COLOR_GREEN;
          run_finish = 0;
        }
        break;

      default:
        break;
      }
    }
  }

  if (run_finish == 0) {
    switch (color_id)
    {
    case Pixetto::COLOR_RED:
      //Serial.println("to red");
      //colorled(1, "#ff0000");
      //colorled(2, "#ff0000");
      //colorled(3, "#ff0000");
      Servo_1.write(red_angel);
      color_id = 0;
      break;

    case Pixetto::COLOR_BLUE:
      //Serial.println("to blue");
      //colorled(1, "#3366ff");
      //colorled(2, "#3366ff");
      //colorled(3, "#3366ff");
      Servo_1.write(blue_angel);
      color_id = 0;
      break;

    case Pixetto::COLOR_GREEN:
      //Serial.println("green");
      //colorled(1, "#33ff33");
      //colorled(2, "#33ff33");
      //colorled(3, "#33ff33");
      Servo_1.write(green_angel);
      color_id = 0;
      break;

    case Pixetto::COLOR_YELLOW:
      //Serial.println("yellow");
      //colorled(1, "#ffff00");
      //colorled(2, "#ffff00");
      //colorled(3, "#ffff00");
      Servo_1.write(yellow_angel);
      color_id = 0;
      break;

    default:
      switch (door_state) {
      case 1:
        //Serial.println("open door");
        time = millis();
        Servo_2.write(50);
        door_state = 2;
        break;
      case 2:
        if (millis() - time >= 1500) {
          //Serial.println("close door");
          Servo_2.write(150);
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
  }
}

/*void colorled(int number, String c)
{
  long rgb_number = strtol(&c[1], NULL, 16);
  int r = rgb_number >> 16;
  int g = rgb_number >> 8 & 0xFF;
  int b = rgb_number & 0xFF;
  if (number <= 0) number = 0;
  else  number--;
  strip9.setPixelColor(number, strip9.Color(r, g, b));
  strip9.show();
}*/