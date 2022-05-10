#version 450 core

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 pTexCoords;
flat out uint pIndex;

uniform int offset;

struct InstanceInformation {
    mat4 transform;
    uint id;
    uint materialoffset;
    uint padding1;
    uint padding2;
};

layout (row_major, std140) uniform Scene 
{
    mat4 viewprojection;
};

layout (row_major, std140) uniform Transforms 
{
    InstanceInformation instances[128];
};

void main()
{
    pIndex = instances[offset].id;
    gl_Position =  viewprojection * instances[offset].transform * vec4(aPos, 1.0);
    pTexCoords = aTexCoords;
}       