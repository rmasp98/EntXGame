#version 330 core

struct Material {
   sampler2D diffuse, specular;
   float shininess;
};

struct Light {
   vec3 pos;
   vec3 ambient, diffuse, specular;
   float linear, quad;
};

in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragUV;

out vec4 colourOut;

uniform vec3 viewPos;
uniform vec3 colour;
uniform Material material;
uniform Light light[30];
uniform int lightNum;
uniform float gamma;

vec3 calcLights(int);

void main() {

   //This ambient value should not be hard coded
   vec3 ambient = 0.05 * vec3(texture(material.diffuse, FragUV));
   vec3 result = vec3(0.0f);
   for (int i=0; i<lightNum; i++)
      result += calcLights(i);

   vec3 finalColour = pow((ambient + result) * colour, vec3(1.0/gamma));
   colourOut = vec4(finalColour, 1.0f);
}




vec3 calcLights(int i) {

   // write a comment
   vec3 norm = normalize(FragNorm);
   vec3 lightDir = normalize(light[i].pos - FragPos);

   //Diffuse
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light[i].diffuse * diff * vec3(texture(material.diffuse, FragUV));

   //Specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
   vec3 specular = light[i].specular * spec * vec3(texture(material.diffuse, FragUV));

   //Attentuation
   float distance = length(light[i].pos - FragPos);
   float attenuation = 1.0f / (1.0f + light[i].linear * distance + light[i].quad * distance * distance);

   return (diffuse + specular) * attenuation;
}
