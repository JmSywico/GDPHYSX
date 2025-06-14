#ifndef P6PARTICLE_DEF
#define P6PARTICLE_DEF

#include "PhysicsParticle.h"

#endif

#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"
#endif

#include "list"

class ForceRegistry
{
public:
	void Add(PhysicsParticle* particle, ForceGenerator* generator);
	void Remove(PhysicsParticle* particle, ForceGenerator* generator);
	void Clear();
	void UpdateForces(float time);

protected:
	struct ParticleForceRegistry
	{
		PhysicsParticle* particle;
		ForceGenerator* generator;
	};

	std::list<ParticleForceRegistry> Registry;
};
