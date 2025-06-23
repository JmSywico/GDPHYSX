#pragma once
#include "Physics/PhysicsParticle.h"
#include "Physics/ParticleContact.h"

	class ParticleLink {
	public:
		PhysicsParticle* particles[2] = { nullptr, nullptr }; // Initialize particles to nullptr
		virtual ParticleContact* GetContact() { return nullptr; };

	protected:
		float currentLength();
		};
