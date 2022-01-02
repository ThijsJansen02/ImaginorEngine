#pragma once
#include <ImaginorPlatform\src\imegs.h>

namespace IME::OpenGL {

    extern "C" IME_GLAPI_INIT(ime_glapi_init);

    extern "C" IME_GLAPI_RBO_CREATE(ime_glapi_create_rbo);
    extern "C" IME_GLAPI_RBO_BIND(ime_glapi_bind_rbo);
    extern "C" IME_GLAPI_RBO_ADD_BUFFER(ime_glapi_add_buffer);
    extern "C" IME_GLAPI_RBO_SET_INDEXBUFFER(ime_glapi_rbo_set_indexbuffer);

    extern "C" IME_GLAPI_VBO_BIND(ime_glapi_vbo_bind);
    extern "C" IME_GLAPI_VBO_BUFFER_SUB_DATA(ime_glapi_vbo_buffer_sub_data);
 
    extern "C" IME_GLAPI_DRAW_ARRAY(ime_glapi_draw_array);
    extern "C" IME_GLAPI_DRAW_INDEXED(ime_glapi_draw_indexed);

    extern "C" IME_GLAPI_SHADER_CREATE(ime_glapi_create_shader);
    extern "C" IME_GLAPI_SHADER_BIND(ime_glapi_bind_shader);
    extern "C" IME_GLAPI_SHADER_ADD_PROGRAM(ime_glapi_shader_add_program);
    extern "C" IME_GLAPI_SHADER_SET_UBO_BINDINGPOINT(ime_glapi_shader_set_ubo_bindingpoint);
    extern "C" IME_GLAPI_SHADER_SET_TEXTURE_BINDING(ime_glapi_shader_set_texture_binding);
    extern "C" IME_GLAPI_SHADER_COMPILE(ime_glapi_shader_compile);

    extern "C" IME_GLAPI_UBO_CREATE(ime_glapi_create_ubo);
    extern "C" IME_GLAPI_UBO_BIND(ime_glapi_bind_ubo);
    extern "C" IME_GLAPI_UBO_BUFFER_DATA(ime_glapi_ubo_buffer_data);
    extern "C" IME_GLAPI_UBO_BUFFER_SUB_DATA(ime_glapi_ubo_buffer_sub_data);

    extern "C" IME_GLAPI_FBO_CREATE(ime_glapi_fbo_create);
    extern "C" IME_GLAPI_FBO_CREATE_TEXTURE_ATTACHMENT(ime_glapi_fbo_create_texture_attachment);
    extern "C" IME_GLAPI_FBO_BIND(ime_glapi_fbo_bind);

    extern "C" IME_GLAPI_TEXTURE_CREATE(ime_glapi_texture_create);
    extern "C" IME_GLAPI_TEXTURE_BIND(ime_glapi_texture_bind);
    extern "C" IME_GLAPI_TEXTURE_RESET(ime_glapi_texture_reset);

    extern "C" IME_GLAPI_ENABLE(ime_glapi_enable);
    extern "C" IME_GLAPI_DISABLE(ime_glapi_disable);

    extern "C" IME_GLAPI_CLEAR(ime_glapi_clear);
    extern "C" IME_GLAPI_SET_VIEWPORT(ime_glapi_set_viewport);
     
}