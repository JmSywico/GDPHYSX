#version 330 core

uniform sampler2D tex0;

out vec4 FragColor;

in vec3 normCoord;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightColor2;
uniform float ambientStr;
uniform vec3 ambientColor;
uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;
uniform float alpha; // Add alpha uniform

void main()
{				     //R   G   B   A
	//FragColor = vec4(0.53f, 0.81f, 0.80f, 1.0f);
	vec3 ambientCol = ambientColor * ambientStr;
	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	vec3 specColor = spec * specStr * lightColor2;

	FragColor = vec4(specColor + diffuse + ambientCol, alpha) * texture(tex0, texCoord); // Use alpha value
}
