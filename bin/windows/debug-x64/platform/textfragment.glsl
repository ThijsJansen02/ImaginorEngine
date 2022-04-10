#version 330 core
out vec4 FragColor;  

in vec4 pColor;
in vec2 pTexCoords;
flat in float pMaterialID;

uniform sampler2D textures[32];
  
void main()
{   
    vec4 texturefagment = texture(textures[int(pMaterialID)], pTexCoords);
    FragColor = vec4(pColor.r, pColor.g, pColor.b, texturefagment.a);
}