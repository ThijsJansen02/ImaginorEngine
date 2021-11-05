#pragma once
#include <core.h>

namespace IME {

    struct MemoryArena
    {
        byte *base = nullptr;
        sizeptr size = 0;
        sizeptr used = 0;
    };

    struct PoolChunk {
        byte* base;
        uint32 size;
    };

    struct MemoryPool {
        
        uint32 maxpoolchunkcount;
        uint32 poolchunkcount;
        PoolChunk* pool;

        uint32 largestpoolchunk;

        sizeptr size;
        sizeptr used;
        byte* base;
    };

    void *
    pushmemory_(MemoryArena* arena, sizeptr datasize);

    void
    deallocateMemory_(MemoryPool* pool, byte* data, sizeptr size);
    void*
    allocateMemory_(MemoryPool* pool, sizeptr datasize);

    namespace Memory {
        byte* alloc(sizeptr size);
        void dealloc(sizeptr size, byte* data);
        void setGlobal(MemoryPool* memory);
    }

    MemoryPool 
    createMemoryPool(MemoryArena* arena, sizeptr size, uint32 maxpoolchunkcount);

    #define pushStruct(arena, struct) (struct*)pushmemory_(arena, sizeof(struct))
    #define pushArray(arena, struct, count) (struct*)pushmemory_(arena, sizeof(struct) * count)
}