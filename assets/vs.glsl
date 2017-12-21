#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aColor;

out vec2 TexCoord;
out vec3 Normal;
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
    TexCoord = aTexCoord;
	Normal = aNormal;//mat3(transpose(inverse(worldView))) * aNormal;
	color = aColor;
} 