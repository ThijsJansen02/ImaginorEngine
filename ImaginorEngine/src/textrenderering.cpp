#include "textrenderering.h"
#include <stb/stb_image.h>
#include <stb/stb_treutype.h>
#include "rendering.h"
#include "rendering/Renderer2D.h"

namespace IME {

    inline uint32
    stringToUsnignedInt(const char* str, uint32 n) {

        int32 i = 0;
        while(str[i] <= '9' && str[i] >= '0' && i < n && str[i] != 0) {
            i++;
        } 
        i--;
        uint32 result = 0;
        for(int32 j = 0; j <= i; j++) {
            result += (str[i - j] - '0') * powerInt32(10, j);
        }
        
        return result;
    }

    bool32 stringCompare(const char* a, const char* b) {
        
        bool32 result = true;
        while(*a && *b) {
            if(*a != *b) {
                result = false;
                break;
            }
            a++; b++;
        }
        if(*a != *b) {
            result = false;
        }
        return result;

    }

    vec2f calcTextSizeFromFont(const char* str, real32 textspacewidth, real32 linespacing, real32 scalefactor, const Assets::Font& font) {

        real32 lineheight = scalefactor * font.pixelheight;
        real32 ascent = scalefactor * font.scale * font.ascent;
        real32 descent = scalefactor * font.scale * font.descent;

        textspacewidth = textspacewidth / scalefactor;

        real32 maxwidth = 0.0f;

        vec2f start = {0.0f, 0.0f};
        real32 lines = 1.0f;

        while (*str) {

            if(*str == '\n') {
                start.x = 0.0f;
                lines += 1.0f;
                ++str;
                continue;
            }

            if (*str >= 32 && *str < 128) {

                if(*str == ' ') {
                    vec2f probe = start;
                    const char* str_probe = str;

                    real32 width = start.x;

                    stbtt_aligned_quad q;
                    stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str_probe-32, &probe.x, &probe.y, &q,1);//1=opengl & d3d10+,0=d3d9
                    ++str_probe;

                    while (*str_probe && *str_probe != ' ' && *str_probe != '\n') {
                        if (*str >= 32 && *str < 128) {
                            
                            //keep track of what thte last 

                            stbtt_aligned_quad q_probe;
                            stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str_probe-32, &probe.x, &probe.y, &q_probe, 1);

                            if(q_probe.x1 >= textspacewidth) {
                                if(maxwidth < width) {
                                    maxwidth = width;
                                }
                                start.x = 0.0f;
                                lines += 1.0f;
                                ++str;
                                break;
                            }

                            ++str_probe;
                        }
                    }
                }
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str-32, &start.x, &start.y, &q,1);//1=opengl & d3d10+,0=d3d9
            }
            ++str;
        } 

        if(start.x > maxwidth) {
            maxwidth = start.x;
        }
        return {maxwidth * scalefactor, (lines * lineheight)};
    }
    vec2f pushTextFromFontToRQ(
                        const char* str, 
                        vec3f topleft, 
                        real32 textspacewidth, 
                        real32 linespacing, 
                        const vec4f& basecolor, 
                        real32 scalefactor,
                        const Assets::Font& font, 
                        Rendering::ArrayList<Rendering::SimpleQuadCommand>* rq) {

        real32 lineheight = scalefactor * font.pixelheight;
        real32 ascent = scalefactor * font.scale * font.ascent;
        real32 descent = scalefactor * font.scale * font.descent;

        textspacewidth = textspacewidth / scalefactor;

        real32 maxwidth = 0.0f;

        vec2f start = {0.0f, 0.0f};
        real32 lines = 1.0f;

        while (*str) {

            if(*str == '\n') {
                start.x = 0.0f;
                lines += 1.0f;
                ++str;
                continue;
            }

            if (*str >= 32 && *str < 128) {

                if(*str == ' ') {
                    vec2f probe = start;
                    const char* str_probe = str;

                    real32 width = start.x;

                    stbtt_aligned_quad q;
                    stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str_probe-32, &probe.x, &probe.y, &q,1);//1=opengl & d3d10+,0=d3d9
                    ++str_probe;

                    while (*str_probe && *str_probe != ' ' && *str_probe != '\n') {
                        stbtt_aligned_quad q_probe;
                        stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str_probe-32, &probe.x, &probe.y, &q_probe, 1);

                        if(q_probe.x1 >= textspacewidth) {
                            if(width > maxwidth) {
                                maxwidth = width;
                            }

                            start.x = 0.0f;
                            lines += 1.0f;
                            ++str;
                            break;
                        }

                        ++str_probe;
                    }
                }

                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str-32, &start.x, &start.y, &q,1);//1=opengl & d3d10+,0=d3d9

                vec2f texcoords[4];
                texcoords[0] = { q.s0, q.t0 };
                texcoords[1] = { q.s1, q.t0 };
                texcoords[2] = { q.s1, q.t1 };
                texcoords[3] = { q.s0, q.t1 };

                vec3f pos = topleft;
                pos.x = (q.x0 + q.x1) / 2.0f;
                pos.y = (q.y0 + q.y1) / 2.0f;

                pos = scalefactor * pos * vec3f{1.0f, -1.0f, 1.0f};

                vec2f size = scalefactor * vec2f{q.x1 - q.x0, q.y0 - q.y1 };

                Rendering::pushQuad(pos + topleft - vec3f{0.0f, (lines - 1) * lineheight + ascent, 0.0f}, size, basecolor, font.fontshader, font.texture, texcoords, rq);
            }
            ++str;
        } 
    
        if(start.x > maxwidth) {
            maxwidth = start.x;
        }

        return {maxwidth * scalefactor, (lines * lineheight)};
    }
    real32 drawTextFromFont(const char* str, vec3f topleft, real32 textspacewidth, real32 linespacing, const vec4f& basecolor, const Assets::Font& font) {
        
        real32 pixelheight = font.pixelheight;
        real32 ascent = font.scale * font.ascent;
        real32 descent = font.scale * font.descent;

        vec2f start = {0.0f, 0.0f};

        real32 lines = 1.0f;

        while (*str) {

            if(*str == '\n') {
                start.x = 0.0f;
                lines += 1.0f;
                ++str;
                continue;
            }

            if (*str >= 32 && *str < 128) {

                if(*str == ' ') {
                    vec2f probe = start;
                    const char* str_probe = str;

                    stbtt_aligned_quad q;
                    stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str_probe-32, &probe.x, &probe.y, &q,1);//1=opengl & d3d10+,0=d3d9
                    ++str_probe;

                    while (*str_probe && *str_probe != ' ') {
                        stbtt_aligned_quad q_probe;
                        stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str_probe-32, &probe.x, &probe.y, &q_probe, 1);

                        if(q_probe.x1 >= textspacewidth) {
                            start.x = 0.0f;
                            lines += 1.0f;
                            ++str;
                            break;
                        }

                        ++str_probe;
                    }
                }

                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *str-32, &start.x, &start.y, &q,1);//1=opengl & d3d10+,0=d3d9

                vec2f texcoords[4];
                texcoords[0] = { q.s0, q.t0 };
                texcoords[1] = { q.s1, q.t0 };
                texcoords[2] = { q.s1, q.t1 };
                texcoords[3] = { q.s0, q.t1 };

                vec3f pos = topleft;
                pos.x = (q.x0 + q.x1) / 2.0f;
                pos.y = (q.y0 + q.y1) / 2.0f;

                pos = pos * vec3f{1.0f, -1.0f, 1.0f};

                vec2f size = {q.x1 - q.x0, q.y0 - q.y1 };

                Renderer2D::drawTexturedQuad(pos + topleft - vec3f{0.0f, (lines - 1) * pixelheight + ascent, 0.0f}, size, basecolor, texcoords);
            }
            ++str;
        } 
        return (lines * pixelheight);
    }

    real32 calcTextLength(const char* str, const Assets::Font& font, const char* end, real32 scalefactor) {

        vec2f start = {0.0f, 0.0f};
        const char* strptr = str;
        while(strptr != end) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *strptr-32, &start.x, &start.y, &q,1);
            strptr++;
        }
        return scalefactor * start.x;
    }

    void pushInputTextFromFontToRQ(
                        const char* str, 
                        vec3f topleft, 
                        real32 textspacewidth, 
                        real32 scalefactor,
                        real32 cursor, 
                        uint32 textorigin, 
                        const vec4f& basecolor, 
                        const Assets::Font& font, 
                        Rendering::ArrayList<Rendering::SimpleQuadCommand>* rq
    ) {

            real32 lineheight = scalefactor * font.pixelheight;
            real32 ascent = scalefactor * font.scale * font.ascent;
            real32 descent = scalefactor * font.scale * font.descent;

            textspacewidth;

            const char* strptr = str;
            vec2f pos = {0.0f, 0.0f};

            real32 lengthtillspaceorigin = calcTextLength(str, font, str + textorigin, scalefactor);
            vec3f textbegin = { topleft.x + cursor - lengthtillspaceorigin, topleft.y, topleft.z }; 
            vec3f start = { 0.0f, 0.0f, textbegin.z };

            while(*strptr) {
                
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, *strptr-32, &start.x, &start.y, &q,1);

                vec3f pos = start;
                pos.x = q.x0;
                pos.y = q.y0;
                pos = scalefactor * pos * vec3f{1.0f, -1.0f, 1.0f} + textbegin - vec3f{0.0f, ascent, 0.0f};

                vec2f size = scalefactor * vec2f{q.x1 - q.x0, q.y1 - q.y0 };

                real32 end = topleft.x + textspacewidth;
                //if the character fits half at the start of drawing area
                if(pos.x <= topleft.x && pos.x + size.x >= topleft.x) {
                    real32 factor = (topleft.x - pos.x) / size.width;

                    vec2f texcoords[4];
                    texcoords[0] = { q.s0 += factor * (q.s1 - q.s0), q.t1 }; //bottom left
                    texcoords[1] = { q.s1, q.t1 }; //bottomright
                    texcoords[2] = { q.s1, q.t0 }; //top right
                    texcoords[3] = { q.s0 += factor * (q.s1 - q.s0), q.t0 }; //topleft

                    pos.x = topleft.x;
                    size.width -= size.width * factor;

                    Rendering::pushQuadTL(pos, size, basecolor, font.fontshader, font.texture, texcoords, rq);
                }

                //if the character fits half at the end of the textarea
                else if(pos.x <= end && pos.x + size.x >= end) {
                    real32 factor = (pos.x + size.x - end) / size.width;

                    vec2f texcoords[4];
                    texcoords[0] = { q.s0, q.t1 };
                    texcoords[1] = { q.s1 -= factor * (q.s1 - q.s0), q.t1 };
                    texcoords[2] = { q.s1 -= factor * (q.s1 - q.s0), q.t0 };
                    texcoords[3] = { q.s0, q.t0 };

                    size.width -= size.width * factor;

                    Rendering::pushQuadTL(pos, size, basecolor, font.fontshader, font.texture, texcoords, rq);
                    break;
                }

                //if the character doesnt fit in front
                else if(pos.x < topleft.x) {
                    strptr++;
                    continue;
                }

                //if the character doesnt fit in the end
                else if(pos.x > end) {
                    break;
                }
                
                //if the character fits
                vec2f texcoords[4];
                texcoords[0] = { q.s0, q.t1 };
                texcoords[1] = { q.s1, q.t1 };
                texcoords[2] = { q.s1, q.t0 };
                texcoords[3] = { q.s0, q.t0 };

                Rendering::pushQuadTL(pos, size, basecolor, font.fontshader, font.texture, texcoords, rq);
                
                strptr++;
            } 
        return;
    }

    real32 getCharacterLength(char character, real32 scalefactor, const Assets::Font& font) {
        vec2f start = {0.0f, 0.0f};
        real32 height = font.pixelheight * scalefactor;


        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(font.cdata, font.props.width, font.props.height, character-32, &start.x, &start.y, &q,1);
        return start.x * scalefactor;
    }
    
}