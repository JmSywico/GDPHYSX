#version 330 core

uniform sampler2D tex0;

out vec4 FragColor;

in vec2 texCoord;

void main()
{				     //R   G   B   A
	FragColor = vec4(0.53f, 0.81f, 0.80f, 1.0f);
	//FragColor = texture(tex0, texCoord);
}