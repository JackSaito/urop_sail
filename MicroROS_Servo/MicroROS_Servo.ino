#include <micro_ros_arduino.h>
#include <Servo.h> // Include the Servo library

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <sensor_msgs/msg/joy.h>

// Micro-ROS handles
rcl_subscription_t subscriber;
sensor_msgs__msg__Joy joy_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

// Servo setup
Servo myServo;
#define SERVO_PIN 9  // Change this to the pin where your servo is connected

// Error handling macros
#define LED_PIN 13
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop() {
  while (1) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

// Callback function for /joy topic
void subscription_callback(const void *msgin) {
  const sensor_msgs__msg__Joy *msg = (const sensor_msgs__msg__Joy *)msgin;

  if (msg->axes.size > 0) {
    // Assume the first axis controls the servo (e.g., left-right joystick)
    float joystick_value = msg->axes.data[0];

    // Map joystick value (-1.0 to 1.0) to servo angle (0 to 180 degrees)
    int servo_position = map(joystick_value * 100, -100, 100, 0, 180);

    // Write position to the servo
    myServo.write(servo_position);

    // Debug output
    Serial.print("Joystick value: ");
    Serial.print(joystick_value);
    Serial.print(" -> Servo position: ");
    Serial.println(servo_position);
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(2000);

  // Set up LED for error indication
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Attach the servo
  myServo.attach(SERVO_PIN);

  // Initialize micro-ROS transport
  set_microros_transports();

  // Initialize micro-ROS allocator and support
  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // Create a ROS 2 node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // Initialize subscriber to /joy topic
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Joy),
    "/joy"));

  // Allocate memory for the /joy message
  joy_msg.axes.data = (float *)malloc(8 * sizeof(float)); // Adjust size based on joystick
  joy_msg.axes.capacity = 8;
  joy_msg.axes.size = 0;

  joy_msg.buttons.data = (int32_t *)malloc(12 * sizeof(int32_t)); // Adjust size based on joystick
  joy_msg.buttons.capacity = 12;
  joy_msg.buttons.size = 0;

  // Initialize the executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &joy_msg, &subscription_callback, ON_NEW_DATA));
}

void loop() {
  // Spin the executor to process incoming messages
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

  // Small delay for stability
  delay(10);
}
