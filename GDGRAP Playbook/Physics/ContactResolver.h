#pragma once
#ifndef P6CONTACT_DEF
#define P6CONTACT_DEF

#include "ParticleContact.h"
#endif
#include <list>

	class ContactResolver
	{
	public:
		unsigned int max_iteration;
		ContactResolver(unsigned int max_iterations)
			: max_iteration(max_iterations), current_iteration(0) {}
		void ResolveContacts(std::list<ParticleContact*> contacts, float time);

	protected:
		unsigned int current_iteration;
	};