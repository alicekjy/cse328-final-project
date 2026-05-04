#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform sampler2D toon1dTex;
uniform sampler2D texture_diffuse1;

void main(){
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);

    float NdotL = dot(N,L);
    float u = (NdotL + 1.0) / 2.0;

    //1D - just the light axis no depth
    float toonLight = texture(toon1dTex, vec2(u, 0.5)).r;
    toonLight = max(toonLight, 0.1);

    vec4 spotTex = texture(texture_diffuse1, TexCoord);
    FragColor = vec4(spotTex.rgb * toonLight * 1.4, 1.0);
}