#version 330 core

in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;  
  
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord) * Color;
}