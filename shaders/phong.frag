#version 330 core
out vec4 FragColor;

in vec3 FragPos; 
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

//Phong material parameters
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main(){
    vec3 baseColor = useTexture ? texture(texture_diffuse1, TexCoord).rgb :objectColor;

    //ambient
    vec3 ambient = ambientStrength * lightColor * baseColor;

    //Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    //Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflect Dir = reflect(-lightdir, norm);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess);
    vec3 speecular = specularStrength * spec * lightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    
}