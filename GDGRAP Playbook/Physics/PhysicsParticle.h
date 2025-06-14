#pragma once
#include <ctime>

#include "MyVector.h"

class PhysicsParticle
{
public:
	float mass = 0;
	MyVector Position;
	MyVector Velocity;
	MyVector Acceleration;

	float damping = 1.0f; //Approximate drag 0.9f

protected:
	void UpdatePosition(float time);
	void UpdateVelocity(float time);

	bool isDestroyed = false;
	MyVector accumulatedForce = MyVector(0, 0, 0);

public:
	void Update(float time);
	void Destroy();
	bool IsDestroyed() const { return isDestroyed; }

	void AddForce(MyVector force);
	void ResetForce();
};
