#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vPos, 1.0);

	vec4 FPos = vec4(model * vec4(vPos, 1.0));
	FragPos = FPos.xyz;

	Normal = mat3(transpose(inverse(model))) * vNormal;
	TexCoord = vTexCoord;
}