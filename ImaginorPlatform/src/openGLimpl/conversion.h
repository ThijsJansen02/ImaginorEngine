#include <core.h>
#include <ImaginorPlatform/src/imegs.h>

namespace IME::OpenGL {

    global_var sizeptr datatypesizes[18] = {
        0,
        sizeof(uint8),
        sizeof(uint16),
        sizeof(uint32),
        sizeof(int8),
        sizeof(int16),
        sizeof(int32),
        sizeof(real32),
        2 * sizeof(real32),
        3 * sizeof(real32),
        4 * sizeof(real32),
        sizeof(real64),
        2 * sizeof(real64),
        3 * sizeof(real64),
        4 * sizeof(real64),
        2 * 2 * sizeof(real32),
        3 * 3 * sizeof(real32),
        4 * 4 * sizeof(real32)
    };

    sizeptr getDataTypeSize(gsdatatype datatype) {
        return datatypesizes[(uint32)datatype];
    }

    global_var int32 gldatatypes[18] = {
        GL_NONE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_INT,
        GL_BYTE,
        GL_SHORT,
        GL_INT,
        GL_FLOAT,
        GL_FLOAT,
        GL_FLOAT,
        GL_FLOAT,
        GL_DOUBLE,
        GL_DOUBLE,
        GL_DOUBLE,
        GL_DOUBLE,
        GL_FLOAT_MAT2,
        GL_FLOAT_MAT3,
        GL_FLOAT_MAT4
    };

    int32 getDataType(gsdatatype datatype) {
        int32 result = gldatatypes[datatype];
        return result;
    }

    global_var int32 datatypecomponents[18] = {
        0,      //none
        1,      //uint8
        1,      //uint16 
        1,      //uint32
        1,      //int8
        1,      //int16
        1,      //int32
        1,      //real32
        2,      //vec2f
        3,      //vec3f
        4,      //vec4f
        1,      //real64
        2,      //vec2d
        3,      //vec3d
        4,      //vec4d
        2 * 2,  //mat2f
        3 * 3,  //mat3f
        4 * 4   //mat4f
    };
    
    int32
    getDataTypeComponentCount(gsdatatype datatype) {
        return datatypecomponents[datatype];
    }

    global_var int32 glshadertypes[2] {
        GL_FRAGMENT_SHADER,
        GL_VERTEX_SHADER
    };

    int32
    getShaderType(gsshaderprogramtype programtype) {
        return glshadertypes[programtype];
    }

    global_var GLenum glcolorformats[] = {
        GL_RED,
        GL_GREEN, 
        GL_BLUE, 
        GL_ALPHA,
        GL_RG,
        GL_RGB,
        GL_RGBA,
        GL_DEPTH_COMPONENT,
        GL_DEPTH_COMPONENT,
        GL_DEPTH_COMPONENT,
        GL_DEPTH_STENCIL,
        GL_RED_INTEGER,
        GL_RED_INTEGER
    };

    GLenum 
    getColorFormat(gstextureformat format) {
        GLenum result = glcolorformats[format];
        return result;
    }

    global_var GLint glinternalcolorformats[] = {
        GL_RED,
        GL_GREEN, 
        GL_BLUE,
        GL_ALPHA,
        GL_RG8,
        GL_RGB8,
        GL_RGBA8,
        GL_DEPTH_COMPONENT16,
        GL_DEPTH_COMPONENT24,
        GL_DEPTH_COMPONENT32,
        GL_DEPTH24_STENCIL8,
        GL_R32I,
        GL_R32UI
    };

    GLint getInternalColorFormat(gstextureformat format) {
        GLint result = glinternalcolorformats[format];
        return result;
    }

    global_var GLenum glcolordatatypes[] = {
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE, 
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_INT,
        GL_UNSIGNED_INT,
        GL_UNSIGNED_INT,
        GL_UNSIGNED_INT,
        GL_INT,
        GL_UNSIGNED_INT
    };

    GLenum getDataTypeForColorFormat(gstextureformat format) {
        return glcolordatatypes[format];
    }

    uint32 gltexturewraps[] = { 
        GL_REPEAT,
        GL_MIRRORED_REPEAT,
        GL_CLAMP_TO_EDGE,
        GL_CLAMP_TO_BORDER
    };

    int32 getTextureWrap(gstexturewrap wrap) {
        return gltexturewraps[wrap];
    }

    int32 gltexturefiltering[] = {
        GL_NEAREST,
        GL_LINEAR,
        GL_NEAREST_MIPMAP_NEAREST,
        GL_NEAREST_MIPMAP_LINEAR,
        GL_LINEAR_MIPMAP_NEAREST,
        GL_LINEAR_MIPMAP_LINEAR
    };

    int32 getTextureFiltering(gstexturefiltering filtering) {
        return gltexturefiltering[filtering];
    }

    GLenum framebufferattachmenttypes[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
        GL_COLOR_ATTACHMENT8,
        GL_COLOR_ATTACHMENT9,
        GL_COLOR_ATTACHMENT10,
        GL_COLOR_ATTACHMENT11,
        GL_COLOR_ATTACHMENT12,
        GL_COLOR_ATTACHMENT13,
        GL_COLOR_ATTACHMENT14,
        GL_COLOR_ATTACHMENT15,
        GL_DEPTH_ATTACHMENT,
        GL_STENCIL_ATTACHMENT,
        GL_DEPTH_STENCIL_ATTACHMENT
    };

    GLenum getFrameBufferAttachmentType(gsframebufferattachmenttype type) {
        return framebufferattachmenttypes[type];
    }

    int32 drawingmodes[] = {
        GL_LINES,
        GL_LINE_STRIP,
        GL_POINTS,
        GL_TRIANGLES
    };

    int32 getDrawingMode(gsdrawmode mode) {
        return drawingmodes[mode];
    }

    int32 operations[] = {
        GL_DEPTH_TEST,
        GL_STENCIL_TEST,
        GL_BLEND
    };

    int32 convertOperation(operation op) {
        return operations[op];
    }

    int32 convertClearBuffer(clearbuffer cb) {
        int32 result = GL_NONE;
        if(cb & IME_COLOR_BUFFER > 0) {
            result |= GL_COLOR_BUFFER_BIT;
        }
        if(cb & IME_DEPTH_BUFFER > 0) {
            result |= GL_DEPTH_BUFFER_BIT;
        }
        if(cb & IME_STENCIL_BUFFER > 0) {
            result |= GL_STENCIL_BUFFER_BIT;
        }

        return result;
    }
}