/*
 * Chain Constraint
 *
 * This class makes a particle act like it's attached to a chain.
 * The chain lets the particle move freely until it reaches the max length.
 * If the particle tries to go farther, it gets pushed back as if hitting the end of a real chain.
 * The restitution value controls how much it bounces when this happens.
 *
 * - No effect if the chain is slack (within max length).
 * - If stretched too far, the particle is pushed back and may bounce.
 */

#include "Chain.h"

#include <iostream>

Chain::Chain(PhysicsParticle* particle, const MyVector& anchor, float maxLength, float restitution)
    : particle(particle), anchor(anchor), maxLength(maxLength), restitution(restitution) {
}

ParticleContact* Chain::GetContact() {
    // Calculate the vector from anchor to particle
    MyVector toParticle = particle->Position - anchor;
    float length = toParticle.Magnitude();

    // If within max length, no contact needed
    if (length <= maxLength) return nullptr;

    // Otherwise, create a contact
    ParticleContact* contact = new ParticleContact();
    contact->particles[0] = particle;
    contact->particles[1] = nullptr; // Anchor is fixed

    // The contact normal is from particle to anchor (direction to push particle back)
    contact->contactNormal = toParticle.normalize() * -1.0f;

    // Penetration depth is how much the chain is overstretched
    contact->depth = length - maxLength;

    // Set restitution for bounce effect
    contact->restitution = restitution;

    return contact;
}
