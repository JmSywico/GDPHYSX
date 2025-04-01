#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float camera_mod_x = 0.0f;
float camera_mod_y = 0.0f;

float x_mod = 0.0f;
float y_mod = 0.0f;
float z_mod = 3.0f;

float scale_x = 0.02;
float scale_y = 0.02;
float scale_z = 0.02;


float theta_mod_x = 0.0f;
float theta_mod_y = 0.0f;
float theta_mod_z = 0.0f;

float axis_x = 0.0f;
float axis_y = 1.0f;
float axis_z = 0.0f;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float windowWidth = 1280;
    float windowHeight = 720;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "GDGRAP MP", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // Initialize Glad
    gladLoadGL();

    // Set the framebuffer size callback
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    stbi_set_flip_vertically_on_load(true);

    int img_width, img_height, color_channels; // color channels ranges from 3 - 4 (RGB - RGBA)
    // 3 == RGB JPGS !tranparency
    // 4 == RGBA PNGS transparency

    unsigned char* tex_bytes = stbi_load(
        "3D/ayaya.png",
        &img_width,
        &img_height,
        &color_channels,
        0
    );

    glfwSetKeyCallback(window, Key_Callback);
    // Vertex Shader

    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    std::string path = "3D/Car.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes; // positions, texture data, and etc.

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
    {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    std::vector<GLfloat> fullVertexData;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
    {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        // (XYZ coordinates)
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);

        // (Normal XYZ)
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);

        // (UV)
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);
    }
    GLfloat vertices[]{
        0.f, 0.5f, 0.f,
        -0.5f, 0.0f, 0.f,
        0.5f, 0.f, 0.f
    };

    GLuint indices[]{
        0, 1, 2
    };

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, // RGBA if 4 channels and RGB if 3 channels
        img_width,
        img_height,
        0, // border but if 0 it means no border
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    // ID of VAO & VBO & EBO
    GLuint VAO, VBO, EBO, VBO_UV;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &VBO_UV);
    // glGenBuffers(1, &EBO);

    // current VAO = null
    glBindVertexArray(VAO);
    // current VAO = VAO

    // Current VBO = null
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Current VBO = VBO
    // current VAO.VBO.append(VBO)

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * fullVertexData.size(), fullVertexData.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // whole vector

    GLintptr normalPtr = 3 * sizeof(float);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)normalPtr); // offset point for normal so start at index 3

    GLintptr uvPtr = 6 * sizeof(float);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)uvPtr); // offset point for UV so start at index 6

    // enables attrib index 0
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glm::mat4 identity_matrix = glm::mat4(1.0f);

    // glm::mat4 projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    glm::vec3 lightPos = glm::vec3(-10, 3, 5);
    glm::vec3 lightColor = glm::vec3(1, 1, 1); // white
    glm::vec3 lightColor1 = glm::vec3(1, 1, 1); // for testing ambient red
    glm::vec3 lightColor2 = glm::vec3(0, 1, 0); // for testing specular green
    float ambientStr = 0.1f; // must not have high values
    glm::vec3 ambientColor = lightColor1;
    float specStr = 5.0f;
    float specPhong = 16;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get the current window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
        // (FOV, Aspect Ratio, zNear, zFar)

        // Position Matrix of Camera
        glm::vec3 cameraPos = glm::vec3(0.0f + camera_mod_x, 0.0f, 1.5f); // Move camera closer to the model
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        // Orientation
        glm::vec3 worldUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // Pointing upwards
        glm::vec3 cameraCenter = glm::vec3(0.0f + camera_mod_x, 0.0f + camera_mod_y, 0.0f); // Center of the model

        // Forward
        glm::vec3 F = cameraCenter - cameraPos;
        F = glm::normalize(F);

        // R = F x WorldUp
        glm::vec3 R = glm::cross(F, worldUp);
        // U = R x F
        glm::vec3 U = glm::cross(R, F);

        glm::mat4 cameraOrientation = glm::mat4(1.0f);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, worldUp);

        // Render main model
        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(0.0f, -0.5f, 0.0f)); // Place model a bit below the view

        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.1f, 0.1f, 0.1f)); // Increase the scale of the model

        // Rotate the model to look forward
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))); // Rotate around Y-axis

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

        unsigned int alphaLoc = glGetUniformLocation(shaderProg, "alpha");
        glUniform1f(alphaLoc, 1.0f); // Full opacity for the main model

        glBindTexture(GL_TEXTURE_2D, texture);
        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glUniform1i(tex0Address, 0);

        GLuint lightAddress = glGetUniformLocation(shaderProg, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));
        GLuint lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

        GLuint ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);
        GLuint ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));

        GLuint cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));
        GLuint specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(specStrAddress, specStr);
        GLuint specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        GLuint lightColor2Address = glGetUniformLocation(shaderProg, "lightColor2"); // assigning color 2
        glUniform3fv(lightColor2Address, 1, glm::value_ptr(lightColor2));

        glUseProgram(shaderProg);
        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

        // Render left model
        transformation_matrix = glm::translate(identity_matrix, glm::vec3(-0.5f, -0.5f, 0.0f)); // Place model to the left

        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.1f, 0.1f, 0.1f)); // Increase the scale of the model

        // Rotate the model to look forward
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))); // Rotate around Y-axis

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
        glUniform1f(alphaLoc, 0.5f); // Set alpha to 0.5 for the left model
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

        // Render right model
        transformation_matrix = glm::translate(identity_matrix, glm::vec3(0.5f, -0.5f, 0.0f)); // Place model to the right

        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.1f, 0.1f, 0.1f)); // Increase the scale of the model

        // Rotate the model to look forward
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))); // Rotate around Y-axis

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
        glUniform1f(alphaLoc, 0.5f); // Set alpha to 0.5 for the right model
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

        glEnd();

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

