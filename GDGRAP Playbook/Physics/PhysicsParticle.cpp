#include "PhysicsParticle.h"

#include <cmath>

void PhysicsParticle :: UpdatePosition(float time)
{
	// Update the position of the particle based on its velocity and time
	this->Position = this->Position + (this->Velocity * time) + ((1.0f / 2.0f) * (this->Acceleration * time * time));
}

void PhysicsParticle::UpdateVelocity(float time)
{
	this->Acceleration += accumulatedForce * (1.0f / mass);
	this->Velocity += (this->Acceleration * time);
	this->Velocity *= powf(damping, time); //Apply damping
}

void PhysicsParticle::Update(float time)
{
	UpdatePosition(time);
	UpdateVelocity(time);

	this->ResetForce();
}

void PhysicsParticle::Destroy()
{
	this->isDestroyed = true;
}

void PhysicsParticle::AddForce(MyVector force)
{
	this->accumulatedForce += force;
}

void PhysicsParticle::ResetForce()
{
	this->accumulatedForce = MyVector(0, 0, 0);
	this->Acceleration = MyVector(0, 0, 0);
}