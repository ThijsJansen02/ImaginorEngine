#version 330 core
out vec4 FragColor;  

in vec3 pColor;
in vec2 pTexCoords;

uniform vec3 colors;

uniform sampler2D texture1;
  
void main()
{  
    vec3 dcolor = 100 * colors;
    FragColor = texture(texture1, pTexCoords); //texture(texture0, pTexCoords);
}
