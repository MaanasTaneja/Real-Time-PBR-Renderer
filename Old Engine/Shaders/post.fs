#version 450 core
out vec4 FragColor;


in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;

vec3 acesFilm(const vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d ) + e), 0.0, 1.0);
}

void main()
{
	vec3 out_color= texture(screenTexture, TexCoords).rgb;

	//out_color += out_color * 0.1;
	//out_color = acesFilm(out_color);
    FragColor = vec4(out_color, 1.0f);
}  