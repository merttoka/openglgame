#pragma once

#include "Vector3D.h"
//#include "Header.h"

const double PI_ON_180 = 0.0174532925199432957;

class Car
{
public:
	Car(float top_speed, Vector3D position)
		:TOP_SPEED(top_speed), pos(position)
	{
		speed = 0.0f;
		angle = 0.0f;
		stopped = true;
		collided = false;
		sumSpeed = 0;
		NEGATIVE_TOP_SPEED = -TOP_SPEED/4;
	}

	void UpdateSumSpeed()
	{
		sumSpeed += speed;
	}

	void ResetCar()
	{
		speed = 0.0f;
		angle = 0.0f;
		stopped = true;
		collided = false;
		pos = Vector3D(0,0,0);
	}

	void Simulate(const float& dt)
	{
		if(!stopped)
		{
			pos.x += speed * (float)sin(angle * PI_ON_180) * dt;
			pos.z += speed * (float)cos(angle * PI_ON_180) * dt;
		}
	}

	Vector3D NextMove(const float& dt)
	{
		Vector3D temp = pos;
		if(!stopped)
		{
			temp.x += speed * (float)sin(angle * PI_ON_180) * dt;
			temp.z += speed * (float)cos(angle * PI_ON_180) * dt;
		}
		return temp;
	}

	void Forward(const float n)
	{
		if(speed < TOP_SPEED)
			speed += n;
	}
	void Backward(const float n)
	{
		if(speed > 0)
			speed -= n;
		else
			NEGATIVE_TOP_SPEED < speed ? speed -= 1.0f: speed = speed;
	}
	void Right(const int turn_angle)
	{
		if(!stopped)
		{
			speed -= .07f;
			if(speed < 0)
				angle -= turn_angle;
			else
				angle += turn_angle;
		}
	}
	void Left(const int turn_angle)
	{
		if(!stopped)
		{
			speed -= .07f;
			if(speed < 0)
				angle += turn_angle;
			else
				angle -= turn_angle;
		}
	}

	void CheckStopped()
	{
		// Whether it is stopped or not
		if(speed < 0.3f && speed > -0.3f)
			stopped = true;
		else
			stopped = false;
	}

	void ApplyFriction()
	{
		// Friction
		if(!stopped && speed > 0)
			speed -= 0.4f;
		else if(!stopped && speed < 0)
			speed += 0.4f;
	}

	const float TOP_SPEED;
	float NEGATIVE_TOP_SPEED;
	float speed;
	float angle;
	bool stopped;
	bool collided;
	Vector3D pos;
	double sumSpeed;
};