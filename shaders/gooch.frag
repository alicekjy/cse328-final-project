#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform sampler2D texture_diffuse1;

//Gooch parameters
//Warm blend (0.45..)
uniform float alpha;
//Cool blend (0.45..)
uniform float beta;

void main()
{
    //Base vectors
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);

    // -- NdotL remapped to [0,1] -- 
    //use full [-1,1] shadowed regions
    float NdotL = dot(N,L);
    //remap to 1.0 
    float t = (NdotL +1.0) / 2.0;


    // -- Gooch cool and warm colors--
    //cool and warm offset
    vec3 k_blue = vec3(0.0, 0.0, 0.4);
    vec3 k_yellow = vec3(0.4,0.4,0.0);

    vec3 k_cool = k_blue + alpha * objectColor;
    vec3 k_warm = k_yellow + beta * objectColor;

    //Cool and warm blend
    vec3 gooch = mix(k_cool, k_warm, t);

    //Specular highlight - phong style
    float spec = pow(max(dot(R,V), 0.0), 32.0);
    //white highlight
    vec3 specular = vec3(1.0) * spec;

    vec4 spotTex = texture(texture_diffuse1, TexCoord);
    FragColor = vec4((gooch + specular) * spotTex.rgb, 1.0);
}