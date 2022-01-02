#pragma once 
#include <ImaginorMath/src/LinearMath/linearmath.h>

namespace IME::UI {

    struct Bounds {
        union {
            struct {
                IME::vec2f topleft;
                IME::vec2f bottomright;
            };
            struct {
                IME::real32 left;
                IME::real32 top;
                IME::real32 right;
                IME::real32 bottom; 
            };
        };
    };

    inline vec2f 
    sizeOfBounds(const Bounds& bounds) {
        return vec2f{bounds.bottomright.x - bounds.topleft.x, bounds.topleft.y - bounds.bottomright.y};
    }

    inline vec2f 
    centerOfBounds(const Bounds& bounds) {
        return bounds.topleft + 0.5f * (bounds.bottomright - bounds.topleft);
    }

    inline vec2f 
    getPositionFromBounds(Bounds bounds) {
        return {(bounds.left + bounds.right) / 2.0f, (bounds.top + bounds.bottom) / 2.0f };
    }
    
    inline vec2f 
    getSizeFromBounds(Bounds bounds) {
        return { bounds.right - bounds.left, bounds.top - bounds.bottom };
    }

    inline bool32 
    isInBounds(vec2f test, const Bounds& bounds) {
        return test.x > bounds.topleft.x && test.x < bounds.bottomright.x && test.y < bounds.topleft.y && test.y > bounds.bottomright.y;
    }

}