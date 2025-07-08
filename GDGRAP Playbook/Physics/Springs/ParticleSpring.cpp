#include "ParticleSpring.h"

#include <cmath>

void ParticleSpring::UpdateForce(PhysicsParticle* particle, float time)
{
	MyVector pos = particle->Position;

	MyVector force = pos - otherParticle->Position;

	float mag = force.magnitude();

	float springForce = -springConstant * abs(mag - restLength);

	force = force.normalize();

	force = force * springForce;

	particle->AddForce(force);
}
