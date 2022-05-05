#version 330 core

out vec4 FragColor;  

in vec2 pTexCoords;

uniform sampler2D textures[32];

void main()
{  
    FragColor = texture(textures[0], pTexCoords); //texture(texture0, pTexCoords);
}
