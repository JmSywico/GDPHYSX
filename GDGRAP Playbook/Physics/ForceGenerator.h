#ifndef P6PARTICLE_DEF
#define P6PARTICLE_DEF

#include "MyVector.h"
#include "PhysicsParticle.h"

#endif
class ForceGenerator
{
public:
	virtual void UpdateForce(PhysicsParticle* p, float time)
	{
		p->AddForce(MyVector(0, 0, 0));
	}
};
