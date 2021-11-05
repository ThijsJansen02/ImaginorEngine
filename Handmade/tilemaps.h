#pragma once
#include "core.h"
#include "rendering.h"

namespace IME {

    struct TileChunk {
        uint32 width;
        uint32 height;

        real32 tilewidth;
        real32 tileheight;

        uint32* data;
    };

    struct TileWorld
    {
        uint32 width = 0;
        uint32 height = 0;

        real32 tileheight = 1.0f;
        real32 tilewidth = 1.0f;

        uint32 chunkdim = 0;

        uint32 renderdistance = 0;
        uint32 loadedchunkscount = 0;
        TileChunk *loadedchunks = nullptr;
    };

    struct WorldPosition
    {
        uint32 chunkposX = 0;
        uint32 chunkposY = 0;

        vec2f relativeposition = {0.0f, 0.0f};
    };

    inline uint32
    getTileID(uint32 x, uint32 y, TileChunk tilechunk)
    {
        return tilechunk.data[x + (tilechunk.height - y - 1) * tilechunk.width];
    }

    inline bool32
    setTileID(uint32 x, uint32 y, TileChunk tilechunk, uint32 value)
    {
        tilechunk.data[x + (tilechunk.height - y - 1) * tilechunk.width] = value;
        return true;
    }

    inline TileChunk
    getTileChunk(uint32 chunkX, uint32 chunkY, const TileWorld &world)
    {
        if (chunkX >= world.width || chunkY >= world.height)
        {
            TileChunk chunk;
            chunk.height = 0;
            chunk.width = 0;
            chunk.data = nullptr;
            return chunk;
        }
        return world.loadedchunks[chunkX + (world.height - chunkY - 1) * world.width];
    }

    inline bool32
    setTileChunk(uint32 chunkX, uint32 chunkY, const TileChunk& chunk, const TileWorld& world) {
        if (chunkX >= world.width || chunkY >= world.height)
        {
            return false;
        }
        world.loadedchunks[chunkX + (world.height - chunkY - 1) * world.width] = chunk;
        return true;
    }   

    void
    drawTileChunkRegion(uint32 x1, uint32 y1, uint32 x2, uint32 y2, Canvas canvas, TileChunk chunk, const mat3 &rasterprojectionview, const ImageBuffer& image)
    {

        if (x2 > chunk.width || y2 > chunk.height)
        {
            return;
        }

        for (int32 y = y1; y < y2; y++)
        {
            for (int32 x = x1; x < x2; x++)
            {
                vec2 pos1 = rasterprojectionview * vec2((x)*chunk.tilewidth, (y)*chunk.tileheight);
                vec2 pos2 = rasterprojectionview * vec2((x + 1.0f) * chunk.tilewidth, (y + 1.0f) * chunk.tileheight);

                if (getTileID(x, y, chunk) == 0)
                {
                    drawQuad(pos1, pos2, canvas, {0.0f, 0.0f, 0.0f, 1.0f});
                }
                if (getTileID(x, y, chunk) == 1)
                {
                    drawQuad(pos1, pos2, canvas, image);
                }
                if (getTileID(x, y, chunk) == 2)
                {
                    drawQuad(pos1, pos2, canvas, { 0.3f, 0.3f, 0.3f, 1.0f});
                }
            }
        }
    }

    void drawTileChunkRegion(vec2 bottomleft, vec2 topright, Canvas canvas, TileChunk chunk, const mat3 &transform, const ImageBuffer& image)
    {
        bottomleft.x /= chunk.tilewidth;
        bottomleft.y /= chunk.tileheight;

        topright.x /= chunk.tilewidth;
        topright.y /= chunk.tileheight;

        drawTileChunkRegion(roundf(bottomleft.x), roundf(bottomleft.y), roundf(topright.x), roundf(topright.y), canvas, chunk, transform, image);
    }

    void
    drawTileChunck(TileChunk chunk, Canvas canvas, const mat3 &transform)
    {
        for (int32 y = 0; y < chunk.height; y++)
        {
            for (int32 x = 0; x < chunk.width; x++)
            {
                vec2 pos1 = transform * vec2((x)*chunk.tilewidth, (y)*chunk.tileheight);
                vec2 pos2 = transform * vec2((x + 1.0f) * chunk.tilewidth, (y + 1.0f) * chunk.tileheight);

                if (getTileID(x, y, chunk) == 0)
                {
                    drawQuad(pos1, pos2, canvas, {1.0f, 1.0f, 1.0f, 1.0f});
                }
                if (getTileID(x, y, chunk) == 1)
                {
                    drawQuad(pos1, pos2, canvas, {0.3f, 0.3f, 0.3f, 1.0f});
                }
            }
        }
    }

    WorldPosition recalculateWorldPosition(WorldPosition position, const TileWorld &world)
    {
        if (position.relativeposition.x <= 0.0f)
        {
            position.relativeposition.x += world.chunkdim * world.tilewidth;
            position.chunkposX--;
        }
        if (position.relativeposition.y <= 0.0f)
        {
            position.relativeposition.y += world.chunkdim * world.tileheight;
            position.chunkposY--;
        }
        if (position.relativeposition.x > world.chunkdim * world.tilewidth)
        {
            position.relativeposition.x -= world.chunkdim * world.tilewidth;
            position.chunkposX++;
        }
        if (position.relativeposition.y > world.chunkdim * world.tileheight)
        {
            position.relativeposition.y -= world.chunkdim * world.tileheight;
            position.chunkposY++;
        }

        return position;
    }

    uint32
    isTileEmpty(vec2 position, TileChunk chunk)
    {

        uint32 x = position.x / chunk.tilewidth;
        uint32 y = position.y / chunk.tileheight;

        if (getTileID(x, y, chunk) == 0)
        {
            return true;
        }
        return false;
    }
}