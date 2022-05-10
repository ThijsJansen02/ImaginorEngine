#version 450 core

layout(location = 0) out vec4 gColor;  
layout(location = 1) out int gIndex;

flat in uint pIndex;
in vec2 pTexCoords;

uniform sampler2D textures[32];

void main()
{  
    gIndex = int(pIndex); //int(pIndex);
    gColor = texture(textures[0], pTexCoords); //texture(texture0, pTexCoords);
}