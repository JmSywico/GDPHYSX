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

void RenderParticle::DrawLink(const glm::vec3& a, const glm::vec3& b, GLuint shaderProgram, const glm::mat4& mvp)
{
    // Set up line vertices
    GLfloat lineVertices[] = {
        a.x, a.y, a.z,
        b.x, b.y, b.z
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    glUseProgram(shaderProgram);

    // Set color uniform (e.g., red)
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);

    // Set MVP uniform
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glDrawArrays(GL_LINES, 0, 2);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}