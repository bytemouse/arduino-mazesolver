#ifndef _MOTOR_h
#define _MOTOR_h

enum Direction
{
	left,
	forward,
	right,
	none
};

class Motor
{
private:
	const float normalSpeed = 0.6f;
	const float maxSpeed = 0.9f;
	const float kp = 0.1;
	const float kd = 1;
	float lastError;

	void moveBothMotors(float speedLeft, float speedRight);

public:
	Direction direction = none;

	Motor();

	void drive(float position);
	void moveMotorOnSide(Direction side, float speed);
};

#endif