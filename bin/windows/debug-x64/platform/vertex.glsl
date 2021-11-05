#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
layout (location = 2) in vec2 aTexCoords;
  
out vec3 pColor; // output a color to the fragment shader
out vec2 pTexCoords;

layout (row_major, std140) uniform Matrices
{
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout (row_major, std140) uniform Colors
{
    vec3 color;
};

void main()
{
    gl_Position =  projection * model * vec4(aPos, 1.0);
    pColor = color; // set ourColor to the input color we got from the vertex data
    pTexCoords = aTexCoords;
}       