#include <Arduino.h>
#include "Motor.h"

Motor::Motor()
{
	//Setup Channel A
	pinMode(12, OUTPUT); //Initiates Motor Channel A pin

	//Setup Channel B
	pinMode(13, OUTPUT); //Initiates Motor Channel B pin
}

void Motor::drive(int position)
{
	if (direction == none) return;

	int motorSpeed = kp * position + kd * (position - lastError);
	lastError = position;

	moveMotorOnSide(left, normalSpeed - motorSpeed);
	moveMotorOnSide(right, normalSpeed - motorSpeed);
}

void Motor::moveMotorOnSide(Direction side, int speed)
{
	speed = max(min(speed, maxSpeed), 0);
	Serial.print("Motor ");
	Serial.print(side == left ? "left " : "right ");
	Serial.println(speed);
	digitalWrite(side == left ? 13 : 12, HIGH);
	analogWrite(side == left ? 11 : 3, speed);
}
