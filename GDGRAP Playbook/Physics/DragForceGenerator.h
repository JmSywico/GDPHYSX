#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"
#endif

class DragForceGenerator : public ForceGenerator
{
	float k1 = 0.74f;
	float k2 = 0.57f;

public:
	DragForceGenerator()
	{
	}

	DragForceGenerator(float _k1, float _k2) : k1(_k1), k2(_k2)
	{
	}

	void UpdateForce(PhysicsParticle* particle, float time) override;
};
