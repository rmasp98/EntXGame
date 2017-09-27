#version 400 core

struct Light2 {
   samplerCube depthMap;
};

struct Light {
   vec3 pos;
   vec3 ambient, diffuse, specular;
   float linear, quad;
};

in vec2 FragUV;

out vec4 colourOut;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform float farPlane;
uniform vec3 viewPos;

uniform float gamma;

uniform int lightNum;
uniform Light light[10];
uniform Light2 light2[3];

//uniform samplerCubeArray test;

vec3 FragPos;
vec3 FragNorm;
vec3 Diffuse;
float Specular;

int samples = 20;
vec3 offsets[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);


vec3 calcLights(int);
float ShadowCalculation(float, int);

void main() {
   FragPos = texture(gPosition, FragUV).rgb;
   FragNorm = texture(gNormal, FragUV).rgb;
   Diffuse = texture(gAlbedoSpec, FragUV).rgb;
   Specular = texture(gAlbedoSpec, FragUV).a;


   // This ambient value should not be hard coded
   vec3 ambient = 0.05 * Diffuse;
   vec3 result = vec3(0.0f);
   for (int i=0; i<lightNum; i++)
      result += calcLights(i);

   vec3 finalColour = pow((ambient + result), vec3(1.0/gamma));
   colourOut = vec4(finalColour, 1.0f);

}




vec3 calcLights(int i) {

   // write a comment
   vec3 norm = normalize(FragNorm);
   vec3 lightDir = normalize(light[i].pos - FragPos);

   //Diffuse
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light[i].diffuse * diff * Diffuse;

   //Specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(norm, halfwayDir), 0.0), Specular);
   vec3 specular = light[i].specular * spec * Diffuse;

   //Attentuation
   float distance = length(light[i].pos - FragPos);
   //float attenuation = 1.0f / (1.0f + light[i].linear * distance + light[i].quad * distance * distance);
   float attenuation = 1.0f / (1.0f + 0.07 * distance + 0.017 * distance * distance);

   float shadow = ShadowCalculation(diff, i);

   return (1.0f - shadow) * (diffuse + specular) * attenuation;
}




float ShadowCalculation(float diff, int iLight)
{
    // get vector between fragment position and light position
    vec3 fragToLight = FragPos - light[iLight].pos;

    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    float bias = max(0.2 * (1.0 - diff), 0.025);
    float shadow = 0.0f;
    float viewDistance = length(viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 50.0;

    for (int iSample = 0; iSample < samples; iSample++) {
       // ise the fragment to light vector to sample from the depth map
       float closestDepth = texture(light2[iLight].depthMap, fragToLight + offsets[iSample] * diskRadius).r;

       // it is currently in linear range between [0,1], let's re-transform it back to original depth value
       closestDepth *= farPlane;

       // test for shadows
       if ((currentDepth - bias) > closestDepth) {
         shadow += 1.0;
       }
    }

    return shadow /= float(samples);
}
