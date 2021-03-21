#version 400 core
layout (location = 0) in vec3 aPos;

out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    fragPos = aPos;  
    gl_Position =  projection * view * vec4(fragPos, 1.0);
}