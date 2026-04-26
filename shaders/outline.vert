#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineThickness;

void main()
{
    //Expand vertex along its normal in world space
    vec3 expandedPos = aPos + aNormal * outlineThickness;
    gl_Position = projection * view * model * vec4(expandedPos, 1.0);
}