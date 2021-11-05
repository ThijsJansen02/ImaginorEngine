#include <core.h>
#include "LinearMath/vec2.h"
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include "rendering.h"
#include "memory.h"
#include "LinearMath/mat3.h"
#include "LinearMath/vec3.h"

namespace IME
{
    struct Vertex {
        vec2f pos;
        vec3f color;
    };

    struct Plane {
        vec3f topleft;
        vec3f bottomright;
    };

    struct Sphere {
        vec3f position;
        real32 radius;
    };

    struct Ray {
        vec3f origin;
        vec3f direction;
    };

    struct Camera {
        float focallength;
        vec2f aperture;
        float nearclippingplane;
        float farclippingplane;
        vec2<uint32> outputresulution;
    };

    struct BoundingBox {
        vec2f topleft;
        vec2f bottomright;
    };

    vec3f points[8] =  {
            { 1, -1, -5}, 
            { 1, -1, -3}, 
            { 1,  1, -5}, 
            { 1,  1, -3}, 
            {-1, -1, -5}, 
            {-1, -1, -3}, 
            {-1,  1, -5}, 
            {-1,  1, -3} 
        };

    global_var vec3f cameraorigin = {0.0f, 0.0f, 0.0f }; 
    
    global_var Sphere sphere = { {0.0f, 0.0f, 20.0f}, 1.0f }; 
    global_var Plane raster = { {-0.5f, 0.5f, 1.0f }, { 0.5f, -0.5f, 1.0f } };

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

    real32
    edgeFunction(const vec2f &a, const vec2f &b, const vec2f &c) 
    { 
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x); 
    } 

    bool32 
    calcSphereRayCollision(Sphere sphere, Ray ray) {
        real32 alpha = dotProduct(ray.direction, sphere.position - ray.origin);
        vec3f intersect = (alpha * ray.direction) + ray.origin;

        vec3f intersectsphere = intersect - sphere.position;
        real32 dsquared = intersectsphere.x * intersectsphere.x + intersectsphere.y * intersectsphere.y + intersectsphere.z * intersectsphere.z; 
        
        return dsquared < (sphere.radius * sphere.radius);
    }

    internal ColorRGBA 
    vec3ToColorRGBA(const vec3f& color) {
        return { color.x, color.y, color.z, 1.0f };
    }

    internal ColorRGBA
    operator+(const ColorRGBA& left, const ColorRGBA& right) {
        return {left.r + right.r, left.g + right.g, left.b + right.b, left.a + right.a };
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

    extern "C" IME_APPLICATION_INIT(applicationInit) {
        
        
        return true;
    }

    extern "C" IME_APPLICATION_UPDATE(applicationUpdate)
    {   

        clearCanvas(canvas, {0, 0, 0, 1});

        mat3 toraster = transformMat3(vec2f{ canvas.width / 2.0f, -(canvas.height / 2.0f) }, vec2f{ canvas.width / 2.0f, canvas.height / 2.0f });
        mat3 transform = transformMat3({1.0f, 1.0f}, {0.0f, 0.0f});
#if 1
        for(int i = 0; i < 1; i++) {
            drawTriangle(
                { toraster * transform * vec2f{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, 
                { toraster * transform * vec2f{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, 
                { toraster * transform * vec2f{ 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}}, canvas);
        }

#else
        real32 alphaX = absolute(raster.topleft.x - raster.bottomright.x) / (real32)canvas.width;
        real32 alphaY = absolute(raster.topleft.y - raster.bottomright.y) / (real32)canvas.height;

        for(uint32 y = 0; y < canvas.height; y++) {
            for(uint32 x = 0; x < canvas.width; x++) {
                
                vec3f rasterposition = { raster.topleft.x + x * alphaX + 0.5f * alphaX, raster.topleft.y - y * alphaY - 0.5f * alphaY, 1.0f };
                Ray ray = {0}; 
                ray.direction = normalize(rasterposition);
                ray.origin = cameraorigin;

                if(calcSphereRayCollision(sphere, ray)) {
                    putPixelSafe({(int32)x, (int32)y}, { 1.0f, 1.0f, 1.0f, 1.0f }, canvas);
                }
            }
        }
#endif  
        return true;
    }
}
