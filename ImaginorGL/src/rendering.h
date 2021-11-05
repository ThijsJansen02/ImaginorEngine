#pragma once
#include <core.h>
#include <intrinsics.h>
#include <LinearMath/mat3.h>
#include <LinearMath/vec2.h>
#include <LinearMath/vec3.h>
#include <LinearMath/ivec2.h>
#include <ImaginorPlatform/src/platform.h>

namespace IME::GL {

     struct ColorRGBA
    {
        real32 r;
        real32 g;
        real32 b;
        real32 a;
    };

    struct Vertex {
        vec3f pos;
        vec3f color;
    };

    struct BoundingBox {
        vec2f topleft;
        vec2f bottomright;
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

    void putPixel(ivec2 position, ColorRGBA color, Canvas canvas) {
        IME_DEBUG_ASSERT_BREAK(position.x < 0 || position.y <0 || position.x >= canvas.width || position.y >= canvas.height, "pixel is not in canvas!")
        canvas.data[position.x + position.y * canvas.width] = colorToColorFragment(color);
    }

    void putPixelSafe(ivec2 position, ColorRGBA color, Canvas canvas) {
        if(position.x < 0 || position.y < 0 || position.x >= canvas.width || position.y >= canvas.height) {
            return;
        }
        canvas.data[position.x + position.y * canvas.width] = colorToColorFragment(color);
    }

    

    BoundingBox
    calcBoundingBoxFromTriangle(vec2f v0, vec2f v1, vec2f v2) {

        BoundingBox result;
        result.topleft = v0;
        result.bottomright = v0;

        if(result.topleft.x > v1.x) { result.topleft.x = v1.x; }
        if(result.topleft.x > v2.x) { result.topleft.x = v2.x; }
        if(result.topleft.y < v1.y) { result.topleft.y = v1.y; }
        if(result.topleft.y < v2.y) { result.topleft.y = v2.y; }
        if(result.bottomright.x < v1.x) { result.bottomright.x = v1.x; }
        if(result.bottomright.x < v2.x) { result.bottomright.x = v2.x; }
        if(result.bottomright.y > v1.y) { result.bottomright.y = v1.y; }
        if(result.bottomright.y > v2.y) { result.bottomright.y = v2.y; }

        return result;
    }

    inline void 
    swap(vec2f* p1, vec2f* p2) {
        vec2 temp = *p1;
        *p1 = *p2;
        *p2 = temp;
    }

    real32
    edgeFunction(const vec2f &a, const vec2f &b, const vec2f &c) 
    { 
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x); 
    } 

    internal ColorRGBA 
    vec3ToColorRGBA(const vec3f& color) {
        return { color.x, color.y, color.z, 1.0f };
    }

    void drawTriangleFlatTop(vec2f v0, vec2f v1, vec2f v2, Canvas canvas) {

        IME_DEBUG_ASSERT_BREAK(v0.y == v1.y, "not a flat top triangle")
        IME_DEBUG_ASSERT_BREAK(v2.y >= v1.y, "not a flat triangle")

        if(v0.x > v1.x) swap(&v0, &v1);

        real32 m0 = (v2.x - v0.x) / (v2.y - v0.y);
        real32 m1 = (v2.x - v1.x) / (v2.y - v1.y);

        const int startY = ceilReal32(v0.y - 0.5f);
        const int endY = ceilReal32(v2.y - 0.5f);

        real32 A = edgeFunction(v0, v1, v2); 

        vec3f color0 = {1.0f, 0.0f, 0.0f};
        vec3f color1 = {0.0f, 1.0f, 0.0f};
        vec3f color2 = {0.0f, 0.0f, 1.0f};

        for(int32 y = startY; y < endY; y++) {

            const real32 px0 = m0 * (real32(y) + 0.5f - v0.y) + v0.x;
            const real32 px1 = m1 * (real32(y) + 0.5f - v1.y) + v1.x;

            const int32 startX = ceilReal32(px0 - 0.5f);
            const int32 endX = ceilReal32(px1 - 0.5f);

            for(int32 x = startX; x < endX; x++) {

                real32 w0 = edgeFunction(v1, v2, {(real32)x, (real32)y}) / A;
                real32 w1 = edgeFunction(v2, v0, {(real32)x, (real32)y}) / A;
                real32 w2 = edgeFunction(v0, v1, {(real32)x, (real32)y}) / A;

                putPixelSafe({x, y}, vec3ToColorRGBA(color0 * w0 + color1 * w1 + color2 + w2), canvas);
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

        real32 A = edgeFunction(v0, v1, v2); 

        vec3f color0 = {1.0f, 0.0f, 0.0f};
        vec3f color1 = {0.0f, 1.0f, 0.0f};
        vec3f color2 = {0.0f, 0.0f, 1.0f};

        for(int32 y = startY; y < endY; y++) {

            const real32 px0 = m0 * (real32(y) + 0.5f - v0.y) + v0.x;
            const real32 px1 = m1 * (real32(y) + 0.5f - v1.y) + v1.x;

            const int32 startX = ceilReal32(px0 - 0.5f);
            const int32 endX = ceilReal32(px1 - 0.5f);

            for(int32 x = startX; x < endX; x++) {

                real32 w0 = edgeFunction(v1, v2, {(real32)x, (real32)y}) / A;
                real32 w1 = edgeFunction(v2, v0, {(real32)x, (real32)y}) / A;
                real32 w2 = edgeFunction(v0, v1, {(real32)x, (real32)y}) / A;

                putPixelSafe({x, y}, vec3ToColorRGBA(color0 * w0 + color1 * w1 + color2 + w2), canvas);
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

    internal void 
    drawTriangle(Vertex v0, Vertex v1, Vertex v2, Canvas canvas) {

        vec2f v0r = {v0.pos.x, v0.pos.y};
        vec2f v1r = {v1.pos.x, v1.pos.y};
        vec2f v2r = {v2.pos.x, v2.pos.y};

        BoundingBox bounds = calcBoundingBoxFromTriangle(v0r, v1r, v2r);
        
        vec2<int32> bottomright;

        bottomright.x = ceilReal32(bounds.bottomright.x);
        bottomright.y = floorReal32(bounds.bottomright.y);

        vec2<int32> topleft;

        topleft.x = floorReal32(bounds.topleft.x);
        topleft.y = ceilReal32(bounds.topleft.y);

        real32 a = edgeFunction(v0r, v1r, v2r);

        for(int32 y = topleft.y; y > bottomright.y; y-- ) {
            for(int32 x = topleft.x; x <= bottomright.x; x++) {

                real32 w2 = edgeFunction(v0r, v1r, {(real32)x, (real32)y}) / a;
                real32 w0 = edgeFunction(v1r, v2r, {(real32)x, (real32)y}) / a;
                real32 w1 = edgeFunction(v2r, v0r, {(real32)x, (real32)y}) / a;

                if(w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    putPixelSafe(
                        {x, y}, 
                        vec3ToColorRGBA(w0 * v0.color + w1 * v1.color + w2 * v2.color),
                        canvas);
                }
            }
        }
    }

}