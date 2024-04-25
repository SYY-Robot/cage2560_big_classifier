#include <Arduino.h>
#include <SparkFun_TB6612.h>
#include <Servo.h>

// Define pins for motor M+ M- & PWM
Motor motor0 = Motor(33, 34, 5, 1, -1);   //A
Motor motor1 = Motor(35, 36, 11, 1, -1);  //B
Motor motor2 = Motor(37, 38, 12, 1, -1);  //C
Motor motor3 = Motor(39, 40, 13, 1, -1);  //D
int motor_speed = 150;

// Define pins for servo
unsigned int servo_pin = 2;
int servo_angle = 90;
int servo_catch_ball_angle = 10;
Servo servo1;

// Define pins for buttons
unsigned int arm_x_1 = 3;
unsigned int arm_x_2 = 6;
unsigned int arm_y_1 = 7;
unsigned int arm_y_2 = 45;
unsigned int arm_z_1 = 46;
unsigned int arm_z_2 = A3;

bool arm_x_1_state = HIGH;
bool arm_x_2_state = HIGH;
bool arm_y_1_state = HIGH;
bool arm_y_2_state = HIGH;
bool arm_z_1_state = HIGH;
bool arm_z_2_state = HIGH;

//Define pins for IR
int IR_pin = A4;

// flag
bool have_ball = 0;

// function
void read_digitale_pin_state();
void arm_init();
void raise_arm();

void setup() {
  //Serial.begin(9600);
  servo1.attach(servo_pin);
  servo1.write(servo_angle);
  pinMode(arm_x_1, INPUT_PULLUP);
  pinMode(arm_x_2, INPUT_PULLUP);
  pinMode(arm_y_1, INPUT_PULLUP);
  pinMode(arm_y_2, INPUT_PULLUP);
  pinMode(arm_z_1, INPUT_PULLUP);
  pinMode(arm_z_2, INPUT_PULLUP);
  pinMode(IR_pin, INPUT);
  arm_init();
}

void loop() {
  while (analogRead(IR_pin) > 300)
  {
    /* code */
    //Serial.println("in while");
  }
  delay(5000);
  servo1.write(servo_catch_ball_angle);
  delay(1000);
  raise_arm();
  delay(1000);
  arm_init();

}

void arm_init() {
  motor1.drive(motor_speed); // motor1 up
  motor0.drive(-(motor_speed - 50));  //motor0 left
  while (arm_x_1_state | arm_y_2_state)
  {
    read_digitale_pin_state();
    if (arm_x_1_state == LOW)  motor0.brake();
    if (arm_y_2_state == LOW)  motor1.brake();
  }

  motor2.drive(-(motor_speed - 30)); //motor2 up
  while (arm_z_2_state)
  {
    read_digitale_pin_state();
    if (arm_z_2_state == LOW) motor2.brake();
  }

  motor1.drive(-motor_speed); // motor1 down
  while (arm_y_1_state)
  {
    read_digitale_pin_state();
    if (arm_y_1_state == LOW) motor1.brake();
  }

  motor2.drive(motor_speed - 60); //motor2 down
  while (arm_z_1_state)
  {
    read_digitale_pin_state();
    if (arm_z_1_state == LOW) motor2.brake();
  }
}

void raise_arm() {
  motor2.drive(-(motor_speed - 50)); //motor2 up
  long time = millis();
  while (millis() - time < 200) {
  }
  motor2.brake();

  motor1.drive(motor_speed); // motor1 up
  time = millis();
  while (millis() - time < 700) {
  }
  motor1.brake();

  motor1.drive(motor_speed); // motor1 up
  motor2.drive(motor_speed - 80); // motor2 down
  time = millis();
  while (arm_y_2_state)
  {
    read_digitale_pin_state();
    if (arm_y_2_state == LOW) motor1.brake();
    if (millis() - time >= 2000) motor2.brake();
  }

  motor2.drive(-(motor_speed - 50)); // motor2 down
  while (arm_z_2_state) {
    read_digitale_pin_state();
    if (arm_z_2_state == LOW) motor2.brake();
  }

  motor0.drive(motor_speed - 50);  //motor0 right
  while (arm_x_2_state)
  {
    read_digitale_pin_state();
    if (arm_x_2_state == LOW) {
      motor0.brake();
      servo1.write(90);
    }
  }


}

void read_digitale_pin_state() {
  arm_x_1_state = digitalRead(arm_x_1);
  arm_x_2_state = digitalRead(arm_x_2);
  arm_y_1_state = digitalRead(arm_y_1);
  arm_y_2_state = digitalRead(arm_y_2);
  arm_z_1_state = digitalRead(arm_z_1);
  arm_z_2_state = digitalRead(arm_z_2);
}