#pragma once
#include "../ForceGenerator.h"
#include "../PhysicsParticle.h"
#include "../MyVector.h"

// Bungee force generator: applies a restoring force only when stretched beyond rest length.
class Bungee : public ForceGenerator
{
private:
    MyVector anchor;         // The fixed anchor point
    float springConstant;    // Stiffness of the bungee
    float restLength;        // Natural length of the bungee

public:
    Bungee(const MyVector& anchor, float springConstant, float restLength);

    // Applies the bungee force to the given particle
    void UpdateForce(PhysicsParticle* particle, float time) override;
};