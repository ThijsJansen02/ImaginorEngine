#include <core.h>
#include "LinearMath/vec2.h"
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include "rendering.h"
#include "tilemaps.h"
#include "memory.h"
#include "LinearMath/matrix.h"

namespace IME
{
    struct BoundingBox {
        union {
            struct {
                vec2f bottomleft;
                vec2f topright;
            };
            struct {
                real32 left;
                real32 bottom;
                real32 right;
                real32 top;
            };
        };
    };

    struct Camera2D {
        BoundingBox clippingspace = {};
        mat3 projection;
        WorldPosition worldposition;
        real32 rotation;
    };

    struct EngineState
    {
        real32 zoom;
        MemoryArena persistentstorage;
        MemoryArena transientstorage;

        MemoryPool mainmemory;

        Camera2D camera;

        ImageBuffer image;
        ImageBuffer hatimage;
    };

    #pragma pack(push, 1)
    struct BitmapHeader {

        //header
        char signature[2];
        uint32 filesize;
        uint32 reserved;
        uint32 dataoffset;

        //infoheader
        uint32 infoheadersize;
        uint32 width;
        uint32 height;
        uint16 planes;
        uint16 bitsperpixel;
        uint32 compression;
        uint32 imagesize;
    };
    #pragma pack(pop)

    ImageBuffer DEBUGloadImageBuffer(const char* filename, ApplicationMemory memory, MemoryArena* arena) {

        FileBuffer file = memory.debug_readfile(filename, nullptr);
        if (!file.data)
        {
            return {};
        }

        BitmapHeader header = *((BitmapHeader*)file.data);
        ImageBuffer result = {};

        result.data = pushArray(arena, ColorFragment, header.height * header.width);
        result.width = header.width;
        result.height = header.height;

        ColorFragment* colordata = (ColorFragment*)((uint8*)file.data + header.dataoffset); 

        for(uint32 y = 0; y < result.height; y++) {
            for(uint32 x = 0; x < result.width; x++) {
                result.data[x + y * result.width] = colordata[x + (result.height - y - 1) * result.width];
            }
        }
        return result;
    }

    inline bool32 
    isInRange(real32 test, real32 min, real32 max) {
        return test >= min && test <= max;

    }

    BoundingBox CalculateIntersection(const BoundingBox& box1, const BoundingBox& box2) {

        BoundingBox result = {};
        
        if(isInRange(box1.left, box2.left, box2.right)) { result.left = box1.left; }
        else if (isInRange(box2.left, box1.left, box1.right)) { result.left = box2.left; }
        else { return {}; }

        if(isInRange(box1.right, box2.left, box2.right)) { result.right = box1.right; }
        else if (isInRange(box2.right, box1.left, box1.right)) { result.right = box2.right; }
        else { return {}; }

        if(isInRange(box1.bottom, box2.bottom, box2.top)) { result.bottom = box1.bottom; }
        else if (isInRange(box2.bottom, box1.bottom, box1.top)) { result.bottom = box2.bottom; }
        else { return {}; }

        if(isInRange(box1.top, box2.bottom, box2.top)) { result.top = box1.top; }
        else if (isInRange(box2.top, box1.bottom, box1.top)) { result.top = box2.top; }
        else { return {}; }

        return result;
    }

    global_var ImageBuffer image;

    void drawTilesInClipSpace(const TileWorld& tileworld, const Camera2D& camera, const mat3& rastermatrix, Canvas canvas) {

        uint32 loadedmapdim = tileworld.renderdistance * 2 + 1;
        vec2 tilechunkdimensions = { tileworld.tilewidth * tileworld.chunkdim, tileworld.tileheight * tileworld.chunkdim };
        
        BoundingBox absoluteCameraBounds = {};
        absoluteCameraBounds.bottomleft = camera.clippingspace.bottomleft + camera.worldposition.relativeposition;
        absoluteCameraBounds.topright = camera.clippingspace.topright + camera.worldposition.relativeposition;

        mat3 rasterviewprojection = rastermatrix * camera.projection * translateMat3(-1.0f * camera.worldposition.relativeposition);

        int32 chunkradius = (int32)floorf((camera.clippingspace.right / tilechunkdimensions.x) + 1.0f);

        for (uint32 y = (uint32)camera.worldposition.chunkposY - chunkradius; y != (uint32)camera.worldposition.chunkposY + chunkradius + 1; y++)
        {
            for(uint32 x = (uint32)camera.worldposition.chunkposX - chunkradius; x != (uint32)camera.worldposition.chunkposX + chunkradius + 1; x++) {
                
                TileChunk chunk = getTileChunk(x, y, tileworld);
                if(!chunk.data) {
                    continue;
                }

                int32 relativechunkX = (int32)x - camera.worldposition.chunkposX;
                int32 relativechunkY = (int32)y - camera.worldposition.chunkposY;

                BoundingBox absoluteTileBounds = {};
                absoluteTileBounds.bottomleft = vec2f(relativechunkX, relativechunkY) * tilechunkdimensions;
                absoluteTileBounds.topright = vec2f((real32)relativechunkX + 1.0f, (real32)relativechunkY + 1.0f) * tilechunkdimensions;

                BoundingBox overlap = CalculateIntersection(absoluteTileBounds, absoluteCameraBounds);
                overlap.bottomleft = overlap.bottomleft - vec2(relativechunkX * tilechunkdimensions.x, relativechunkY * tilechunkdimensions.y);
                overlap.topright = overlap.topright - vec2(relativechunkX * tilechunkdimensions.x, relativechunkY * tilechunkdimensions.y);

                drawTileChunkRegion(overlap.bottomleft,
                                    overlap.topright,
                                    canvas,
                                    chunk,
                                    rasterviewprojection * translateMat3(vec2f{relativechunkX * tilechunkdimensions.x,
                                        	                                relativechunkY * tilechunkdimensions.y}), image);
            }
        }
    }

    bool32 
    setTileValue(uint32 chunkX, uint32 chunkY, uint32 tileX, uint32 tileY, uint32 value, TileWorld* world, MemoryArena* memory) {

        TileChunk chunk = getTileChunk(chunkX, chunkY, *world);
        if(chunk.data == nullptr) {
            chunk.data = pushArray(memory, uint32, world->chunkdim * world->chunkdim);
            chunk.height = world->chunkdim;
            chunk.width = world->chunkdim;
            chunk.tileheight = world->tileheight;
            chunk.tilewidth = world->tilewidth;
            if(!setTileChunk(chunkX, chunkY, chunk, *world)) {
                //TODO: make sure to release the memory from the arena and add debug output!;
                return false;
            }
        }
        return setTileID(tileX, tileY, chunk, value);
    }

    uint32
    getTileValue(uint32 chunkX, uint32 chunkY, uint32 tileX, uint32 tileY, const TileWorld& world) {

        TileChunk chunk = getTileChunk(chunkX, chunkY, world);
        if(chunk.data == nullptr) {
            return 0;
        }
        return getTileID(tileX, tileY, chunk);
    }   

    void
    generateRoom(uint32 xo, uint32 yo, TileWorld world, MemoryArena* memory) {
        
        uint32 roomwidth = 15;
        uint32 roomheight = 10;
        for(uint32 y = 0; y < roomheight; y++) {
            for(uint32 x = 0; x < roomwidth; x++) {
                if(x == 0 || x == roomwidth -1 || y == 0 || y == roomheight -1) {
                    setTileValue((xo + x) / world.chunkdim, (yo + y) / world.chunkdim, (xo + x) % world.chunkdim, (yo + y) % world.chunkdim, 2, &world, memory);    
                } else {
                    setTileValue((xo + x) / world.chunkdim, (yo + y) / world.chunkdim, (xo + x) % world.chunkdim, (yo + y) % world.chunkdim, 1, &world, memory);
                }

            }
        }
    }

    bool32 
    isWorldPositionEmpty(WorldPosition position, const TileWorld& world) {

        uint32 tilevalue = getTileValue(
            position.chunkposX, 
            position.chunkposY, 
            (uint32)position.relativeposition.x, 
            (uint32)position.relativeposition.y, 
            world);

        if(tilevalue == 2) {
            return false;
        } else {
            return true;
        }
    }

    vec2f vertices[4] {
        { -0.5f,  0.5f }, { 0.5f,  0.5f },
        {  0.5f,  -0.5f }, { -0.5f, -0.5f }
    };

    global_var MemoryPool* mainmemory;
    byte* allocator(sizeptr size) {
        return (byte*)allocateMemory_(mainmemory, size);
    }

    void deallocator(sizeptr size, byte* data) {
        deallocateMemory_(mainmemory, data, size);
        return;
    }

    extern "C" IME_APPLICATION_INIT(applicationInit)
    {
        EngineState *gamestate = (EngineState *)memory.persistentstorage;
        mainmemory = &gamestate->mainmemory;

        gamestate->zoom = 1.0f;

        //create memory arena;
        gamestate->persistentstorage.base = (byte*)memory.persistentstorage + sizeof(EngineState);
        gamestate->persistentstorage.size = memory.persistentstoragesize - sizeof(EngineState);
        gamestate->persistentstorage.used = 0;

        gamestate->transientstorage.base = (byte*)memory.transientstorage;
        gamestate->transientstorage.size = memory.transientstoragesize;
        gamestate->transientstorage.used = 0;

        gamestate->mainmemory = createMemoryPool(&gamestate->transientstorage, gamestate->transientstorage.size, 1024);

        IME::Data::DynamicArray_<uint32, allocator, deallocator> array;
        array.resize(10);
        array.resize(100);
    
        gamestate->image = DEBUGloadImageBuffer("flowerbed.bmp", memory, &gamestate->persistentstorage);
        image = gamestate->image;

        gamestate->hatimage = DEBUGloadImageBuffer("hat32x32.bmp", memory, &gamestate->persistentstorage);

        Camera2D camera;

        real32 width = 16.0f;
        real32 height = width * (9.0f / 16.0f);

        real32 left = -width / 2.0f;
        real32 right = width / 2.0f;
        real32 bottom = -height / 2.0f; 
        real32 top = height / 2.0f;

        camera.projection = createOrthographicMat3(left, right, bottom, top);
        camera.clippingspace.bottomleft = {left - 1.0f, bottom - 1.0f};
        camera.clippingspace.topright = { right + 1.0f, top + 1.0f };
        camera.worldposition.relativeposition = {0.0f, 0.0f};
        camera.rotation = 0.0f;

        gamestate->camera = camera;
        return true;
    }


    extern "C" IME_APPLICATION_UPDATE(applicationUpdate)
    {
        EngineState *gamestate = (EngineState *)memory.persistentstorage;

        clearCanvas(canvas, {200, 20, 200, 0});

        Camera2D camera = gamestate->camera;

        //create the rasterprojection matrix;
        IME::mat3 torasterposition = transformMat3(vec2f{ canvas.width / 2.0f, -(canvas.height / 2.0f) }, vec2f{ canvas.width / 2.0f, canvas.height / 2.0f });
        IME::mat3 projection = camera.projection;
        IME::mat3 relativeview = translateMat3(-1.0f * camera.worldposition.relativeposition);
        IME::mat3 rasterprojection = torasterposition * projection;
        IME::mat3 viewprojectionraster = rasterprojection * relativeview;

        //drawTilesInClipSpace(gamestate->tileworld, camera, torasterposition, canvas);

        drawQuad(rasterprojection * (vec2(-0.8f, 1.8f)), rasterprojection * (vec2(0.8f, -1.8f)), canvas, {1.0f, 1.0f, 0.0f, 1.0f});
        drawQuad(rasterprojection * (vec2(-0.8f, 1.8f)), rasterprojection * (vec2(0.8f, -1.8f)), canvas, gamestate->hatimage);

        d_drawTriangle(vec2f{100, 100}, vec2f{300, 150}, vec2f{200, 400}, canvas);
        //drawTriangleFlatBottom({100, 400}, {400, 400}, {300, 100}, canvas);
        //drawTriangleFlatTop({100, 100}, {600, 100}, {300, 400}, canvas);
        //drawQuad(rasterprojection * vec2{-4.0f, -4.0f}, rasterprojection * vec2{4.0f, 4.0f}, canvas, gamestate->hatimage);
        
        gamestate->camera = camera;
        return true;
    }
}
