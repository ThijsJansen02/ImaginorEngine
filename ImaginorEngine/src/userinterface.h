#pragma once
#include <ImaginorPlatform/src/imegs.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/lists/arrays/ArrayList.h>
#include "memory.h"
#include <datastructures/lists/arrays/FragmentedArrayList.h>
#include "rendering.h"
#include "datastructures/strings/string.h"
#include "functionality.h"
#include "datastructures/maps/hashmap.h"

namespace IME::UI {

    template<typename T>
    using Arraylist = Data::ArrayList_<T, Memory::alloc, Memory::dealloc>;

    template<typename T>
    using ElementArray = Data::FragmentedArrayList_<T, Memory::alloc, Memory::dealloc>;
    using String = Data::String<Memory::alloc, Memory::dealloc>;

    struct Layer;
    struct ElementPtr;

    typedef Pair<bool32, bool32> eventresult; 

    inline sizeptr
    stringhash(const String& string)
    {
        const char* str = string.getCstring();

        sizeptr hash = 5381;
        uint32 c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }

    enum ElementType {
        NONE,
        WINDOW,
        INPUTFIELD,
        PARAGRAPH,
        IMAGE,
        DOCKINGSPACE,
        DIV
    };

    struct ElementPtr {
        void* data;
        ElementType type = ElementType::NONE;
    };

    inline bool operator==(ElementPtr el1, ElementPtr el2) {
        return el1.data == el2.data;
    }

    #define ON_MOUSE_CLICK_EVENT(name) UI::eventresult name(UI::ElementPtr element, vec2f relativemousepos, Event e, UI::Layer* uilayer, const PlatformInterface& platform)
    typedef ON_MOUSE_CLICK_EVENT(on_mouse_click);

    #define ON_INPUT(name) UI::eventresult name(UI::ElementPtr element, IME::Event e, UI::Layer* uilayer, const IME::PlatformInterface& platform)
    typedef ON_INPUT(on_input);

    #define ON_RESIZE_EVENT(name) UI::eventresult name(UI::ElementPtr element, vec2f newsize, UI::Layer* uilayer, const PlatformInterface& platform)
    typedef ON_RESIZE_EVENT(on_resize);

    enum ElementConstraintType {
        WIDTH,
        HEIGHT,
        TEXT_HEIGHT,
        TEXT_COLOR,
        BACKGROUND_COLOR,
        BORDER_COLOR,
        PADDING,
        BORDER,
        MARGIN,
        FLOAT
    };

    enum FloatingType {
        LEFT,
        RIGHT,
        UNSET
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

        Assets::Font* font;
    };

    struct ElementParameters {
        ElementConstraints parentcache;
        Arraylist<ElementConstraint> elementconstraints;

        on_mouse_click* onmouseclick = nullptr;
        on_resize* onresize = nullptr;

        Region contentregion;
        Region backgroundregion;
        Region borderregion;
        Region marginregion;

        vec4f backgroundcolor;
        vec4f bordercolor;
        ElementPtr parent;

        String* tag = nullptr;

        bool32 hovered;
    };

#define NO_TOPBAR 1 << 0
#define NO_RESIZING 1 << 1
#define NO_DRAGGING 1 << 2
#define SCROLL_BAR 1 << 3


    struct Window {
        ElementParameters parameters;

        Region region;
        Region topbarregion;

        String title;    
        bool32 hovered;

        uint32 flags;

        real32 maxscroll;
        real32 scroll;

        Arraylist<ElementPtr> children;

        Assets::FrameBuffer* framebuffer;

        bool32 isdocked = false;
    };

    struct Div {
        ElementParameters parameters;

        Arraylist<ElementPtr> children;
    };

    struct Image {
        ElementParameters parameters;

        Assets::Texture* texture;
    };

    struct Paragraph {
        ElementParameters parameters;

        vec4f textcolor;
        real32 textsize;
        String text;
    };

    struct InputField {
        ElementParameters parameters;
      
        bool32 recievesinput = false;

        on_input* oninput = nullptr;
        
        vec4f textcolor;
        real32 textsize;

        String text;

        uint32 cursorposition = 0.0f;
        real32 origin = 0;
    };

    struct DockingSpace {
        ElementParameters parameters;

        //Region region;
        bool32 iscontainerresizing;
        uint32 edge;

        Arraylist<Pair<ElementPtr, real32>> containers; 

        //true = vertical split; false = horizontal split
        bool horizontal = false;
    };

    struct Layer {

        Arraylist<Window*> windows;

        UI::Arraylist<ElementPtr> windoworder;
        UI::Arraylist<ElementPtr> dockingspaces;

        Assets::Font* basefont;
        gl_id composingshader;

        ElementPtr basewindow;

        vec2f clickingoffset;
        bool32 elementresizing = false;
        bool32 elementgrabbing = false;
        bool32 editinginputfield = false;
        bool32 resizingdockedcontainer = false;
        int32 editedcontaineredge = 0;

        ElementPtr editedelement = {0, NONE};
        ElementPtr focussedinput = {0, NONE};

        ElementPtr focussedwindow;

        Assets::Library* assets;
        PlatformInterface* platform;

        Data::HashMap_<String, ElementPtr, 512, stringhash, Memory::alloc, Memory::dealloc> tagmap;

        void* userpointer;
    };


    ElementPtr addDiv(ElementPtr parent, Layer* uilayer);
    ElementPtr addWindowToLayer(Region windowregion, gl_id shader, const char* title, const PlatformInterface& platform, Layer* layer, uint32 flags);
    ElementPtr addParagraph(const char* text, ElementPtr parent, Layer* uilayer);
    ElementPtr addInputField(const char* initialtext, ElementPtr parent, Layer* uilayer);
    ElementPtr addImage(Assets::Texture* image, ElementPtr parent, Layer* uilayer);
    ElementPtr addDockingSpace(ElementPtr parent, Layer* uilayer);

    void dockWindow(ElementPtr window, ElementPtr dockingspace, bool horizontal, uint32 index, UI::Layer* uilayer);
    ElementPtr undockWindow(ElementPtr window, UI::Layer* uilayer); 

    void destroyElement(ElementPtr el, Layer* layer);
    void destroyChildren(ElementPtr el, Layer* layer);

    bool32 addOnClickEventHandler(ElementPtr element, on_mouse_click* handler, Layer* uilayer);
    bool32 addOnResizeEventHandler(ElementPtr element, on_resize* handler, Layer* uilayer);
    bool32 addOnInputEventHandler(ElementPtr element, on_input* handleer, Layer* uilayer);

    Region getContentRegion(ElementPtr element);

    void addTag(ElementPtr element, const char* tag, Layer* layer);
    ElementPtr getElementByTag(const char* tag, Layer* layer);

    Region calculateElementParameters(ElementPtr el, Layer* layer);

    Layer createLayer(gl_id composingshader, PlatformInterface* platform, Assets::Font* basefont, Assets::Library* assets);
    bool32 propagateEventToLayer(Event e, Layer* layer, const PlatformInterface& platform);
    void calculateElementsForWindow(ElementPtr window, const Layer& layer);
    void calculateAllElements(const Layer& layer);
    void pushLayerToRenderSet(const Layer& layer, Rendering::RenderSet* renderset, const mat4& projection, const PlatformInterface& platform);
    bool32 addConstraint(ElementPtr element, ElementConstraintType type, const char* value);
    bool32 removeConstraint(ElementPtr element, ElementConstraintType type);
}