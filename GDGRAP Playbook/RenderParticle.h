#pragma once
#include "Model.h"
#include "Physics/PhysicsParticle.h"

class RenderParticle
{
public:
	PhysicsParticle* particle;
	Model* RenderObject;
	MyVector Color;

	RenderParticle(PhysicsParticle* p, Model* r, MyVector c)
		: particle(p), RenderObject(r), Color(c)
	{
	}

	void Draw(GLuint shaderProgram, const glm::mat4& transformation_matrix);
	void DrawLink(const glm::vec3& a, const glm::vec3& b);

};
