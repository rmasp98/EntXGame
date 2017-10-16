#version 420 core

in vec2 FragUV;
out vec4 colourOut;

uniform sampler2D text;
uniform vec3 colour;
uniform int renderType;

void main()
{
   if (renderType == 0) {
      vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, FragUV).r);
      colourOut = vec4(colour, 1.0) * sampled;
   } else if (renderType == 1)
      colourOut = vec4(texture(text, FragUV).rgb, 1.0);
   else if (renderType == 2)
      colourOut = vec4(0.0, 0.0, 0.0, 0.5);

}
