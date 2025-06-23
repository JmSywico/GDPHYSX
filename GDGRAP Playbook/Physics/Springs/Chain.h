#pragma once
#include "../PhysicsParticle.h"
#include "../ParticleContact.h"
#include "../MyVector.h"
#include "../../ParticleLink.h"

// This is a chain link because it only prevents the particle from moving farther than the maximum length from the anchor point, like a real chain. It does not resist compression (slack).

class Chain : public ParticleLink{
public:
    PhysicsParticle* particle;      // The particle attached to the chain
    MyVector anchor;           // The fixed point in space
    float maxLength;           // The maximum length of the chain
    float restitution;         // Bounciness

    Chain(PhysicsParticle* particle, const MyVector& anchor, float maxLength, float restitution);

    ParticleContact* GetContact();
    };
