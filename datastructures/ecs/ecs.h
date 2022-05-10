#pragma once
#include <core.h>
#include <utility>

namespace IME::Data {

    typedef byte* allocator(sizeptr size);
    typedef void deallocator(sizeptr size, byte* data);

    #define COMPONENT() static constexpr sizeptr comp_index() {return __COUNTER__;} 
    struct Entity {
        uint32 index;
    };

    const real32 growthfactor = 1.5f;

    template<uint32 n_entities, uint32 n_components, allocator alloc, deallocator dealloc>
    struct Registry {

        struct ComponentPtr {
            uint32 index;
        };

        struct ComponentStorage {
            uint32 firstopenspot;
            uint32 count;
            uint32 capacity;
            void* data;
        };

        struct ComponentMap {
            ComponentPtr map[n_entities * n_components];
            inline void setStorageLocation(uint32 entity, uint32 component, uint32 location) {
                map[entity + component * n_entities].index = location;
            }

            inline uint32 getStorageLocation(uint32 entity, uint32 component) {
                return map[entity + component * n_entities].index;
            }
        };

        template<typename Comp>
        struct ComponentData {

            Comp component;
            Entity entity;
            bool32 used;
        };

        bool32 usedidentities[n_entities];
        uint32 firstunusedentity;
        ComponentMap map;
        ComponentStorage components[n_components];

        template<typename Comp1, typename Comp2>
        void forEachPair(void(* func)(Comp1*, Comp2*, Entity, void*), void* userptr) {

            ComponentStorage* storage1 = &components[Comp1::comp_index()];
            ComponentStorage* storage2 = &components[Comp2::comp_index()];

            ComponentData<Comp1>* data1 = (ComponentData<Comp1>*)storage1->data;
            ComponentData<Comp2>* data2 = (ComponentData<Comp2>*)storage2->data;

            for(uint32 i = 0; i < storage1->count; i++) {
                ComponentData<Comp1>* current1 = &data1[i];
                if(current1->used) {
                    uint32 component2index = map.getStorageLocation(current1->entity.index, Comp2::comp_index());
                    if(component2index == 0xFFFFFFFF) {continue;}

                    ComponentData<Comp2>* current2 = &data2[component2index];

                    func(&current1->component, &current2->component, current1->entity, userptr);
                }
            }
        }

        template<typename Comp1, typename Comp2, void(* func)(Comp1*, Comp2*, Entity, void*)>
        void forEachPair(void* userptr) {

            ComponentStorage* storage1 = &components[Comp1::comp_index()];
            ComponentStorage* storage2 = &components[Comp2::comp_index()];

            ComponentData<Comp1>* data1 = (ComponentData<Comp1>*)storage1->data;
            ComponentData<Comp2>* data2 = (ComponentData<Comp2>*)storage2->data;

            for(uint32 i = 0; i < storage1->count; i++) {
                ComponentData<Comp1>* current1 = &data1[i];
                if(current1->used) {
                    uint32 component2index = map.getStorageLocation(current1->entity.index, Comp2::comp_index());
                    if(component2index == 0xFFFFFFFF) {continue;}

                    ComponentData<Comp2>* current2 = &data2[component2index];

                    func(&current1->component, &current2->component, current1->entity, userptr);
                }
            }
        }

        template<typename Comp>
        struct View {
            struct iterator {
                iterator(ComponentData<Comp>* ptr) : m_Ptr(ptr) {}
                ComponentData<Comp>& operator*() {
                    return *m_Ptr;
                }

                bool32 operator==(iterator other) {
                    return this->m_Ptr == other.m_Ptr;
                }
                bool32 operator!=(iterator other) {
                    return this->m_Ptr != other.m_Ptr;
                }
                void operator--() {
                    m_Ptr--;
                }
                void operator++() {
                    m_Ptr++;
                }
                ComponentData<Comp>* m_Ptr;
            };
            iterator begin() {
                return iterator((ComponentData<Comp>*)parent->components[Comp::comp_index()].data);
            }
            iterator end() {
                return iterator(&((ComponentData<Comp>*)parent->components[Comp::comp_index()].data)[parent->components[Comp::comp_index()].count]);
            }
            Registry* parent;
        };

        void init() {
            memset(&map, 0xFFFFFFFF, sizeof(this->map));
            memset(usedidentities, 0x00000000, sizeof(usedidentities));
        }
    	
        template<typename Comp>
        View<Comp> view() {return {this}; }

        Entity createEntity() {
            usedidentities[firstunusedentity] = true;
            Entity result = { firstunusedentity };
            while((bool)usedidentities[++firstunusedentity] == true) { }

            return result;
        }

        template<typename Comp>
        bool32 hasComponent(Entity entity) {
            uint32 comp_index = Comp::comp_index();
            return map.getStorageLocation(entity.index, comp_index) != 0xFFFFFFFF;
        }

        template<typename Comp>
        Comp& getComponent(Entity entity) {
            uint32 index = Comp::comp_index();
            ComponentStorage* storage = &this->components[index];
            ComponentData<Comp>* data = (ComponentData<Comp>*)storage->data;
            return data[map.getStorageLocation(entity.index, index)].component;
        }

        template<typename Comp>
        void addComponent(Entity entity, const Comp& component) {
            uint32 index = component.comp_index();
            IME_DEBUG_ASSERT_BREAK(index < n_components, "this index is not supported!")

            ComponentStorage* storage = &this->components[index];

            if(storage->count == storage->firstopenspot) {
                storage->count++;
                if(storage->count > storage->capacity) {
                    uint32 newcapacity = (uint32)((real32)storage->count * growthfactor);
                    void* newdata = alloc(sizeof(ComponentData<Comp>) * newcapacity);
                    if(storage->data) {
                        IME::copy((byte*)storage->data, (byte*)newdata, sizeof(ComponentData<Comp>) * storage->count - 1);
                        dealloc(storage->capacity * sizeof(ComponentData<Comp>), (byte*)storage->data);
                    }

                    storage->data = newdata;
                    storage->capacity = newcapacity;
                }
            }

            ComponentData<Comp>* data = (ComponentData<Comp>*)storage->data;
            data[storage->firstopenspot] = ComponentData<Comp>{ component, entity.index, true };
            map.setStorageLocation(entity.index, Comp::comp_index(), storage->firstopenspot);

            while((bool)data[++storage->firstopenspot].used == true && storage->firstopenspot < storage->count) { }
        }

        template<typename comp>
        void removeComponent(Entity entity) {
            IME_DEBUG_ASSERT_BREAK(index < n_components, "this index is not supported!")

            uint32 index = comp.comp_index();
            ComponentStorage* storage = &this->components[index];
            ComponentData<comp>* data = (ComponentData<Comp>*)storage->data;

            uint32 componentlocation = map.getStorageLocation(entity.index, index);
            data[componentlocation].used = false;
            if(componentlocation < storage->firstopenspot) {
                storage->firstopenspot = componentlocation;
            }
        }   
 

    };
}