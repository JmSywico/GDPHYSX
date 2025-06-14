#include "PhysicsWorld.h"

void PhysicsWorld::PhysicsWorld::AddParticle(PhysicsParticle* toAdd)
{
	particles.push_back(toAdd);
	forceRegistry.Add(toAdd, &Gravity);
}

void PhysicsWorld::PhysicsWorld::Update(float time)
{
	UpdateParticleList();
	forceRegistry.UpdateForces(time);

	for (auto p = particles.begin();
	     p != particles.end();
	     ++p)
	{
		(*p)->Update(time);
	}
}

const std::list<PhysicsParticle*>& PhysicsWorld::PhysicsWorld::GetParticles() const
{
	return particles;
}

void PhysicsWorld::PhysicsWorld::UpdateParticleList()
{
	particles.remove_if([](PhysicsParticle* p) { return p->IsDestroyed(); });
}
