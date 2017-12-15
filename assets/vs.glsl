#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec4 Normal;
  
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 worldView;
uniform mat4 worldViewProjection;

void main()
{
    gl_Position = worldViewProjection * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
    Normal = worldView * vec4(aNormal, 0.0f);
} 