#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 worldPos;
in vec4 color;

out vec4 FragColor;  
  
uniform sampler2D texture1;
uniform vec3 lightColor;

void main()
{
	vec4 t1 = texture(texture1, TexCoord);

	vec3 diff = clamp(dot(Normal, vec3(0.5,0.7,0.5)), 0, 1) * lightColor;
	vec3 amb = vec3(0.4,0.4,0.4);

    FragColor = t1 * color * vec4(diff + amb, 1);
}