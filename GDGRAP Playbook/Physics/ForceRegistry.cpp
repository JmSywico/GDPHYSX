#include "ForceRegistry.h"

void ForceRegistry::Add(PhysicsParticle* particle, ForceGenerator* generator)
{
	ParticleForceRegistry toAdd;

	toAdd.particle = particle;
	toAdd.generator = generator;
	Registry.push_back(toAdd);
}

void ForceRegistry::Remove(PhysicsParticle* particle, ForceGenerator* generator)
{
	Registry.remove_if(
		[particle, generator](ParticleForceRegistry reg)
		{
			return reg.particle == particle && reg.generator == generator;
		}
	);
}

void ForceRegistry::Clear()
{
	Registry.clear();
}

void ForceRegistry::UpdateForces(float time)
{
	for (std::list<ParticleForceRegistry>::iterator i = Registry.begin(); i != Registry.end(); i++)
	{
		i->generator->UpdateForce(i->particle, time);
	}
}
