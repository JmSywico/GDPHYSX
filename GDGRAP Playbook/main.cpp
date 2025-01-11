#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        int numVertices = 8;

        glBegin(GL_POLYGON);
        for (int i = 0; i < numVertices; ++i)
        {
            float angle = 0.394f;
            float yOffset = 0.5f;
            float radius = 0.5f;

            float theta = i * 2.0f * M_PI / numVertices; // Calculate the angle for the current vertex

            // Calculate the x and y coordinates of the vertex
        	float x = cos(theta) * radius;
            float y = sin(theta) * radius + yOffset;

			// 2D rotation matrix
            float rotatedX = x * cos(angle) - y * sin(angle);
            float rotatedY = x * sin(angle) + y * cos(angle);

            glVertex2f(rotatedX, rotatedY);
        }
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}