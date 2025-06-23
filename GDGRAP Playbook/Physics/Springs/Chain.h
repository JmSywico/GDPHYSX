#pragma once
#include "../PhysicsParticle.h"
#include "../ParticleContact.h"
#include "../MyVector.h"
#include "../../ParticleLink.h"

class Chain : public ParticleLink
{
public:
	PhysicsParticle* particle; // The particle attached to the chain
	MyVector anchor; // The fixed point in space
	float maxLength; // The maximum length of the chain
	float restitution; // Bounciness

	Chain(PhysicsParticle* particle, const MyVector& anchor, float maxLength, float restitution);

	ParticleContact* GetContact() override;
};
