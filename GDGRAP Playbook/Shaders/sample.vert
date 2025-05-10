#version 330 core

layout(location = 0) in vec3 aPos; // The position variable has attribute position 0

uniform mat4 transform; // Uniform variable for the transform matrix

void main()
{
    // Multiply the position by the transform matrix to get the final position
    gl_Position = transform * vec4(aPos, 1.0);
}
