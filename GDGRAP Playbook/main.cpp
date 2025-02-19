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

float scale_x = 1;
float scale_y = 1;
float scale_z = 1;

float thetha = 0.0f;
float theta_mod_x = 0.0f;
float theta_mod_y = 0.0f;
float theta_mod_z = 0.0f;

float axis_x = 0.0f;
float axis_y = 1.0f;
float axis_z = 0.0f;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
        else if (key == GLFW_KEY_V)
        {
            camera_mod_y += 0.1f;
        }
        else if (key == GLFW_KEY_B)
        {
            camera_mod_y -= 0.1f;
        }
        else if (key == GLFW_KEY_F)
        {
            camera_mod_x += 0.1f;
        }
        else if (key == GLFW_KEY_G)
        {
            camera_mod_x -= 0.1f;
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

    float windowWidth = 600;
    float windowHeight = 600;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "Rafael Ira R. Villanueva", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //Initialize Glad
    gladLoadGL();

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

    int img_width, img_height, color_channels; //color channels ranges from 3 - 4 (RGB - RGBA)
    //3 == RGB JPGS !tranparency
    //4 == RGBA PNGS transparency

    unsigned char* tex_bytes = stbi_load(
        "3D/ayaya.png",
        &img_width,
        &img_height,
        &color_channels,
        0
    );


    glfwSetKeyCallback(window, Key_Callback);
    //Vertex Shader

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


    std::string path = "3D/myCube.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector < tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes; //positions, texture data, and etc.

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

    GLfloat vertices[]
    {
        0.f, 0.5f, 0.f,
        -0.5f, 0.0f, 0.f,
        0.5f, 0.f, 0.f
    };

    GLuint indices[]
    {
        0, 1, 2
    };

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, //RGBA if 4 channels and RGB if 3 channels
        img_width,
        img_height,
        0, //border but if 0 it means no border
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    //ID of VAO & VBO & EBO
    GLuint VAO, VBO, EBO, VBO_UV;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_UV);
    glGenBuffers(1, &EBO);

    //current VAO = null
    glBindVertexArray(VAO);
    //current VAO = VAO

    //Current VBO = null
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Current VBO = VBO
    //current VAO.VBO.append(VBO)

    //Data
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * attributes.vertices.size(), //Size of buffer in bytes
        &attributes.vertices[0], //Array itself
        GL_STATIC_DRAW //Static Objects for moving object need to use GL_DYNAMIC_DRAW
    );

    //Describes how to read data
    glVertexAttribPointer(
        //0 Position Data
        0, //Attrib Index-Index of VBO
        3, // X , Y , Z
        GL_FLOAT, //Array of GL floats
        GL_FALSE, //Is normalized?
        3 * sizeof(GLfloat), //size of components in bytes
        (void*)0 //stride value
    );

    float bytes = (sizeof(GLfloat)) * (sizeof(UV) / sizeof(UV[0]));
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER, bytes, &UV[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);


    //current VBO = VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //current VBO = EBO
    //current VAO.VBO.append(EBO)

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    //enables attrib index 0
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glm::mat4 identity_matrix = glm::mat4(1.0f);

    //glm::mat4 projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f + z_mod), windowHeight / windowWidth, 0.1f, 100.0f);
        //(FOV, Aspect Ratio, zNear, zFar)    

        //Position Matrix of Camera
        glm::vec3 cameraPos = glm::vec3(0.0f + camera_mod_x, 0.0f, 5.0f); //Eye of Camera
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        //Orientation
        glm::vec3 worldUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); //Pointing upwards
        glm::vec3 cameraCenter = glm::vec3(0.0f + camera_mod_x, 3.0f + camera_mod_y, 0.0f); //A bit on top of the model

        //Forward
        glm::vec3 F = cameraCenter - cameraPos;
        F = glm::normalize(F);

        //R = F x WorldUp
        glm::vec3 R = glm::cross(F, worldUp);
        //U = R x F
        glm::vec3 U = glm::cross(R, F);

        glm::mat4 cameraOrientation = glm::mat4(1.0f);

        //Row 1 = R
        //cameraOrientation[0][0] = R.x;
        //cameraOrientation[1][0] = R.y;
        //cameraOrientation[2][0] = R.z;
        //
  //      //Row 2 = U
        //cameraOrientation[0][1] = U.x;
        //cameraOrientation[1][1] = U.y;
        //cameraOrientation[2][1] = U.z;
        //
  //      //Row 3 = -F
        //cameraOrientation[0][2] = -F.x;
        //cameraOrientation[1][2] = -F.y;
  //      cameraOrientation[2][2] = -F.z;
        //End of Orientation Matrix ^^^

        //glm::mat4 viewMatrix = (cameraOrientation * cameraPosMatrix);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, worldUp);

        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x_mod, y_mod, z_mod));

        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_x), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_y), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_mod_z), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

        glBindTexture(GL_TEXTURE_2D, texture);
        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glUniform1i(tex0Address, 0);


        glUseProgram(shaderProg);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);


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
