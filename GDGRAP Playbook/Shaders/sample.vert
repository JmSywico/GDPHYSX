#version 330 core

//Gets the data at Attrib Index 0
//Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos; //Pos
layout(location = 1) in vec3 vertexNormal; //Normal
layout(location = 2) in vec2 aTex; //UV

//uniform float x;
//uniform float y;
//uniform float z;

uniform mat4 transform;

uniform mat4 projection;

out vec3 normCoord;
out vec3 fragPos;
out vec2 texCoord;

//view matrix
uniform mat4 view;

void main()
{
	normCoord = mat3(transpose(inverse(transform))) * vertexNormal;
	fragPos = vec3(transform * vec4(aPos, 1.0));
	gl_Position = projection * view * transform * vec4(aPos, 1.0);

	texCoord = aTex;
}