#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>
#include <chrono>
#include <iostream>
#include <random>

#include "Model.h"
#include "RenderParticle.h"
#include "Physics/DragForceGenerator.h"
#include "Physics/PhysicsParticle.h"
#include "Physics/PhysicsWorld.h"

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);

// Camera settings
auto cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
auto cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Transformation settings
float scale_x = 30.0f, scale_y = 30.0f, scale_z = 30.0f;
float thetha = 0.0f, axis_x = 0.0f, axis_y = 1.0f, axis_z = 0.0f;

// Model positions
std::vector<glm::vec3> modelPositions;

/*
* ===========================================================
* ====================== Key Input ==========================
* ===========================================================
*/
void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main()
{
	/*
	* ===========================================================
	* ======================== Setup ============================
	* ===========================================================
	*/
	if (!glfwInit())
		return -1;

	GLFWwindow* window = glfwCreateWindow(800, 800, "Gocomma Engine", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSetKeyCallback(window, KeyCallback);

	glViewport(0, 0, 800, 800);

	// Load shaders
	std::ifstream vertSrc("Shaders/sample.vert"), fragSrc("Shaders/sample.frag");
	std::string vertCode((std::istreambuf_iterator<char>(vertSrc)), std::istreambuf_iterator<char>());
	std::string fragCode((std::istreambuf_iterator<char>(fragSrc)), std::istreambuf_iterator<char>());
	const char* vert = vertCode.c_str();
	const char* frag = fragCode.c_str();

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

	Model model("3D/sphere.obj");
	auto identity_matrix = glm::mat4(1.0f);
	modelPositions.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));

	using clock = std::chrono::high_resolution_clock;
	auto curr_time = clock::now();
	auto prev_time = curr_time;
	std::chrono::nanoseconds curr_ns(0);

	/*
	* ===========================================================
	* ==================== Physics World ========================
	* ===========================================================
	*/
	PhysicsWorld::PhysicsWorld pWorld;
	std::list<RenderParticle*> renderParticles;

	/*
	* ===========================================================
	* ======================= Particles =========================
	* ===========================================================
	*/
	PhysicsParticle p1 = PhysicsParticle();

	// Set initial position at the bottom (e.g., y = -900.0f)
	p1.Position = MyVector(0.0f, -900.0f, 0.0f);

	// Set initial upward velocity (tune the value for desired height)
	p1.Velocity = MyVector(0.0f, 135.0f, 0.0f);

	p1.mass = 1.0f;
	pWorld.AddParticle(&p1);
	RenderParticle rp1 = RenderParticle(&p1, &model, MyVector(1.0f, 0.0f, 0.0f));
	renderParticles.emplace_back(&rp1);

	/*
	* ===========================================================
	* ========================= Drag ============================
	* ===========================================================
	*/
	
	DragForceGenerator drag = DragForceGenerator(0.001f, 0.0001f);
	pWorld.forceRegistry.Add(&p1, &drag);

	/*DragForceGenerator drag = DragForceGenerator(0.00001f, 0.00001f);
	pWorld.forceRegistry.Add(&p1, &drag);*/

	/*
	* ===========================================================
	* ===================== Main Program ========================
	* ===========================================================
	*/
	while (!glfwWindowShouldClose(window))
	{
		curr_time = clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
		prev_time = curr_time;

		curr_ns += dur;

		if (curr_ns >= timestep)
		{
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
			curr_ns -= curr_ns;
			pWorld.Update(static_cast<float>(ms.count()) / 100.0f);

		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		for (auto i = renderParticles.begin(); i != renderParticles.end(); ++i)
		{
			PhysicsParticle* particle = (*i)->particle;
			glm::vec3 updatedPos(particle->Position.x, particle->Position.y, particle->Position.z);
			glm::mat4 model = glm::translate(identity_matrix, updatedPos);
			model = glm::rotate(model, glm::radians(thetha), glm::vec3(axis_x, axis_y, axis_z));
			model = glm::scale(model, glm::vec3(scale_x, scale_y, scale_z));

			glm::mat4 mvp = projection * view * model;
			(*i)->Draw(shaderProgram, mvp);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
