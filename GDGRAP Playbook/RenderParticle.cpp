#include "RenderParticle.h"
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

void RenderParticle::Draw(GLuint shaderProgram, const glm::mat4& transformation_matrix)
{
	if (!particle->IsDestroyed())
	{
		// Set color as a uniform for this draw call
		GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
		glUniform3fv(colorLoc, 1, glm::value_ptr(static_cast<glm::vec3>(Color)));

		RenderObject->Draw(shaderProgram, transformation_matrix);
	}
}

// Add this function somewhere in your file
void RenderParticle::DrawLink(const glm::vec3& a, const glm::vec3& b)
{
	glUseProgram(0); // Use fixed-function pipeline for simplicity
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for links (if using legacy OpenGL)
	glBegin(GL_LINES);
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
	glEnd();
}
