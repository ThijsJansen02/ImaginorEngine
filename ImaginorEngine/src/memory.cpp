#include "core.h"
#include "string.h"
#include "memory.h"

namespace IME {

    namespace Memory {

        static MemoryPool* globalmemory;
        void setGlobal(MemoryPool* memory) {
            globalmemory = memory;
        }

        byte* alloc(sizeptr size) {
            IME_DEBUG_ASSERT_BREAK(globalmemory, "global memory not yet set!")
            return (byte*)allocateMemory_(globalmemory, size);
        }

        void dealloc(sizeptr size, byte* data) {
            IME_DEBUG_ASSERT_BREAK(globalmemory, "global memory not yet set!")
            deallocateMemory_(globalmemory, data, size);
        }
    }

    void *
    pushmemory_(MemoryArena* arena, sizeptr datasize) {
        IME_DEBUG_ASSERT_BREAK(arena->used + datasize <= arena->size, "arena doesnt have enough memory")

        void* result = arena->base + arena->used;
        arena->used += datasize;
        return result;
    }

    MemoryPool 
    createMemoryPool(MemoryArena* arena, sizeptr size, uint32 maxpoolchunkcount) {
        MemoryPool result = {};
        result.maxpoolchunkcount = maxpoolchunkcount;
        result.poolchunkcount = 0;
        result.largestpoolchunk = 0;
        result.pool = pushArray(arena, PoolChunk, result.maxpoolchunkcount);

        result.size = size - (maxpoolchunkcount * sizeof(PoolChunk));
        result.used = 0;
        result.base = (byte*)pushmemory_(arena, result.size);

        return result;
    }

    /**
     * returns the index where the new value should be inserted
     * 
     * 
     * 
    template<typename T,  int32(*comp)(const T& left, const T& right)>
    uint32 binaryIndexSearch(T* array, uint32 count, const T& compare) {

        T* current = array;
        sizeptr currentcount = count;

        while(true) {
            uint32 index = currentcount / 2;
            int32 result = comp(current[index], compare));
            if(result > 0) {
                currentcount = index;
                continue;
            } 
            if (result < 0) {
                currentcount = currentcount - index; 
            }
            if (result == 0) {

            }
        }
    }
     */

    template<typename T> 
    void 
    swap(T* val1, T* val2) {
        T temp = *val1;
        *val1 = *val2;
        *val2 = temp;
    }

    //inserting a poolchunk in its correct position
    void insertPoolChunk(PoolChunk* pool, PoolChunk chunk, uint32* poolsize) {
        for(uint32 i = 0; i < *poolsize; i++) {
            PoolChunk* current = &pool[i];
            if(current->size > chunk.size) {
                for(uint32 j = *poolsize; j > i; j--) {
                    pool[j] = pool[j - 1];
                }
                pool[i] = chunk;
                *poolsize += 1;
                return;
            }
        }
        //if the new chunk is larger than all other poolchunks insert at the last place
        pool[*poolsize] = chunk;
        *poolsize += 1;
    }

    void 
    removePoolChunk(PoolChunk* pool, uint32 index, uint32* poolsize) {
        for(int i = index + 1; i < *poolsize; i++) {
            pool[i - 1] = pool[i];
        }
        *poolsize -= 1;
        pool[*poolsize] = {};
        return;
    }

    void
    replaceSmallerPoolChunk(PoolChunk* pool, uint32 index, PoolChunk chunk, uint32 poolsize) {
        for(int32 i = index - 1; i >= 0; i--) {
            PoolChunk* current = &pool[i];
            if(current->size > chunk.size) {
                pool[i + 1] = *current;
                continue;
            } else {
                pool[i + 1] = chunk;
                return;
            }
        }
        pool[0] = chunk;
    }

    void 
    replaceLargerPoolChunk(PoolChunk* pool, uint32 index, PoolChunk chunk, uint32 poolsize) {
        for(uint32 i = index + 1; i < poolsize; i++) {
            PoolChunk* current = &pool[i];
            if(current->size < chunk.size) {
                pool[i - 1] = *current;
                continue;
            } else {
                pool[i - 1] = chunk;
                return;
            }
        }
        pool[poolsize - 1] = chunk;
    }

    #define allocateStruct(memorypool, struct) (struct*)allocateMemory_(memorypool, sizeof(struct))
    #define allocateArray(memorypool, struct, count) (struct*)allocateMemory_(memorypool, sizeof(struct) * count)

    #define deallocateStruct(memorypool, data, struct) deallocateMemory_(memorypool, (byte*)data, sizeof(struct))
    #define deallocateArray(memorypool, data, struct, count) deallocateMemory_(memorypool, (byte*)data, sizeof(struct) * count)

    #define NULL_INDEX 0xFFFFFFFF

    void
    deallocateMemory_(MemoryPool* pool, byte* data, sizeptr size) {

        PoolChunk newpoolchunk = {};
        newpoolchunk.base = data;
        newpoolchunk.size = size;

        uint32 beforeindex = NULL_INDEX;
        uint32 afterindex = NULL_INDEX;

        memset(data, 0, size);

        //finding if there are any chunks adjacent to the this chunk
        for(int i = 0; i < pool->poolchunkcount; i++) {
            PoolChunk* current = &pool->pool[i];
            if(current->base == newpoolchunk.base + size) {
                afterindex = i;
            }
            if(current->base + current->size == newpoolchunk.base) {
                beforeindex = i;
            }
        }

        //calculating the new pool in case of no adjecent chunks
        if(beforeindex == NULL_INDEX && afterindex == NULL_INDEX) {
            if (newpoolchunk.base + size >= pool->base + pool->used) {
                pool->used -= newpoolchunk.size;
            } else {
                IME_DEBUG_ASSERT_BREAK(pool->poolchunkcount + 1 <= pool->maxpoolchunkcount, "memory pool to fragmentated!")
                insertPoolChunk(pool->pool, newpoolchunk, &pool->poolchunkcount);
            }
        }

        //calculating the new pool in case of a chunk after the new chunk
        else if(beforeindex != NULL_INDEX && afterindex == NULL_INDEX) {
            PoolChunk before = pool->pool[beforeindex];
            before.size += newpoolchunk.size;

            if(before.base + before.size >= pool->base + pool->used) {
                pool->used -= before.size;
                removePoolChunk(pool->pool, beforeindex, &pool->poolchunkcount);
            } else {
                replaceLargerPoolChunk(pool->pool, beforeindex, before, pool->poolchunkcount);
            }
        }

        //calculating the new pool in case of a chunk befor the new chunk
        else if(beforeindex == NULL_INDEX && afterindex != NULL_INDEX) {
            PoolChunk after = pool->pool[afterindex];
            after.size += newpoolchunk.size;
            after.base = newpoolchunk.base;

            if(after.base + after.size >= pool->base + pool->used) {
                pool->used -= after.size;
                removePoolChunk(pool->pool, afterindex, &pool->poolchunkcount);
            } else {
                replaceLargerPoolChunk(pool->pool, afterindex, after, pool->poolchunkcount);
            }
        } 
        
        //calculating new pool in case of two adjecent chunks
        else if(beforeindex != NULL_INDEX && afterindex != NULL_INDEX) {
            PoolChunk after = pool->pool[afterindex];
            PoolChunk before = pool->pool[beforeindex];
            before.size += newpoolchunk.size + after.size;

            if(before.base + before.size >= pool->base + pool->used) {
                pool->used -= before.size;
                removePoolChunk(pool->pool, afterindex, &pool->poolchunkcount);
                removePoolChunk(pool->pool, beforeindex, &pool->poolchunkcount);
            } else {
                removePoolChunk(pool->pool, afterindex, &pool->poolchunkcount);
                replaceLargerPoolChunk(pool->pool, beforeindex, before, pool->poolchunkcount);
            }
        }

        if(pool->poolchunkcount > 0) {
            pool->largestpoolchunk = pool->pool[pool->poolchunkcount - 1].size;
        } else {
            pool->largestpoolchunk = 0;
        }
    }

    void*
    allocateMemory_(MemoryPool* pool, sizeptr datasize) {
        if(pool->largestpoolchunk < datasize) {
            void* result = (byte*)pool->base + pool->used;
            pool->used += datasize;
            IME_DEBUG_ASSERT_BREAK(pool->used < pool->size, "")
            return result;
        } else {
            for(uint32 i = 0; i < pool->poolchunkcount; i++) {
                PoolChunk current = pool->pool[i];
                if(current.size >= datasize) {
                    void* result = current.base;

                    current.base += datasize;
                    current.size -= datasize;
                    if(current.size == 0 ) {
                        //remove instance from the pool
                        removePoolChunk(pool->pool, i, &pool->poolchunkcount);

                        if(pool->poolchunkcount > 0) {
                            pool->largestpoolchunk = pool->pool[pool->poolchunkcount - 1].size;
                        } else {
                            pool->largestpoolchunk = 0;
                        }
                    } else {
                        //find correct place for smaller poolchunk
                        replaceSmallerPoolChunk(pool->pool, i, current, pool->poolchunkcount);
                        pool->largestpoolchunk = pool->pool[pool->poolchunkcount - 1].size;
                    }
                    return result;
                } else {
                    continue;
                }
            }
            //largest poolchunk is incorrect
            IME_DEBUG_BREAK();
            return nullptr;
        }
    }


    

}