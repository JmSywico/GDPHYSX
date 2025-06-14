#include "GravityForceGenerator.h"

void GravityForceGenerator::UpdateForce(PhysicsParticle* particle, float time)
{
	if (particle->mass <= 0) return;

	MyVector force = Gravity * particle->mass;
	particle->AddForce(force);
}
