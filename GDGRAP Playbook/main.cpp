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

#include "Model.h"
#include "RenderParticle.h"
#include "Physics/DragForceGenerator.h"
#include "Physics/PhysicsParticle.h"
#include "Physics/PhysicsWorld.h"

// Engine-level particle class
class EngineParticle {
public:
    MyVector Position;
    MyVector Velocity;
    float Lifespan; // in seconds

    EngineParticle(const MyVector& pos, const MyVector& vel, float lifespan)
        : Position(pos), Velocity(vel), Lifespan(lifespan) {}
    virtual ~EngineParticle() = default;
};

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);

// Camera settings
auto cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
auto cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Camera and control state
float cameraYaw = 0.0f;   // Horizontal angle (Y axis)
float cameraPitch = 0.0f; // Vertical angle (X axis)
float cameraRadius = 1200.0f; // Increased radius for large scene
glm::vec3 cameraTarget(0.0f, -400.0f, 0.0f); // Center of action
bool isPerspective = false;
bool isPaused = false;

// Key state for smooth movement
bool keyW = false, keyA = false, keyS = false, keyD = false;

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
	std::list<std::unique_ptr<RenderParticle>> renderParticles;
	std::list<std::unique_ptr<PhysicsParticle>> physicsParticles;

	/*
	* ===========================================================
	* ========================= Drag ============================
	* ===========================================================
	*/
	DragForceGenerator drag = DragForceGenerator(0.001f, 0.0001f);
	
	// Particle spawn timing
	constexpr float spawnInterval = 0.5f; // seconds
	float timeSinceLastSpawn = 0.0f;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
	std::uniform_real_distribution<float> scaleDist(2.0f, 10.0f);
	std::uniform_real_distribution<float> lifespanDist(1.0f, 10.0f);
	std::uniform_real_distribution<float> vxDist(-80.0f, 80.0f);
	std::uniform_real_distribution<float> vzDist(-40.0f, 40.0f); 

	/*
	* ===========================================================
	* ===================== Main Program ========================
	* ===========================================================
	*/
	while (!glfwWindowShouldClose(window))
	{
		curr_time = clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
		float deltaTime = static_cast<float>(dur.count()) / 1e9f; // seconds

		if (!isPaused)
		{
			prev_time = curr_time;
			curr_ns += dur;
			timeSinceLastSpawn += deltaTime;

			// Only spawn particles if not paused
			if (timeSinceLastSpawn >= spawnInterval)
			{
				timeSinceLastSpawn = 0.0f;

				const int particlesPerSpawn = 10; // Number of particles to spawn at once
				for (int i = 0; i < particlesPerSpawn; ++i)
				{
					float vx = vxDist(gen);
					float vy = 150.0f + static_cast<float>(rand() % 80); // 150 to 230
					float vz = vzDist(gen); // add Z spread
					float scale = scaleDist(gen); // random radius/scale
					float lifespan = lifespanDist(gen); // random lifespan
					EngineParticle ep(MyVector(0.0f, -700.0f, 0.0f), MyVector(vx, vy, vz), lifespan);

					auto p = std::make_unique<PhysicsParticle>();
					p->Position = ep.Position;
					p->Velocity = ep.Velocity;
					p->mass = 1.0f;
					p->damping = 0.9f; // Set damping

					pWorld.AddParticle(p.get());
					pWorld.forceRegistry.Add(p.get(), &drag);

					float r = colorDist(gen);
					float g = colorDist(gen);
					float b = colorDist(gen);

					auto rp = std::make_unique<RenderParticle>(p.get(), &model, MyVector(r, g, b));

					renderParticles.emplace_back(std::move(rp));
					physicsParticles.emplace_back(std::move(p));
				}
			}

			// Only update physics if not paused
			if (curr_ns >= timestep)
			{
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
				curr_ns -= curr_ns;
				pWorld.Update(static_cast<float>(ms.count()) / 100.0f);
			}
		}
		else
		{
			// When paused, update prev_time so that time delta is zero when unpausing
			prev_time = curr_time;
		}

		// --- Camera rotation and position update: always runs ---
		const float yawSpeed = 1.5f;   // radians/sec
		const float pitchSpeed = 1.0f; // radians/sec

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

		// --- Rendering (unchanged) ---
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection;
		if (isPerspective)
			projection = glm::perspective(glm::radians(45.0f), 1.0f, 10.0f, 3000.0f);
		else
			projection = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, 0.1f, 3000.0f);

		glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

		for (const auto& rp : renderParticles)
		{
			PhysicsParticle* particle = rp->particle;
			glm::vec3 updatedPos(particle->Position.x, particle->Position.y, particle->Position.z);
			glm::mat4 modelMat = glm::translate(identity_matrix, updatedPos);
			modelMat = glm::rotate(modelMat, glm::radians(thetha), glm::vec3(axis_x, axis_y, axis_z));
			modelMat = glm::scale(modelMat, glm::vec3(scale_x, scale_y, scale_z));

			glm::mat4 mvp = projection * view * modelMat;
			rp->Draw(shaderProgram, mvp);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
