#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;  
  
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	vec4 t1 = texture(texture1, TexCoord);
	vec4 t2 = texture(texture2, TexCoord);
    FragColor = mix(t1, t2, t2.a);
}