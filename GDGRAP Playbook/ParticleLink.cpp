#include "ParticleLink.h"

#include "Physics/MyVector.h"


	float ParticleLink::currentLength() 
	{
		MyVector ret = particles[0]->Position - particles[1]->Position;
		return ret.magnitude();
	}
