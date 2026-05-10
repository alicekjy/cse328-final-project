#version 330 core

out vec4 FragColor;

uniform vec3 viewPos;
uniform float nearDist;
uniform float farDist;

out vec4 FragColor;

void main()
{
    float dist = length(FragPos - viewPos);
    float fade = 1.0 - clamp((dist - nearDist) / (farDist - nearDist), 0.0, 1.0);
    //SOLID black for outline
    FragColor = vec4(0.0, 0.0, 0.0, fade);
}   