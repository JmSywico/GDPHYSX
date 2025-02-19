// Model.h
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

class Model {
public:
    Model(const std::string& path);
    ~Model();
    void Draw(GLuint shaderProgram, const glm::mat4& transformation_matrix);

private:
    std::vector<GLuint> mesh_indices;
    GLuint VAO, VBO, EBO;
    void LoadModel(const std::string& path);
};

