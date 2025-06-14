#pragma once
#include <list>

#include "ForceRegistry.h"
#include "GravityForceGenerator.h"
#include "PhysicsParticle.h"

namespace PhysicsWorld
{
	class PhysicsWorld
	{
	public:
		std::list<PhysicsParticle*> particles;

		ForceRegistry forceRegistry;

		void AddParticle(PhysicsParticle* toAdd);
		void Update(float time);
		const std::list<PhysicsParticle*>& GetParticles() const;

	private:
		void UpdateParticleList();
		//GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0, -9.8f, 0)); //0, -9.8f, 0
		GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0, -280.0f, 0)); //0, -9.8f, 0
	};
}
