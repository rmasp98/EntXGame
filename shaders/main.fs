#version 330 core
struct Material {
   sampler2D diffuse, specular;
   float shininess;
};

struct Light {
   vec3 pos[200]; 
   vec3 ambient, diffuse, specular;
   float linear, quad;
   int num;
};

in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragUV;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec3 calcLights(int);

void main() {

   vec3 result = vec3(0.0f);
   for (int i=0; i<light.num; i++)
        result += calcLights(i);     
   
   color = vec4(result, 0.0f);  
} 




vec3 calcLights(int i) {

   //Ambient
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, FragUV));

   vec3 norm = normalize(FragNorm);
   vec3 lightDir = normalize(light.pos[i] - FragPos);
   float distance = length(light.pos[i] - FragPos);
   float attenuation = 1.0f / (1.0f + light.linear * distance + light.quad * distance * distance);

   //Diffuse
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, FragUV));

   //Specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);  
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = light.specular * spec * vec3(texture(material.diffuse, FragUV));

   return (ambient + diffuse + specular)*attenuation;
}
