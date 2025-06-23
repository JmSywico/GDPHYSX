#include "ParticleContact.h"

#include <iostream>

void ParticleContact::Resolve(float time)
{
	ResolveVelocity(time);
	ResolveInterpenetration(time);
}

float ParticleContact::GetSeparatingSpeed()
{
	MyVector velocity = particles[0]->Velocity;

	if (particles[1]) velocity -= particles[1]->Velocity;
	return velocity.ScalarProduct(contactNormal);
}

void ParticleContact::ResolveVelocity(float time)
{
	float separatingSpeed = GetSeparatingSpeed();

	if (separatingSpeed > 0) return;

	float newSS = -restitution * separatingSpeed;
	float deltaSpeed = newSS - separatingSpeed;

	float totalMass = static_cast<float>(1) / particles[0]->mass;
	if (particles[1]) totalMass += static_cast<float>(1) / particles[1]->mass;

	if (totalMass <= 0) return;

	float impulseMag = deltaSpeed / totalMass;
	MyVector Impulse = contactNormal * impulseMag;

	MyVector v_A = Impulse * (static_cast<float>(1) / particles[0]->mass);
	particles[0]->Velocity = particles[0]->Velocity + v_A;

	if (particles[1])
	{
		MyVector v_B = Impulse * (static_cast<float>(1) / particles[1]->mass);
		particles[1]->Velocity = particles[1]->Velocity - v_B;
	}
}

void ParticleContact::ResolveInterpenetration(float time)
{
	if (depth <= 0) return;

	// Only one particle (anchored chain)
	if (!particles[1])
	{
		// Move the particle back along the contact normal by the penetration depth
		particles[0]->Position += contactNormal * depth;
	}
	else
	{
		// Standard two-particle resolution
		float totalMass = 1.0f / particles[0]->mass + 1.0f / particles[1]->mass;
		if (totalMass <= 0) return;

		float movePerMass = depth / totalMass;
		MyVector move = contactNormal * movePerMass;

		particles[0]->Position += move * (1.0f / particles[0]->mass);
		particles[1]->Position -= move * (1.0f / particles[1]->mass);
	}

	depth = 0;
}
