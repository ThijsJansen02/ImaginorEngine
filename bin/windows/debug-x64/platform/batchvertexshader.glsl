#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aMaterialID;
  
out vec4 pColor; // output a color to the fragment shader
out vec2 pTexCoords;
out float pMaterialID;

layout (row_major, std140) uniform Matrices
{
    mat4 viewprojection;
};

void main()
{
    gl_Position =  viewprojection * vec4(aPos, 1.0);
    pMaterialID = aMaterialID;
    pColor = aColor; // set ourColor to the input color we got from the vertex data
    pTexCoords = aTexCoords;
}       