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
#include <memory>
#include <cmath>
#include <string>

#include "Model.h"
#include "RenderParticle.h"
#include "Rod.h"
#include "ParticleLink.h"

#include "Physics/DragForceGenerator.h"
#include "Physics/PhysicsParticle.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/Springs/Bungee.h"
#include "Physics/Springs/Chain.h"
#include "Physics/ParticleContact.h"
#include "Physics/ContactResolver.h"

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);

// Engine-level particle class
class EngineParticle
{
public:
	MyVector Position;
	MyVector Velocity;
	float Lifespan; // in seconds

	EngineParticle(const MyVector& pos, const MyVector& vel, float lifespan)
		: Position(pos), Velocity(vel), Lifespan(lifespan)
	{
	}

	virtual ~EngineParticle() = default;
};

/*
* ===========================================================
* =================== Camera Settings =======================
* ===========================================================
*/
auto cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
auto cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Camera and control state
float cameraYaw = 0.0f; // Horizontal angle (Y axis)
float cameraPitch = 0.0f; // Vertical angle (X axis)
float cameraRadius = 1200.0f; // Increased radius for large scene
glm::vec3 cameraTarget(0.0f, 0, 0.0f); // Center of action
bool isPerspective = false;
bool isPaused = false;

// Key state for smooth movement
bool keyW = false, keyA = false, keyS = false, keyD = false;

// Transformation settings
float thetha = 0.0f, axis_x = 0.0f, axis_y = 1.0f, axis_z = 0.0f;

// Model positions
std::vector<glm::vec3> modelPositions;

/*
* ===========================================================
* ==================== Physics World ========================
* ===========================================================
*/
//Particle containers
PhysicsWorld pWorld;
std::list<RenderParticle*> renderParticles;
std::list<PhysicsParticle*> physicsParticles;
std::list<float> particleScales;

MyVector accumulatedAcceleration(0.f, 0.f, 0.f);
	
// Place anchors and particles for left (bungee) and right (chain)
MyVector bungeeAnchor(-100, 100, 0); // Left side
MyVector chainAnchor(100, 100, 0);   // Right side

PhysicsParticle bungeeParticle;
PhysicsParticle chainParticle;

// Links
Bungee* bungeeForce = nullptr;
Chain* chainLink = nullptr;

/*
* ===========================================================
* ========================= Drag ============================
* ===========================================================
*/
//auto drag = DragForceGenerator(0.001f, 0.0001f);

// Particle spawn timing
constexpr float spawnInterval = 1.0f; // seconds
float timeSinceLastSpawn = 0.0f;

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

	// Projection swap
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		isPerspective = false;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		isPerspective = true;

	// Play/Pause
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		isPaused = !isPaused;

	// WASD for camera rotation (set key state)
	if (key == GLFW_KEY_W) keyW = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_A) keyA = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_S) keyS = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_D) keyD = (action != GLFW_RELEASE);
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

	GLFWwindow* window = glfwCreateWindow(800, 800, "GoComma Engine", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSetKeyCallback(window, KeyCallback);

	glViewport(0, 0, 800, 800);
	glEnable(GL_DEPTH_TEST);

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

	bungeeParticle.Position = bungeeAnchor + MyVector(0, 100, 0);
	bungeeParticle.mass = 1.0f;
	bungeeParticle.damping = 0.98f;
	bungeeParticle.Velocity = MyVector(0, 0, 0);
	pWorld.AddParticle(&bungeeParticle);

	chainParticle.Position = chainAnchor + MyVector(0, -100, 0);
	chainParticle.mass = 1.0f;
	chainParticle.damping = 0.98f;
	chainParticle.Velocity = MyVector(80, 0, 0);
	pWorld.AddParticle(&chainParticle);

	// Create the bungee force generator
	bungeeForce = new Bungee(bungeeAnchor, 40.0f, 400.0f); // example springConstant=40.0f
	pWorld.forceRegistry.Add(&bungeeParticle, bungeeForce);

	// Add links to the world
	chainLink = new Chain(&chainParticle, chainAnchor, 400.0f, 0.5f);
	pWorld.Links.push_back(chainLink);

	// Add to renderParticles
	renderParticles.push_back(new RenderParticle(&bungeeParticle, &model, MyVector(1.0f, 0.0f, 0.0f)));
	renderParticles.push_back(new RenderParticle(&chainParticle, &model, MyVector(1.0f, 1.0f, 0.0f)));

	/*
	* ===========================================================
	* ===================== Main Program ========================
	* ===========================================================
	*/
	while (!glfwWindowShouldClose(window))
	{
		std::cout << "Chain Pos: " << chainParticle.Position.x << ", " << chainParticle.Position.y << ", " << chainParticle.Position.z
			<< " Vel: " << chainParticle.Velocity.x << ", " << chainParticle.Velocity.y << ", " << chainParticle.Velocity.z << std::endl;

		curr_time = clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
		float deltaTime = static_cast<float>(dur.count()) / 1e9f; // In seconds

		if (!isPaused)
		{
			prev_time = curr_time;
			curr_ns += dur;
			timeSinceLastSpawn += deltaTime;


			if (curr_ns >= timestep)
			{
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
				curr_ns -= curr_ns;
				pWorld.Update(static_cast<float>(ms.count()) / 100.0f);
				//contact.Resolve((float)ms.count() / 100.0f);
			}

		}
		else
		{
			prev_time = curr_time;
		}
		constexpr float yawSpeed = 1.5f;
		constexpr float pitchSpeed = 1.0f;

		if (keyA) cameraYaw += yawSpeed * deltaTime;
		if (keyD) cameraYaw -= yawSpeed * deltaTime;
		if (keyW) cameraPitch += pitchSpeed * deltaTime;
		if (keyS) cameraPitch -= pitchSpeed * deltaTime;

		if (cameraPitch > glm::radians(89.0f)) cameraPitch = glm::radians(89.0f);
		if (cameraPitch < glm::radians(-89.0f)) cameraPitch = glm::radians(-89.0f);

		cameraPos.x = cameraTarget.x + cameraRadius * cosf(cameraPitch) * sinf(cameraYaw);
		cameraPos.y = cameraTarget.y + cameraRadius * sinf(cameraPitch);
		cameraPos.z = cameraTarget.z + cameraRadius * cosf(cameraPitch) * cosf(cameraYaw);
		cameraFront = glm::normalize(cameraTarget - cameraPos);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection;
		if (isPerspective)
			projection = glm::perspective(glm::radians(45.0f), 1.0f, 10.0f, 3000.0f);
		else
			projection = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 3000.0f);

		glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

		auto individualScale = particleScales.begin();
		for (auto i = renderParticles.begin(); i != renderParticles.end(); ++i)
		{
			float scale = 20.0f;
			if (individualScale != particleScales.end())
			{
				scale = *individualScale;
				++individualScale;
			}
			PhysicsParticle* particle = (*i)->particle;
			glm::vec3 updatedPos(particle->Position.x, particle->Position.y, particle->Position.z);
			glm::mat4 model = glm::translate(identity_matrix, updatedPos);
			model = glm::rotate(model, glm::radians(thetha), glm::vec3(axis_x, axis_y, axis_z));
			model = glm::scale(model, glm::vec3(scale, scale, scale));

			glm::mat4 mvp = projection * view * model;
			(*i)->Draw(shaderProgram, mvp);

		}

		//// Create a temporary RenderParticle instance to call DrawLink
		//RenderParticle tempRenderParticle(nullptr, nullptr, MyVector(0.0f, 0.0f, 0.0f));

		//// Draw bungee link (bungeeParticle to bungeeAnchor)
		//tempRenderParticle.DrawLink(
		//	glm::vec3(bungeeParticle.Position.x, bungeeParticle.Position.y, bungeeParticle.Position.z),
		//	glm::vec3(bungeeAnchor.x, bungeeAnchor.y, bungeeAnchor.z)
		//);

		//// Draw chain link (chainParticle to chainAnchor)
		//tempRenderParticle.DrawLink(
		//	glm::vec3(chainParticle.Position.x, chainParticle.Position.y, chainParticle.Position.z),
		//	glm::vec3(chainAnchor.x, chainAnchor.y, chainAnchor.z)
		//);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
