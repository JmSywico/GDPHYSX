#include "Rod.h"

	ParticleContact* Rod::GetContact() {
		float currLen = currentLength();

		if (currLen == length) {
			return nullptr;
		}

		ParticleContact* ret = new ParticleContact();
		ret->particles[0] = particles[0];
		ret->particles[1] = particles[1];

		MyVector dir = particles[1]->Position - particles[0]->Position;
		dir = dir.normalize();

		if (currLen > length)
		{
			ret->contactNormal = dir;
			ret->depth = currLen - length;

		}
		else
		{
			ret->contactNormal = dir * -1;
			ret->depth = length - currLen;
		}

		ret->restitution = restitution;

	}