#include "RenderParticle.h"
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

void RenderParticle::Draw(GLuint shaderProgram, const glm::mat4& transformation_matrix)
{
    if (!particle->IsDestroyed())
    {
        // Set color as a uniform for this draw call
        GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
        glUniform3fv(colorLoc, 1, glm::value_ptr((glm::vec3)Color));

        RenderObject->Draw(shaderProgram, transformation_matrix);
    }
}
