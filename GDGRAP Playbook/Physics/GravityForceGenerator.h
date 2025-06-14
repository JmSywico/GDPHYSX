#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"
#endif

class GravityForceGenerator : public ForceGenerator
{
	//MyVector Gravity = MyVector(0, -9.8f, 0);
	MyVector Gravity = MyVector(0, 0, 0);

public:
	GravityForceGenerator(const MyVector gravity) : Gravity(gravity)
	{
	}

	void UpdateForce(PhysicsParticle* particle, float time) override;
};
