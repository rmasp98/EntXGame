#version 330 core

in vec2 FragUV;
out vec4 colour;

uniform sampler2D text;
uniform vec3 textColour;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, FragUV).r);
    colour = vec4(textColour, 1.0) * sampled;
}
