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
#include <string>
#include <limits> // Add this include for std::numeric_limits

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

// Force application state
bool forceApplied = false;
bool applyForceNextFrame = false;

/*
* ===========================================================
* =================== Initialization ========================
* ===========================================================
*/
//Particle containers
PhysicsWorld pWorld;
std::list<RenderParticle*> renderParticles;
std::list<PhysicsParticle*> physicsParticles;
std::list<float> particleScales;

MyVector accumulatedAcceleration(0.f, 0.f, 0.f);

// Newton's Cradle setup
const int NUM_BALLS = 5;
std::vector<PhysicsParticle> cradleBalls;
std::vector<MyVector> cradleAnchors;

/*
* ===========================================================
* ========================= Drag ============================
* ===========================================================
*/
auto drag = DragForceGenerator(0.0f, 0.0f);
auto gravity = GravityForceGenerator(MyVector(0.0f, -9.8f, 0.0f)); // gravity in m/s^2

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
	{
		isPaused = !isPaused;
		applyForceNextFrame = true; // Set flag to apply force on next frame
	}

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

	/*
	* ===========================================================
	* ===================== User Input ==========================
	* ===========================================================
	*/
	float cableLength, particleGap, particleRadius, gravityStrength, applyForce;

	std::cout << "Enter cable length: ";
	std::cin >> cableLength;
	std::cout << "Enter particle gap (center to center): ";
	std::cin >> particleGap;
	std::cout << "Enter particle radius: ";
	std::cin >> particleRadius;
	std::cout << "Enter gravity strength (negative for downward, e.g. -9.8): ";
	std::cin >> gravityStrength;
	std::cout << "Enter force to apply to the leftmost particle: ";
	std::cin >> applyForce;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Use user input for simulation parameters
	const float CABLE_LENGTH = cableLength;
	const float BALL_RADIUS = particleRadius;
	const float PARTICLE_GAP = 2.0f * BALL_RADIUS;

	// Update gravity with user input
	gravity = GravityForceGenerator(MyVector(0.0f, gravityStrength, 0.0f));

	/*
	* ===========================================================
	* ===================== Particles ===========================
	* ===========================================================
	*/

	cradleBalls.resize(NUM_BALLS);
	float totalWidth = (NUM_BALLS - 1) * PARTICLE_GAP;
	float startX = -totalWidth / 2.0f;

	for (int i = 0; i < NUM_BALLS; ++i)
	{
		float xPos = startX + i * PARTICLE_GAP;
		MyVector anchorPosition(xPos, CABLE_LENGTH, 0);

		// Start at the anchor (top of cable)
		cradleBalls[i].Position = anchorPosition;
		cradleBalls[i].mass = 1.0f;
		cradleBalls[i].damping = 1.0f;
		cradleAnchors.push_back(anchorPosition);

		renderParticles.push_back(new RenderParticle(&cradleBalls[i], &model, MyVector(0.7f, 0.7f, 0.7f)));

		pWorld.AddParticle(&cradleBalls[i]);
	}

	/*
	* ===========================================================
	* ===================== Main Program ========================
	* ===========================================================
	*/
	while (!glfwWindowShouldClose(window))
	{
		curr_time = clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
		float deltaTime = static_cast<float>(dur.count()) / 1e9f; // In seconds

		if (!isPaused)
		{
			prev_time = curr_time;
			curr_ns += dur;
			timeSinceLastSpawn += deltaTime;

			// Apply gravity and drag to each ball
			for (int i = 0; i < NUM_BALLS; ++i)
			{
				gravity.UpdateForce(&cradleBalls[i], deltaTime);
				drag.UpdateForce(&cradleBalls[i], deltaTime);
			}

			if (curr_ns >= timestep)
			{
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
				curr_ns -= curr_ns;
				pWorld.Update(static_cast<float>(ms.count()) / 100.0f);

				// Enforce cable length constraint for each ball
				for (size_t i = 0; i < cradleBalls.size(); ++i)
				{
					MyVector& pos = cradleBalls[i].Position;
					const MyVector& anchor = cradleAnchors[i];
					MyVector offset = pos - anchor;
					float dist = offset.Magnitude();
					if (dist > CABLE_LENGTH)
					{
						MyVector direction = offset.normalize();
						pos = anchor + direction * CABLE_LENGTH;
						MyVector& vel = cradleBalls[i].Velocity;
						float velAlongCable = vel.ScalarProduct(direction);
						if (velAlongCable > 0)
							vel -= direction * velAlongCable;
					}
				}

				 // Collision handling with multiple iterations
				const int collisionIterations = 5; // More iterations for better propagation
				for (int iter = 0; iter < collisionIterations; ++iter)
				{
					for (int i = 0; i < NUM_BALLS - 1; ++i)
					{
						float minDist = 2.0f * BALL_RADIUS;
						MyVector delta = cradleBalls[i + 1].Position - cradleBalls[i].Position;
						float dist = delta.Magnitude();
						if (dist < minDist)
						{
							MyVector collisionNormal = delta.normalize();
							float v1 = cradleBalls[i].Velocity.ScalarProduct(collisionNormal);
							float v2 = cradleBalls[i + 1].Velocity.ScalarProduct(collisionNormal);

							float restitution = 0.99f; // Nearly elastic

							// Only transfer if balls are moving towards each other
							if (v1 > v2)
							{
								// Calculate new velocities (1D elastic collision, equal mass)
								float v1After = v2;
								float v2After = v1;

								cradleBalls[i].Velocity += (v1After - v1) * collisionNormal * restitution;
								cradleBalls[i + 1].Velocity += (v2After - v2) * collisionNormal * restitution;
							}

							// Separate the balls so they are not overlapping
							float overlap = minDist - dist;
							cradleBalls[i].Position -= collisionNormal * (overlap * 0.5f);
							cradleBalls[i + 1].Position += collisionNormal * (overlap * 0.5f);
						}
					}
				}
			}
		}
		else
		{
			prev_time = curr_time;
		}

		// Apply force only once when space is pressed
		if (applyForceNextFrame && !forceApplied)
		{
			cradleBalls[0].AddForce(MyVector(applyForce, 0.0f, 0.0f));
			forceApplied = true;
			applyForceNextFrame = false;
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
		int ballIndex = 0;
		for (auto i = renderParticles.begin(); i != renderParticles.end(); ++i)
		{
			float scale = BALL_RADIUS;
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

			// Set MVP for the particle
			glm::mat4 mvp = projection * view * model;
			GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

			(*i)->Draw(shaderProgram, mvp);

			// Set MVP for the line
			glm::mat4 mvpLine = projection * view;
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpLine));

			// Draw the link (line) to the anchor
			if (ballIndex < cradleAnchors.size())
			{
				MyVector anchorPos = cradleAnchors[ballIndex];
				(*i)->DrawLink(
					glm::vec3(particle->Position.x, particle->Position.y, particle->Position.z),
					glm::vec3(anchorPos.x, anchorPos.y, anchorPos.z),
					shaderProgram,
					mvpLine
				);
			}
			ballIndex++;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
