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

/*
 * ==============================================================
 * =====================Added for PC01============================
 * ==============================================================
 */
int p3_space_presses = 0;

float p3_accel_per_press = 10.0f;

float finishDistance = 1000.0f;

struct ParticleResult
{
	std::string name;
	float finishTime;
	bool finished;
};

std::vector<ParticleResult> results = {
	{"Particle 1", 0.0f, false},
	{"Particle 2", 0.0f, false},
	{"Player", 0.0f, false},
	{"Particle 4", 0.0f, false}
};

auto sim_start_time = std::chrono::high_resolution_clock::now();
/*
 * ==============================================================
 * =====================Added for PC01============================
 * ==============================================================
 */

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

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		p3_space_presses += 50;
	}
}

int main()
{
	if (!glfwInit())
		return -1;

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "PC01 Francis Obina", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSetKeyCallback(window, KeyCallback);

	glViewport(0, 0, 1000, 1000);

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

	PhysicsWorld::PhysicsWorld pWorld;
	std::list<RenderParticle*> renderParticles;

	auto p1 = PhysicsParticle();
	p1.Position = MyVector(-900.0f, 700.0f, 0.0f);
	/*
	p1.Velocity = MyVector(5.0f, 0.0f, 0.0f);
	p1.Acceleration = MyVector(0.0f, 0.0f, 0.0f);
	*/
	p1.mass = 1.0f;
	pWorld.AddParticle(&p1);
	auto rp1 = RenderParticle(&p1, &model, MyVector(1.0f, 0.0f, 0.0f));
	renderParticles.emplace_back(&rp1);

	auto p2 = PhysicsParticle();
	p2.Position = MyVector(-900.0f, 250.0f, 0.0f);
	/*
	p2.Velocity = MyVector(-3.2f, -2.4f, 0.0f); // Toward center
	p2.Acceleration = MyVector(0.0f, 0.0f, 0.0f);
	*/
	p2.mass = 1.0f;
	pWorld.AddParticle(&p2);
	auto rp2 = RenderParticle(&p2, &model, MyVector(0.0f, 1.0f, 0.0f));
	renderParticles.push_back(&rp2);

	auto p3 = PhysicsParticle();
	p3.Position = MyVector(-900.0f, -700.0f, 0.0f);
	/*
	p3.Velocity = MyVector(3.2f, 2.4f, 0.0f); // Toward center
	p3.Acceleration = MyVector(0.0f, 0.0f, 0.0f);
	*/
	p3.mass = 1.0f;
	pWorld.AddParticle(&p3);
	auto rp3 = RenderParticle(&p3, &model, MyVector(0.0f, 0.0f, 1.0f));
	renderParticles.push_back(&rp3);

	auto p4 = PhysicsParticle();
	p4.Position = MyVector(-900.0f, -250.0f, 0.0f);
	/*
	p4.Velocity = MyVector(-3.2f, 2.4f, 0.0f); // Toward center
	p4.Acceleration = MyVector(0.0f, 0.0f, 0.0f);
	*/
	p4.mass = 1.0f;
	pWorld.AddParticle(&p4);
	auto rp4 = RenderParticle(&p4, &model, MyVector(1.0f, 1.0f, 0.0f));
	renderParticles.push_back(&rp4);

	/*
	DragForceGenerator drag = DragForceGenerator(0.01f, 0.001f);
	pWorld.forceRegistry.Add(&p1, &drag);
	pWorld.forceRegistry.Add(&p2, &drag);
	pWorld.forceRegistry.Add(&p3, &drag);
	pWorld.forceRegistry.Add(&p4, &drag);
	*/

	/*
	* ==============================================================
	* =====================Added for PC01============================
	 * ==============================================================
	 */
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> randomAccelerator(20.0f, 30.0f);
	std::uniform_real_distribution<float> factorDist(1.1f, 8.0f);

	float baseAccel1 = randomAccelerator(gen);
	float baseAccel2 = randomAccelerator(gen);
	float baseAccel4 = randomAccelerator(gen);

	// Multiplier for each particle (starts at 1.0, changes after 60%)
	float accelMult1 = 1.0f, accelMult2 = 1.0f, accelMult4 = 1.0f;

	constexpr float trackLength = 1000.0f;
	constexpr float triggerDistance = 0.6f * trackLength;
	float startX1 = p1.Position.x, startX2 = p2.Position.x, startX4 = p4.Position.x;
	bool applied1 = false, applied2 = false, applied4 = false;
	/*
	 * ==============================================================
	 * =====================Added for PC01============================
	 * ==============================================================
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
			pWorld.Update(static_cast<float>(ms.count()) / 1000.0f);

			/*
			 * ==============================================================
			 * =====================Added for PC01============================
			 * ==============================================================
			 */
			p3.ResetForce();

			if (p3_space_presses > 0)
			{
				float total_accel = p3_accel_per_press * p3_space_presses;
				p3.AddForce(MyVector(total_accel * p3.mass, 0.0f, 0.0f));
				p3_space_presses = 0;
			}

			p1.AddForce(MyVector(baseAccel1 * accelMult1 * p1.mass, 0.0f, 0.0f));
			p2.AddForce(MyVector(baseAccel2 * accelMult2 * p2.mass, 0.0f, 0.0f));
			p4.AddForce(MyVector(baseAccel4 * accelMult4 * p4.mass, 0.0f, 0.0f));

			if (!applied1 && std::abs(p1.Position.x - startX1) >= triggerDistance)
			{
				accelMult1 = factorDist(gen);
				applied1 = true;
			}
			if (!applied2 && std::abs(p2.Position.x - startX2) >= triggerDistance)
			{
				accelMult2 = factorDist(gen);
				applied2 = true;
			}
			if (!applied4 && std::abs(p4.Position.x - startX4) >= triggerDistance)
			{
				accelMult4 = factorDist(gen);
				applied4 = true;
			}

			if (!results[0].finished && p1.Position.x >= 1100.0f)
			{
				results[0].finishTime = std::chrono::duration<float>(clock::now() - sim_start_time).count();
				results[0].finished = true;
			}
			if (!results[1].finished && p2.Position.x >= 1100.0f)
			{
				results[1].finishTime = std::chrono::duration<float>(clock::now() - sim_start_time).count();
				results[1].finished = true;
			}
			if (!results[2].finished && p3.Position.x >= 1100.0f)
			{
				results[2].finishTime = std::chrono::duration<float>(clock::now() - sim_start_time).count();
				results[2].finished = true;
			}
			if (!results[3].finished && p4.Position.x >= 1100.0f)
			{
				results[3].finishTime = std::chrono::duration<float>(clock::now() - sim_start_time).count();
				results[3].finished = true;
			}

			if (results[0].finished && results[1].finished && results[2].finished && results[3].finished)
			{
				std::sort(results.begin(), results.end(), [](const ParticleResult& a, const ParticleResult& b)
				{
					return a.finishTime < b.finishTime;
				});

				std::cout << "\n--- Race Results ---\n";
				for (size_t i = 0; i < results.size(); ++i)
				{
					std::cout << (i + 1) << ". " << results[i].name << " - " << results[i].finishTime << " seconds\n";
				}
				std::cout << "--------------------\n";
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}

			/*
			 * ==============================================================
			 * =====================Added for PC01===========================
			 * ==============================================================
			 */
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 0.1f, 100.0f);
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
