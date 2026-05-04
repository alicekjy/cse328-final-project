#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 ligthPos;
uniform vec3 viewPos;
uniform sampler2D xtoonTex;

//tune to the scene
//D = 1 full detail, D = 0 fully abstracted
uniform float nearDist; 
uniform float farDist;

void main(){
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);

    //U axis - NdotL remapped from [-1, 1] to [0,1]
    float NdotL = dot(N,L);
    float u = (NdotL + 1.0) / 2.0;

    //V axis - D from distance, close = 1, far = 0
    float dist = length(viewPos - FragPos);
    float D = 1.0 - clamp((dist - nearDist) / (farDist - nearDist), 0.0, 1.0);

    //Sample 2D toon texture
    vec4 toonColor = texture(xtoonTex, vec2(u,D));

    FragColor = toonColor;
}