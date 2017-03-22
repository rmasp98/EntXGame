#version 330 core

//Need to pass font texture to this
//uniform sampler2D tex;

struct Material {
   sampler2D diffuse, specular;
   float shininess;
};

out vec4 color;
in vec2 FragUV;
uniform Material material;

void main()
{
    color = vec4(texture(material.diffuse, FragUV), 0.0f)
}
