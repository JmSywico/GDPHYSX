/*
 * Bungee Force Generator
 *
 * This class makes a particle behave like it's attached to a real bungee cord.
 * The particle moves freely until it stretches the bungee past its rest length.
 * When stretched, the bungee pulls the particle back toward the anchor with a force
 * that gets stronger the more it is stretched. No force is applied if the bungee is slack.
 * This creates a springy, bouncing motion just like a real bungee cord.
 *
 * - No force if the particle is within the rest length (bungee is slack).
 * - Force pulls back only when stretched, and is stronger the more it stretches.
 * - Different from a chain, which just stops the particle at a max length.
 */

#include "Bungee.h"

Bungee::Bungee(const MyVector& anchor, float springConstant, float restLength)
    : anchor(anchor), springConstant(springConstant), restLength(restLength)
{
}

void Bungee::UpdateForce(PhysicsParticle* particle, float /*time*/)
{
    // Calculate vector from anchor to particle
    MyVector force = particle->Position - anchor;
    float length = force.magnitude();

    // Only apply force if stretched beyond rest length
    if (length <= restLength) return;

    // Calculate magnitude of the force
    float extension = length - restLength;
    float magnitude = -springConstant * extension;

    // Normalize direction and scale by magnitude
    force = force.normalize() * magnitude;

    // Apply the force to the particle
    particle->AddForce(force);
}
