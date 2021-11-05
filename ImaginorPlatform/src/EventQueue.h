#pragma once
#define EVENTQUEUE_SIZE 16
#include <datastructures/lists/arrays/LocalCircularArrayList.h>
#include "platform.h"

namespace IME {

    inline Event copyEvent(Event event) {
        if(event.type == IME_DEBUG_MESSAGE) {
            Event result = event;
            sizeptr len = strlen((char*)event.param2);
            if(len > 0) {
                result.param2 = (uint64)strcpy((char*)(void*)malloc(len + 1), (char*)event.param2);
            }
            return result;
        }
        return event;
    }

    inline void deleteEvent(Event* event) {
        if(event->type == IME_DEBUG_MESSAGE) {
            free((void*)event->param2);
        }
        *event = {};
    }

    struct EventQueue {

        Data::LocalCircularArrayList<Event, 24> output;
        Data::LocalCircularArrayList<Event, 24> input;

        sizeptr poppedevents;
        sizeptr availableevents;
    };

}