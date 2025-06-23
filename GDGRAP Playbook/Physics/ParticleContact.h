#ifndef PHYSICS_PARTICLE_DEF
#define PHYSICS_PARTICLE_DEF
#include "MyVector.h"
#include "PhysicsParticle.h"


class ParticleContact
{
public:
	PhysicsParticle* particles[2];
	float restitution;
	MyVector contactNormal;
	void Resolve(float time);

	float GetSeparatingSpeed();

	float depth;

protected:
	void ResolveVelocity(float time);

	void ResolveInterpenetration(float time);
};

#endif
