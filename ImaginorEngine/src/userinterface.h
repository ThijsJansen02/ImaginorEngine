#pragma once
#include <ImaginorPlatform/src/imegs.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/lists/arrays/ArrayList.h>
#include "memory.h"
#include <datastructures/lists/arrays/FragmentedArrayList.h>
#include "rendering.h"
#include "datastructures/strings/string.h"
#include "functionality.h"

namespace IME::UI {

    template<typename T>
    using Arraylist = Data::ArrayList_<T, Memory::alloc, Memory::dealloc>;

    template<typename T>
    using ElementArray = Data::FragmentedArrayList_<T, Memory::alloc, Memory::dealloc>;
    using String = Data::String<Memory::alloc, Memory::dealloc>;

    struct Layer;
    struct ElementPtr;

    typedef Pair<bool32, bool32> eventresult; 

    enum ElementType {
        NONE,
        WINDOW,
        INPUTFIELD,
        PARAGRAPH,
        IMAGE
    };

    struct ElementPtr {
        uint32 offset = 0;
        ElementType type = ElementType::NONE;
    };

    #define ON_MOUSE_CLICK_EVENT(name) UI::eventresult name(UI::ElementPtr element, Event e, UI::Layer* uilayer, const PlatformInterface& platform)
    typedef ON_MOUSE_CLICK_EVENT(on_mouse_click);

    #define ON_RESIZE_EVENT(name) UI::eventresult name(UI::ElementPtr element, vec2f newsize, UI::Layer* uilayer, const PlatformInterface& platform)
    typedef ON_RESIZE_EVENT(on_resize);

    enum ElementConstraintType {
        WIDTH,
        HEIGHT,
        TEXT_HEIGHT,
        BACKGROUND_COLOR,
        BORDER_COLOR,
        PADDING,
        BORDER,
        MARGIN,
        FLOAT
    };

    struct ElementConstraint {
        ElementConstraintType type;
        String value;
    };

    struct Region {
        union {
            struct {
                real32 x;
                real32 y;
                real32 z;
                real32 width;
                real32 height;
            };
            struct {
                vec3f topleft;
                vec2f size;
            };
        };

    };

    inline bool32 operator==(const Region& left, const Region& right) {
        return left.x == right.x && left.y == right.y && left.z == right.z && left.width == right.width && left.height == right.height;
    } 

    inline bool32 operator!=(const Region& left, const Region& right) {
        return !(left == right);
    }

    struct Window {
        on_resize* onresize = nullptr;

        Region region;
        Region topbarregion;
        Region contentregion;
        Region borderregion;

        String title;    
        bool32 hovered;

        Arraylist<ElementPtr> children;

        gl_id framebuffer;
        gl_id colorbuffer;
        gl_id depthbuffer;
        gl_id shader;
    };

    enum FloatingType {
        LEFT,
        RIGHT,
        UNSET
    };

    struct ElementConstraints {
        Region availableregion = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        Region availablefloatingregion = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

        Region region = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        Region floatingregion = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

        vec4f backgroundcolor = {0.0f, 0.0f, 0.0f, 0.0f};
        vec4f bordercolor = {0.0f, 0.0f, 0.0f, 0.0f};
        vec4f textcolor = {0.0f, 0.0f, 0.0f, 1.0f};
        
        real32 textsize = 12.0f;

        bool32 editedheight = false;
        bool32 editedwidth = false;

        vec4f padding = {0.0f, 0.0f, 0.0f, 0.0f}; //left right top bottom
        vec4f margin = {0.0f, 0.0f, 0.0f, 0.0f}; //left right top bottom
        vec4f border = {0.0f, 0.0f, 0.0f, 0.0f};; //left right top bottom

        FloatingType floattype = UNSET;
    };

    struct ElementParameters {
        ElementConstraints parentcache;

        Region contentregion;
        Region backgroundregion;
        Region borderregion;
        Region marginregion;

        vec4f backgroundcolor;
        vec4f bordercolor;
        ElementPtr parent;

        bool32 hovered;
    };

    struct Image {
        on_mouse_click* onmouseclick = nullptr;
        ElementParameters parameters;

        Assets::Texture* texture;
        Arraylist<ElementConstraint> elementconstraints;
    };

    struct Paragraph {
        on_mouse_click* onmouseclick = nullptr;

        ElementParameters parameters;

        vec4f textcolor;
        real32 textsize;
        String text;

        Arraylist<ElementConstraint> elementconstraints;
    };

    struct InputField {
        on_mouse_click* onmouseclick = nullptr;
        
        bool32 recievesinput = false;

        ElementParameters parameters;
        vec4f textcolor;
        real32 textsize;

        String text;

        uint32 cursorposition = 0.0f;
        real32 origin = 0;

        Arraylist<ElementConstraint> elementconstraints;
    };

    struct Layer {

        ElementArray<Window> windows;
        ElementArray<Paragraph> paragraphs;
        ElementArray<InputField> inputfields;
        ElementArray<Image> images;

        UI::Arraylist<ElementPtr> windoworder;

        Assets::Font* basefont;
        gl_id composingshader;

        ElementPtr basewindow;

        vec2f clickingoffset;
        bool32 elementresizing = false;
        bool32 elementgrabbing = false;
        bool32 editinginputfield = false;
        ElementPtr editedelement = {0, NONE};
        ElementPtr focussedinput = {0, NONE};

        void* userpointer;
    };

    ElementPtr addWindowToLayer(Region windowregion, gl_id shader, const char* title, const PlatformInterface& platform, Layer* layer);
    ElementPtr addParagraph(const char* text, ElementPtr parent, Layer* uilayer);
    ElementPtr addInputField(const char* initialtext, ElementPtr parent, Layer* uilayer);
    ElementPtr addImage(Assets::Texture* image, ElementPtr parent, Layer* uilayer);
    bool32 addOnClickEventHandler(ElementPtr element, on_mouse_click* handler, Layer* uilayer);
    bool32 addOnResizeEventHandler(ElementPtr element, on_resize* handler, Layer* uilayer);
    Region getContentRegion(ElementPtr element, const Layer& uilayer);

    Layer createLayer(gl_id composingshader, const PlatformInterface& platform, Assets::Font* basefont);
    bool32 propagateEventToLayer(Event e, Layer* layer, const PlatformInterface& platform);
    void calculateElementsForWindow(ElementPtr window, const Layer& layer);
    void pushLayerToRenderSet(const Layer& layer, Rendering::RenderSet* renderset, const mat4& projection, const PlatformInterface& platform);
    bool32 addConstraint(ElementPtr element, ElementConstraintType type, const char* value, Layer* uilayer);
}