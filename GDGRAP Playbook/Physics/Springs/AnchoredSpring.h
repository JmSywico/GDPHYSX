#pragma once
#include "../ForceGenerator.h"

class AnchoredSpring : public ForceGenerator
{
	MyVector anchorPoint;
	float springConstant;
	float restLength;

public:
	AnchoredSpring(MyVector pos, float springConstant, float restLength) : anchorPoint(pos),
	                                                                       springConstant(springConstant),
	                                                                       restLength(restLength)
	{
	}

	void UpdateForce(PhysicsParticle* particle, float time) override;
};
