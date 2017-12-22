#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec3 worldPos;
out vec4 color;
  
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 worldView;
uniform mat4 worldViewProjection;

void main()
{
	worldPos = (world * vec4(aPos, 1.0f)).xyz;
    gl_Position = projection * view * vec4(worldPos, 1.0f);
	color = aColor;
} 