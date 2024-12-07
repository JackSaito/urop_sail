#include <ros.h>
#include <ros/time.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Int8.h>
#include <sensor_msgs/Joy.h>
#include "A4988.h"

// using a 200-step motor (most common)
#define MOTOR_STEPS 200
// configure the pins connected
#define STEP 9
#define DIR 8
#define rpm 30
#define micro_step 8

ros::NodeHandle nh;

A4988 stepper(MOTOR_STEPS, DIR, STEP);

void messageCb(const sensor_msgs::Joy& joy){
  stepper.begin(rpm, micro_step);
  if (joy.buttons[6] == 1) {
    // Tell motor to rotate * degrees.
    stepper.rotate(360 * 5);
  }
  else if (joy.buttons[7] == 1) {
    // Tell motor to rotate * degrees.
    stepper.rotate(-360 * 5);
  }
}

ros::Subscriber<sensor_msgs::Joy> sub("joy", &messageCb);

void setup(){
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

  nh.initNode();
  nh.subscribe(sub);
}

void loop(){
  nh.spinOnce();
  delay(1);
}
