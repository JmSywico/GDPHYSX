#pragma once
#include "../ForceGenerator.h"

	class ParticleSpring : public ForceGenerator {

	private:
		PhysicsParticle* otherParticle;
		float springConstant;
		float restLength;

	public:
		ParticleSpring(PhysicsParticle* otherParticle, float springConstant, float restLength) : otherParticle(otherParticle), springConstant(springConstant), restLength(restLength) {}
		void UpdateForce(PhysicsParticle* particle, float time) override;
	};	
