#pragma once
#include <core.h>
#include <intrinsics.h>
#include <LinearMath/mat3.h>
#include <LinearMath/vec2.h>
#include <ImaginorPlatform/src/platform.h>
#include <LinearMath/ivec2.h>

namespace IME
{
    struct RendererState {
        bool32 backfaceculling;
    };

    struct FrameBuffer {
    };

    struct VertexBuffer {
        void* vertices;
        uint32 vertexsize;
        uint32 count;
    };

    struct IndexBuffer {
        uint32* indeces;
        uint32 count;
    };

    struct ColorRGBA
    {
        real32 r;
        real32 g;
        real32 b;
        real32 a;
    };

    struct ImageBuffer {

    ColorFragment* data = nullptr;

        uint32 width = 0;
        uint32 height = 0;
    };

    ColorFragment
    colorToColorFragment(const ColorRGBA &color)
    {
        ColorFragment result;
        result.r = (uint8)(color.r * 255.0f);
        result.g = (uint8)(color.g * 255.0f);
        result.b = (uint8)(color.b * 255.0f);
        result.a = (uint8)(color.a * 255.0f);
        return result;
    }

    void 
    drawQuadImmidiate(int32 xo, int32 yo, int32 width, int32 height, const Canvas &canvas, ColorFragment color)
    {
        for (int32 y = 0; y < height; y++)
        {
            for (int32 x = 0; x < width; x++)
            {
                canvas.data[xo + x + (yo + y) * canvas.width] = color;
            }
        }
    }

    void 
    drawQuadImmidiate(int32 xo, int32 yo, int32 width, int32 height, const Canvas &canvas, ImageBuffer image)
    {
        for (real32 y = 0; y < height; y++)
        {
            for (real32 x = 0; x < width; x++)
            {
                real32 imagepercentX = x / width;
                real32 imagepercentY = y / height;
                
                ColorFragment color = image.data[(uint32)floorReal32(imagepercentX * image.width) + (uint32)floorReal32(imagepercentY * image.height) * image.width];

                if(color.a > 0) {
                    canvas.data[(uint32)(xo + x + (yo + y) * canvas.width)] = color;
                } 
            }
        }
    }

    inline void 
    swap(real32 *l, real32 *r)
    {
        real32 plh = *l;
        *l = *r;
        *r = plh;
    }

    inline void
    drawQuad(real32 x1, real32 y1, real32 x2, real32 y2, const Canvas &canvas, const ColorRGBA &color)
    {
        if (x1 > x2) { swap(&x1, &x2); }
        if (y1 > y2) { swap(&y1, &y2); }

        if (x1 < 0.0f) { x1 = 0.0f; }
        if (y1 < 0.0f) { y1 = 0.0f; }
        if (x2 >= canvas.width) { x2 = canvas.width; }
        if (y2 >= canvas.height) { y2 = canvas.height; }

        drawQuadImmidiate(floorReal32(x1), floorReal32(y1), ceilReal32(x2 - x1), ceilReal32(y2 - y1), canvas, colorToColorFragment(color));
    }

    inline void
    drawQuad(real32 x1, real32 y1, real32 x2, real32 y2, const Canvas &canvas, const ImageBuffer& imagebuffer)
    {
        if (x1 > x2) { swap(&x1, &x2); }
        if (y1 > y2) { swap(&y1, &y2); }

        if (x1 < 0.0f) { x1 = 0.0f; }
        if (y1 < 0.0f) { y1 = 0.0f; }
        if (x2 >= canvas.width) { x2 = canvas.width; }
        if (y2 >= canvas.height) { y2 = canvas.height; }

        drawQuadImmidiate(floorReal32(x1), floorReal32(y1), ceilReal32(x2 - x1), ceilReal32(y2 - y1), canvas, imagebuffer);
    }

    inline void
    drawQuad(vec2f pos1, vec2f pos2, const Canvas &canvas, const ColorRGBA &color)
    {
        drawQuad(pos1.x, pos1.y, pos2.x, pos2.y, canvas, color);
    }

    inline void
    drawQuad(vec2f pos1, vec2f pos2, const Canvas &canvas, const ImageBuffer &iamgebuffer)
    {
        drawQuad(pos1.x, pos1.y, pos2.x, pos2.y, canvas, iamgebuffer);
    }

    inline void 
    drawQuad(const mat3 &transform, const Canvas &canvas, const ColorRGBA &color)
    {
        vec2f pos1 = transform * vec2f(0.0f, 0.0f);
        vec2f pos2 = transform * vec2f(1.0f, -1.0f);
        drawQuad(pos1, pos2, canvas, color);
    }

    inline void 
    clearCanvas(const Canvas &canvas, ColorFragment clearcolor)
    {
        for (int y = 0; y < canvas.height; y++)
        {
            for (int x = 0; x < canvas.width; x++)
            {
                canvas.data[x + y * canvas.width] = clearcolor;
            }
        }
    }

    inline void 
    swap(int32* value1, int32* value2) {
        int32 temp = *value1;
        *value1 = *value2;
        *value2 = temp;
    }

    inline void 
    drawLineImmediate(int32 x1, int32 y1, int32 x2, int32 y2, Canvas canvas, ColorFragment color) {

        int32 deltaX = x2 - x1;
        int32 deltaY = y2 - y1;

        if(absolute(deltaX) > absolute(deltaY)) {

            if(x1 > x2) {
                swap(&x1, &x2);
                swap(&y1, &y2);
            }
            real32 rico = (real32)(deltaY) / (real32)(deltaX);
            int32 offset = y1 - rico * x1;

            for(int32 x = x1; x < x2; x++) {
                int32 y = (rico * x + offset);
                if(x < 0 || x >= canvas.width || y < 0 || y >= canvas.height) {
                    continue;
                }
                canvas.data[floorReal32(x + y * canvas.width)] = color;
            }
        } else {

            if(y1 > y2) {
                swap(&x1, &x2);
                swap(&y1, &y2);
            }
            real32 rico = (real32)(deltaX) / (real32)(deltaY);
            int32 offset = x1 - rico * y1;

            for(int32 y = y1; y < y2; y++) {
                int32 x = (rico * y + offset);
                if(x < 0 || x >= canvas.width || y < 0 || y >= canvas.height) {
                    continue;
                }
                canvas.data[floorReal32(x + y * canvas.width)] = color;
            }
        }
    }

    inline void 
    drawLinesLooped(vec2f* lines, uint32 count, Canvas canvas, const mat3& transform) {

        if(count > 1) {
            vec2f prevpoint = transform * lines[0];
            for(uint32 i = 1; i < count + 1; i++) {
                vec2f nextpoint = transform * lines[i % count];
                drawLineImmediate(prevpoint.x, prevpoint.y, nextpoint.x, nextpoint.y, canvas, {255, 255, 255, 255});
                prevpoint = nextpoint;
            }
        }
    }

    inline void 
    swap(vec2f* p1, vec2f* p2) {
        vec2f temp = *p1;
        *p1 = *p2;
        *p2 = temp;
    }

    void putPixelSafe(ivec2 position, ColorRGBA color, Canvas canvas) {
        if(position.x < 0 || position.y < 0 || position.x >= canvas.width || position.y >= canvas.height) {
            return;
        }
        canvas.data[position.x + position.y * canvas.width] = colorToColorFragment(color);
    }

    void putPixel(ivec2 position, ColorRGBA color, Canvas canvas) {
        IME_DEBUG_ASSERT_BREAK(position.x < 0 || position.y <0 || position.x >= canvas.width || position.y >= canvas.height, "pixel is not in canvas!")
        canvas.data[position.x + position.y * canvas.width] = colorToColorFragment(color);
    }

    void drawTriangleFlatTop(vec2f v0, vec2f v1, vec2f v2, Canvas canvas) {

        IME_DEBUG_ASSERT_BREAK(v0.y == v1.y, "not a flat top triangle")
        IME_DEBUG_ASSERT_BREAK(v2.y >= v1.y, "not a flat triangle")

        if(v0.x > v1.x) swap(&v0, &v1);

        real32 m0 = (v2.x - v0.x) / (v2.y - v0.y);
        real32 m1 = (v2.x - v1.x) / (v2.y - v1.y);

        const int startY = ceilReal32(v0.y - 0.5f);
        const int endY = ceilReal32(v2.y - 0.5f);

        for(int32 y = startY; y < endY; y++) {

            const real32 px0 = m0 * (real32(y) + 0.5f - v0.y) + v0.x;
            const real32 px1 = m1 * (real32(y) + 0.5f - v1.y) + v1.x;

            const int32 startX = ceilReal32(px0 - 0.5f);
            const int32 endX = ceilReal32(px1 - 0.5f);

            for(int32 x = startX; x < endX; x++) {
                putPixelSafe({x, y}, {1.0f, 1.0f, 1.0f, 1.0f}, canvas);
            }
        }

    }

    void drawTriangleFlatBottom(vec2f v0, vec2f v1, vec2f v2, Canvas canvas) {

        IME_DEBUG_ASSERT_BREAK(v2.y == v1.y, "not a flat bottom triangle")
        IME_DEBUG_ASSERT_BREAK(v0.y <= v1.y, "not a flat bottom triangle")

        if(v2.x > v1.x) swap(&v1, &v2);

        real32 m0 = (v2.x - v0.x) / (v2.y - v0.y);
        real32 m1 = (v1.x - v0.x) / (v1.y - v0.y);

        const int32 startY = ceilReal32(v0.y - 0.5f);
        const int32 endY = ceilReal32(v2.y - 0.5f);

        for(int32 y = startY; y < endY; y++) {

            const real32 px0 = m0 * (real32(y) + 0.5f - v0.y) + v0.x;
            const real32 px1 = m1 * (real32(y) + 0.5f - v1.y) + v1.x;

            const int32 startX = ceilReal32(px0 - 0.5f);
            const int32 endX = ceilReal32(px1 - 0.5f);

            for(int32 x = startX; x < endX; x++) {
                putPixelSafe({x, y}, {1.0f, 1.0f, 1.0f, 1.0f}, canvas);
            }

        }
    }

    void 
    d_drawTriangle(vec2f v0, vec2f v1, vec2f v2, Canvas canvas) {

        if (v1.y < v0.y) { swap(&v1, &v0); }
        if (v2.y < v0.y) { swap(&v2, &v0); }
        if (v2.y < v1.y) { swap(&v2, &v1); }

        //triangle is already flat top
        if(v0.y == v1.y) {
            drawTriangleFlatTop(v0, v1, v2, canvas);
            return;
        }

        //triangle is already flatbottom
        if(v2.y == v1.y) {
            drawTriangleFlatBottom(v0, v1, v2, canvas);
            return;
        }

        real32 alpha = (v1.y - v0.y) / (v2.y - v0.y);
        vec2 vi = v0 + (alpha * (v2 - v0));

        if(v1.x > vi.x) swap(&v1, &vi);

        drawTriangleFlatBottom(v0, v1, vi, canvas);
        drawTriangleFlatTop(vi, v1, v2, canvas);
    }


    template<typename T>
    T interpolate(T src, T dest, T alpha) {

        return src + alpha * (dest - src);

    }

    inline void* 
    getVertex(uint32 vertex, VertexBuffer vertexbuffer) {
        uint8* vertices = (uint8*)vertexbuffer.vertices;
        return (void*)vertices[vertex * vertexbuffer.vertexsize]; 
    }

    inline void
    drawTriangles(VertexBuffer vertices, IndexBuffer indeces, RendererState* state) {

        //for(int i = ) 

    }
}