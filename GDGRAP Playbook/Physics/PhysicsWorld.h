#pragma once
#include <list>
#include "PhysicsParticle.h"
#include "../ParticleLink.h"

#include "ForceRegistry.h"
#include "GravityForceGenerator.h"
#include "ContactResolver.h"

class PhysicsWorld
{
public:
	ForceRegistry forceRegistry;

	std::list<PhysicsParticle*> Particles;
	std::list<ParticleLink*> Links;

	void AddParticle(PhysicsParticle* toAdd);
	void Update(float time);

	std::list<ParticleContact*> Contacts;

	void AddContact(PhysicsParticle* p1, PhysicsParticle* p2, float restitution, MyVector contactNormal);

private:
	void UpdateParticleList();
	GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0, -9.8f, 0)); //0, -9.8f, 0

	ContactResolver contactResolver = ContactResolver(100); // Max iterations, tolerance

protected:
	void GenerateContacts();
};
