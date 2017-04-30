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
	const unsigned int normalSpeed = 130;
	const unsigned int maxSpeed = 230;
	const float kp = 0.1;
	const float kd = 1;
	int lastError;

protected:

public:
	Motor();

	void drive(int position);
	void moveRightMotor(int speed);
	void moveLeftMotor(int speed);

	Direction direction = none;
};

#endif