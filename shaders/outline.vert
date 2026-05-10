#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineThickness;

out vec3 FragPos;

void main()
{
    //Expand vertex along its normal in world space
    vec3 expandedPos = aPos + aNormal * outlineThickness;
    //factor out world Pos
    vec4 worldPos = model * vec4(expandedPos, 1.0);
    FragPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}