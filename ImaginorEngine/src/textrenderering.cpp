#include "textrenderering.h"
#include <dep/include/stb_image.h>

namespace IME {

    TextureAtlas
    loadTextureAtlas(const char* filename, uint32 nxsprites, uint32 nysprites, const PlatformInterface& platform, gl_id shader) {
        TextureAtlas result;
        result.nxsprites = nxsprites;
        result.nysprites = nysprites;

        result.spritewidth = (1.0f - 2.0f/128.0f) / (real32)result.nxsprites;
        result.spriteheight = 1.0f / (real32)result.nysprites;

        TextureProperties props;
        int32 nrchannels;

        result.shader = shader;

        stbi_set_flip_vertically_on_load(true);

        FileBuffer imagefilebuffer = platform.io.debug_readfile(filename, nullptr); 
        int32 channels;
        uint8* datasrc = stbi_load_from_memory((stbi_uc*)imagefilebuffer.data, imagefilebuffer.size, &props.width, &props.height, &channels, 3);
        props.format = IME_R;
        props.generatemipmaps = false;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;
        result.texture = platform.gfx.texture_create(props, (byte*)datasrc, IME_RGB, IME_UINT8);
        platform.io.debug_releasefilememory(&imagefilebuffer);
        stbi_image_free(datasrc);
        return result;
    }

    void 
    getUVcoordinatesFromTextureAtlas(TextureAtlas atlas, char character, vec2f* coordinates) {
        uint32 index = (character - atlas.offset);

        real32 x = (index % atlas.nxsprites) * atlas.spritewidth;
        real32 y = 1.0f - (index / atlas.nxsprites + 1) * atlas.spriteheight;

        coordinates[0] = {x, y};
        coordinates[1] = {x + atlas.spritewidth, y};
        coordinates[2] = {x + atlas.spritewidth, y + atlas.spriteheight};
        coordinates[3] = {x, y + atlas.spriteheight};
    };

    real32 
    calcTextHeight(const char* str, vec2f glyphsize, real32 textspacewidth, real32 textspacing) {
        int32 y = 0;
        int32 x = 0;
        for (uint32 i = 0; str[i] != '\0'; i++) {
            if (str[i] == '\n') {
                y += 1;
                x = 0;
                continue;
            };
            if ((x + 1) * glyphsize.x > textspacewidth) {
                y += 1;
                x = 0;
                continue;
            }
            x++;
        }

        return (y + 1) * (glyphsize.y + textspacing) - textspacing;
    }

    inline uint32
    stringToUsnignedInt(const char* str, uint32 n) {
        int32 i = 0;
        
        while(str[i] <= '9' && str[i] >= '0' && i < n) {
            i++;
        } 
        i--;
        uint32 result = 0;
        for(int32 j = 0; j <= i; j++) {
            result += (str[i - j] - '0') * powerInt32(10, j);
        }
        
        return result;
    }


    real32
    drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor, RenderQueue2D* renderqueue, real32 depth) {
        return drawStringFromTextureAtlas(str, start, glyphsize, atlas, textspacewidth, basecolor, renderqueue, depth, 0.0f);
    }

    real32
    drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor, RenderQueue2D* renderqueue, real32 depth, real32 linespacing) {

        if (str == nullptr) {
            return 0.0f;
        }
        
        vec4f color = basecolor;
        int32 x = 0;
        int32 y = 0;
        while(true) {
            if(*str == '\0') {
                break;
            }

            if(*str == ' ') {
                uint32 i = 1;
                while(true) {
                    if(str[i] == ' ' || str[i] == '\0') {
                        break;
                    }
                    i++;
                }

                if((x + i) * glyphsize.x > textspacewidth) {
                    y-=1;
                    x=0;
                    str++;
                }
            }
            //for(uint32 i = 0; str[i] != ' ')

            if(*str == '\u001b') {
                if (str[1] == '[') {
                    uint32 code = stringToUsnignedInt(&str[2], 2);
                    if(code == 0) { color = basecolor; str += 3; };
                    if(code == 30) { color = {0.0f, 0.0f, 0.0f, 1.0f}; str += 4;}
                    if(code == 31) { color = {1.0f, 0.0f, 0.0f, 1.0f}; str += 4;}
                    if(code == 32) { color = {0.0f, 1.0f, 0.0f, 1.0f}; str += 4;}
                    if(code == 33) { color = {0.0f, 1.0f, 1.0f, 1.0f}; str += 4;}
                    if(code == 34) { color = {0.0f, 0.0f, 1.0f, 1.0f}; str += 4;}
                    if(code == 35) { color = {1.0f, 0.0f, 1.0f, 1.0f}; str += 4;}
                    continue;
                }
            }
            if(*str == '\n') {
                y -= 1;
                x = 0;
                continue;
            };
            if((x + 1) * glyphsize.x > textspacewidth) {
                y -= 1;
                x = 0;
                //  continue;
            }


            RendererCommand2D command;
            command.shader = atlas.shader;
            command.color = color;
            command.transform = transformMat4(vec3f{start.x + x * glyphsize.x + glyphsize.x / 2.0f, start.y + y * (glyphsize.y + linespacing) - glyphsize.y / 2.0f, depth}, {glyphsize.x, glyphsize.y, 1.0f});
            getUVcoordinatesFromTextureAtlas(atlas, *str, command.texcoords);
            command.texture = atlas.texture;
            renderqueue->commands.push_back(command);
            str++;
            x++;
        }
        return (y - 1) * glyphsize.y;
    }

    vec2f calculateStringSize(const char* str, real32 maxwidth, vec2f glyphsize, real32 linespacing) {

        uint32 x = 1;
        uint32 maxcalcwidth = 0;
        uint32 y = 1;
        while(true) {
            if(*str == '\0') {
                break;
            }

            if(*str == ' ') {
                uint32 i = 1;
                while(true) {
                    if(str[i] == ' ' || str[i] == '\0') {
                        break;
                    }
                    i++;
                }

                if((x + i - 1) * glyphsize.x > maxwidth) {
                    maxcalcwidth = x > maxcalcwidth ? x : maxcalcwidth;
                    y+=1;
                    x=0;
                }
            }
            //for(uint32 i = 0; str[i] != ' ')

            if(*str == '\u001b') {
                if (str[1] == '[') {
                    uint32 code = stringToUsnignedInt(&str[2], 2);
                    if(code == 0) { str += 3; };
                    if(code == 30) { str += 4;}
                    if(code == 31) { str += 4;}
                    if(code == 32) { str += 4;}
                    if(code == 33) { str += 4;}
                    if(code == 34) { str += 4;}
                    if(code == 35) { str += 4;}
                    continue;
                }
            }
            if(*str == '\n') {
                maxcalcwidth = x > maxcalcwidth ? x : maxcalcwidth;
                y += 1;
                x = 0;
            };
            if((x + 1) * glyphsize.x > maxwidth) {
                maxcalcwidth = x > maxcalcwidth ? x : maxcalcwidth;
                y += 1;
                x = 0;
            }
            x++;
            str++;
        }

        maxcalcwidth = maxcalcwidth < x ? x : maxcalcwidth;
        return {maxcalcwidth * glyphsize.x, (y) * (glyphsize.y + linespacing) - linespacing};
    }

    real32
    drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor, RenderQueue2D* renderqueue) {
        return drawStringFromTextureAtlas(str, start, glyphsize, atlas, textspacewidth, basecolor, renderqueue, 0.0f);
    }

}