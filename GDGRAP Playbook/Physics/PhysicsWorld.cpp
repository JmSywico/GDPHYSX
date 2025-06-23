#include "PhysicsWorld.h"

#include "Springs/Chain.h"

void PhysicsWorld::AddParticle(PhysicsParticle* toAdd)
{
	this->Particles.push_back(toAdd);
	forceRegistry.Add(toAdd, &Gravity);
}

void PhysicsWorld::Update(float time)
{
	constexpr float maxStep = 0.01f; // 10 ms per sub-step
	while (time > 0.0f)
	{
		float dt = (time > maxStep) ? maxStep : time;
		UpdateParticleList();
		forceRegistry.UpdateForces(dt);
		for (auto* p : Particles) p->Update(dt);
		GenerateContacts();
		if (!Contacts.empty()) contactResolver.ResolveContacts(Contacts, dt);

		

		time -= dt;
	}
}

void PhysicsWorld::AddContact(PhysicsParticle* p1, PhysicsParticle* p2, float restitution, MyVector contactNormal)
{
	auto toAdd = new ParticleContact();
	toAdd->particles[0] = p1;
	toAdd->particles[1] = p2;

	toAdd->restitution = restitution;
	toAdd->contactNormal = contactNormal;

	Contacts.push_back(toAdd);
}

void PhysicsWorld::UpdateParticleList()
{
	Particles.remove_if([](PhysicsParticle* p) { return p->IsDestroyed(); });
}

void PhysicsWorld::GenerateContacts()
{

	Contacts.clear();
	for (std::list<ParticleLink*>::iterator i = Links.begin();
		i != Links.end(); ++i)
	{
		ParticleContact* contact = (*i)->GetContact();

		if (contact != nullptr)
		{
			Contacts.push_back(contact);
		}
	}
}
