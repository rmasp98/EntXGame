#version 400 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragUV;

struct Material {
   sampler2D diffuse;
   float shininess;
};
uniform Material material;

void main() {

   gPosition = FragPos;
   gNormal = normalize(FragNorm);
   gAlbedoSpec.rgb = texture(material.diffuse, FragUV).rgb;
   gAlbedoSpec.a = material.shininess;

}
