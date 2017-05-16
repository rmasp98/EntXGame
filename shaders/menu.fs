#version 330 core

in vec2 FragUV;
out vec4 colour;

uniform sampler2D text;
uniform vec3 textColour;
uniform int renderType;

void main()
{
   if (renderType == 0) {
      vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, FragUV).r);
      colour = vec4(textColour, 1.0) * sampled;
   } else if (renderType == 1) 
      colour = vec4(texture(text, FragUV).rgb, 1.0);
   else if (renderType == 2)
      colour = vec4(0.0, 0.0, 0.0, 0.5);

}
