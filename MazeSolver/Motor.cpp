#include "Motor.h"
#include <Arduino.h>

Motor::Motor() {
	//Setup Channel A
	pinMode(12, OUTPUT); //Initiates Motor Channel A pin

	//Setup Channel B
	pinMode(13, OUTPUT); //Initiates Motor Channel B pin
}

void Motor::drive(int position) {
	if (direction == none) return;

	int motorSpeed = kp * position + kd * (position - lastError);
	lastError = position;

	moveLeftMotor(max(min(normalSpeed - motorSpeed, maxSpeed), 0));
	moveRightMotor(max(min(normalSpeed + motorSpeed, maxSpeed), 0));
}

void Motor::moveRightMotor(int speed) {
	//Motor A forward
	digitalWrite(12, HIGH); //Establishes forward direction of Channel A
	analogWrite(3, speed);   //Spins the motor on Channel A
}

void Motor::moveLeftMotor(int speed) {
	//Motor B forward
	digitalWrite(13, HIGH); //Establishes forward direction of Channel B
	analogWrite(11, speed);   //Spins the motor on Channel B
}
