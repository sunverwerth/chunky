#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 worldPos;
in vec4 color;

out vec4 FragColor;  
  
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 fogColor;
uniform float fade;

void main()
{
	// Vector4(0.8f, 0.8f, 1.0f, 1.0f), 1.0);


	vec4 t1 = texture(texture1, TexCoord);

	vec3 diff = clamp(dot(Normal, vec3(0.5,0.7,0.5)), 0, 1) * lightColor;
	vec3 amb = vec3(0.4,0.4,0.4);

	float fog = max(fade, pow(clamp(gl_FragCoord.z * 50 - 49, 0, 1), 16));
    FragColor = mix(t1 * color * vec4(diff + amb, 1), vec4(fogColor,1), fog);
}