#version 400 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

out vec4 colourOut;

struct Material {
   sampler2D diffuse;
   float shininess;
};


in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragUV;

uniform Material material;

void main() {

   gPosition = FragPos;
   gNormal = normalize(FragNorm);
   gAlbedoSpec.rgb = texture(material.diffuse, FragUV).rgb;
   gAlbedoSpec.a = material.shininess;

   // colourOut = vec4(FragPos, 1.0f);

}
