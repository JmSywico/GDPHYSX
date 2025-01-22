#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float x_mod = 0;
float y_mod = 0;
float z_mod = 0;

float x = 0.0f;
float y = 0.0f;
float z = 0.0f;

float scale_x = 1.0f;
float scale_y = 1.0f;
float scale_z = 1.0f;

float thetha = 0.0f;
float axis_x = 0.0f;
float axis_y = 1.0f;
float axis_z = 0.0f;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_W)
		{
			y_mod = 0.0001f;
		}
		else if (key == GLFW_KEY_A)
		{
			x_mod = -0.0001f;
		}
		else if (key == GLFW_KEY_S)
		{
			y_mod = -0.0001f;
		}
		else if (key == GLFW_KEY_D)
		{
			x_mod = 0.0001f;
		}
		else if (key == GLFW_KEY_UP)
		{
			thetha -= 10;
			axis_x = 1.0f;
			axis_y = 0.0f;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			thetha += 10;
			axis_x = 1.0f;
			axis_y = 0.0f;
		}
		else if (key == GLFW_KEY_LEFT)
		{
			thetha -= 10;
			axis_x = 0.0f;
			axis_y = 1.0f;
		}
		else if (key == GLFW_KEY_RIGHT)
		{
			thetha += 10;
			axis_x = 0.0f;
			axis_y = 1.0f;
		}
		else if (key == GLFW_KEY_Q)
		{
			scale_x -= 0.1f;
			scale_y -= 0.1f;
			scale_z -= 0.1f;
		}
		else if (key == GLFW_KEY_E)
		{
			scale_x += 0.1f;
			scale_y += 0.1f;
			scale_z += 0.1f;
		}

	}

	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W || key == GLFW_KEY_S)
		{
			y_mod = 0.0f;
		}
		else if (key == GLFW_KEY_A || key == GLFW_KEY_D)
		{
			x_mod = 0.0f;
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	gladLoadGL();

	glfwSetKeyCallback(window, KeyCallback);

	std::fstream vertSrc("Shaders/sample.vert");
	std::stringstream vertBuff;
	vertBuff << vertSrc.rdbuf();
	std::string vertS = vertBuff.str();
	const char* vert = vertS.c_str();

	std::fstream fragSrc("Shaders/sample.frag");
	std::stringstream fragBuff;
	fragBuff << fragSrc.rdbuf();
	std::string fragS = fragBuff.str();
	const char* frag = fragS.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vert, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &frag, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	std::string path = "3D/bunny.obj";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;

	tinyobj::attrib_t attributes;

	bool success = LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());

	std::vector<GLuint> mesh_indices;
	for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
	{
		mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
	}

	constexpr int numVertices = 4; // Use a constant value for the array size
	GLfloat vertices[numVertices * 3] = {
		-0.5f, 0.5f, 0.f, // Top-left
		0.5f, 0.5f, 0.f, // Top-right
		0.5f, -0.5f, 0.f, // Bottom-right
		-0.5f, -0.5f, 0.f // Bottom-left
	};

	GLuint indices[] = {
		0, 1, 2, // First triangle
		2, 3, 0 // Second triangle
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * attributes.vertices.size(), &attributes.vertices[0],
	             GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	auto identity_matrix = glm::mat4(1.0f);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// Update position based on modifiers
		x += x_mod;
		y += y_mod;

		glm::mat4 transformation_matrix = translate(identity_matrix, glm::vec3(x, y, z));

		transformation_matrix = rotate(transformation_matrix, glm::radians(thetha), glm::vec3(axis_x, axis_y, axis_z));
		transformation_matrix = scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));

		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(transformation_matrix));

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}
