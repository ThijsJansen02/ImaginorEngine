#version 440 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 pTexCoords;

uniform int offset;

layout (row_major, std140) uniform Scene 
{
    mat4 viewprojection;
};

layout (row_major, std140) uniform Transforms 
{
    mat4 transform[128];
};

void main()
{
    gl_Position =  viewprojection * transform[offset] * vec4(aPos, 1.0);
    pTexCoords = aTexCoords;
}       