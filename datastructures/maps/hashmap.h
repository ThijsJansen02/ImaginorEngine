#pragma once
#include <core.h>
#include "..\lists\linkedlists\DoublyLinkedList.h"
#include "..\lists\arrays\basetypes\Array.h"

namespace IME::Data {


    template<
        typename key, 
        typename value, 
        sizeptr mapsize, 
        sizeptr(*hashfunc)(const key& key), 
        byte *(*allocator)(sizeptr size), 
        void (*deallocator)(sizeptr size, byte * data)
    >
    class HashMap_ {
    public:

        struct KeyValuePair {
            key k;
            value v;
        };

        void init() {
            for(auto& map : hashtable) {
                map.init();
            }
        }


        Data::Array<DoublyLinkedList_<KeyValuePair, allocator, deallocator>, mapsize> hashtable;

        KeyValuePair* add(const key& key, const value& value) {
            sizeptr index = hashfunc(key) % mapsize;
            hashtable[index].push_front({key, value});
            return &hashtable[index].m_Head->getValue();
        }

        KeyValuePair* get(const key& key) {
            sizeptr index = hashfunc(key) % mapsize;
            DoublyLinkedList_<KeyValuePair, allocator, deallocator> list = hashtable[index];

            for(auto node : list) {
                if(node->getValue().k == key) {
                    return &node->getValue();
                }
            }

            return nullptr;
        }
    };



}