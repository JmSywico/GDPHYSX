#pragma once

#include "MyVector.h"
#include "PhysicsParticle.h"

class ForceGenerator
{
public:
	virtual void UpdateForce(PhysicsParticle* p, float time)
	{
		p->AddForce(MyVector(0, 0, 0));
	}
};
