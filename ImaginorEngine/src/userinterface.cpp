#include <core.h>
#include "userinterface.h"
#include "textrenderering.h"

namespace IME::UI {

    bool32 isPointInRegion(vec2f point, Region region) {
        return point.x > region.x && point.x < region.x + region.width && point.y < region.y && point.y > region.y - region.height; 
    }

    enum Extension {
        FALSE,
        NOEXIST,
        PIXELS,
        PERCENTAGE
    };

    Pair<real32, Extension> translateReal32ValueWidthExtension(const char* value) {

        char buffer[32];
        char extbuffer[8];
        const char* strptr = value;
        uint32 bufferptr = 0;
        uint32 extbufferptr = 0;

        while(*strptr != 0) {
            if((*strptr >= '0' && *strptr <= '9') || *strptr == '.'){
                buffer[bufferptr] = *strptr;
                bufferptr++;
            }
            else if(*strptr != ' ') {
                extbuffer[extbufferptr] = *strptr;
                extbufferptr++;
            }

            strptr++;
        }
        buffer[bufferptr] = 0;
        extbuffer[extbufferptr] = 0;
        if(stringCompare(extbuffer, "px")) { return {stringToReal32(buffer), Extension::PIXELS};}
        if(stringCompare(extbuffer, "%"))  { return {stringToReal32(buffer), Extension::PERCENTAGE}; }
        if(stringCompare(extbuffer, "")) {return {stringToReal32(buffer), Extension::NOEXIST}; }
        if(stringCompare(extbuffer, "f")) {return {stringToReal32(buffer), Extension::NOEXIST}; }
        return {stringToReal32(buffer), Extension::FALSE};
    }

    uint32 translateReal32ValuesWidthExtensions(const char* value, Pair<real32, Extension>* values) {
        const char* strptr = value;
        char buffer[64];
        uint32 bufferptr = 0;
        uint32 valuesptr = 0;

        while (*strptr) {
            if(*strptr == ',') {
                buffer[bufferptr] = 0;
                values[valuesptr] = translateReal32ValueWidthExtension(buffer);
                bufferptr = 0;
                valuesptr++;
                strptr++;
            }
            if(*strptr != ' ') {
                buffer[bufferptr] = *strptr;
                bufferptr++;
            }
            strptr++;
        }
        buffer[bufferptr] = 0;
        values[valuesptr] = translateReal32ValueWidthExtension(buffer);
        valuesptr++;
        return valuesptr;
    }

    void translateTextHeightConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {

        auto [value_, extension] = translateReal32ValueWidthExtension(value);
        
        if(extension == PIXELS) {
            targetconstraints->textsize = value_;
            return;
        }
        IME_DEBUG_BREAK()
    }

    void translateHeightConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {

        auto [value_, extension] = translateReal32ValueWidthExtension(value);
        if(extension == PERCENTAGE) {
            if(parentinfo.availableregion.height != INFINITY) {
                real32 height = parentinfo.availableregion.height * (value_ / 100.0f);
                real32 floatingheight = parentinfo.availablefloatingregion.height * (value_ / 100.0f);

                targetconstraints->region.height = height;
                targetconstraints->floatingregion.height = floatingheight;

                targetconstraints->editedheight = true;

                return;
            }
            return;
        }
        if(extension == PIXELS) {
            real32 height = value_;
            real32 floatingheight = value_;

            targetconstraints->region.height = height;
            targetconstraints->floatingregion.height = floatingheight;
            targetconstraints->editedheight = true;
            return;
        }
        IME_DEBUG_BREAK()
    }

    void translateWidthConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {

        auto [value_, extension] = translateReal32ValueWidthExtension(value);
        if(extension == PERCENTAGE) {
            if(parentinfo.availableregion.width != INFINITY) {
                real32 width = parentinfo.availableregion.width * (value_ / 100.0f);
                real32 floatingwidth = parentinfo.availablefloatingregion.width * (value_ / 100.0f);

                targetconstraints->region.width = width;
                targetconstraints->floatingregion.width = floatingwidth;
                targetconstraints->editedwidth = true;
                return;
                
            }
        } 
        if(extension == PIXELS) {
            real32 width = value_;
            real32 floatingwidth = value_;

            targetconstraints->region.width = width;
            targetconstraints->floatingregion.width = floatingwidth;
            targetconstraints->editedwidth = true;
            return;
        }
        IME_DEBUG_BREAK()
    }

    void translatePaddingConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {
        
        Pair<real32, Extension> values[4];
        uint32 count = translateReal32ValuesWidthExtensions(value, values);

        for(uint32 i = 0; i < count; i++) {
            auto [value_, extension] = values[i];
            if(extension == PIXELS) {
                targetconstraints->padding.data[i] = value_;
            }
        }
    }

    void translateBorderConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {
        
        Pair<real32, Extension> values[4];
        uint32 count = translateReal32ValuesWidthExtensions(value, values);

        for(uint32 i = 0; i < count; i++) {
            auto [value_, extension] = values[i];
            if(extension == PIXELS) {
                targetconstraints->border.data[i] = value_;
            }
        }
    }

    void translateBorderColorConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {

        Pair<real32, Extension> values[4];

        uint32 count = translateReal32ValuesWidthExtensions(value, values);
        for(uint32 i = 0; i < count; i++) {
            auto [value_, extension] = values[i];
            if(extension == NOEXIST) {
                targetconstraints->bordercolor.data[i] = value_;
            }
        }

    }

    void translateMarginConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {

        Pair<real32, Extension> values[4];
        uint32 count = translateReal32ValuesWidthExtensions(value, values);

        for(uint32 i = 0; i < count; i++) {
            auto [value_, extension] = values[i];
            if(extension == PIXELS) {
                targetconstraints->margin.data[i] = value_;
            }
        }
    }

    void translateTextColorConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {
        Pair<real32, Extension> values[4];

        uint32 count = translateReal32ValuesWidthExtensions(value, values);
        for(uint32 i = 0; i < count; i++) {
            auto [value_, extension] = values[i];
            if(extension == NOEXIST) {
                targetconstraints->textcolor.data[i] = value_;
            }
        }
    }

    void translateBackGroundColorConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {

        Pair<real32, Extension> values[4];

        uint32 count = translateReal32ValuesWidthExtensions(value, values);
        for(uint32 i = 0; i < count; i++) {
            auto [value_, extension] = values[i];
            if(extension == NOEXIST) {
                targetconstraints->backgroundcolor.data[i] = value_;
            }
        }
    }

    void translateFloatConstraint(const char* value, ElementConstraints* targetconstraints, const ElementConstraints parentinfo) {
        if(stringCompare(value, "left")) {
            targetconstraints->floattype = LEFT;
        }
        if(stringCompare(value, "unset")) {
            targetconstraints->floattype = UNSET;
        }
        if(stringCompare(value, "right")){
            targetconstraints->floattype = RIGHT;
        }
    }
    
    ElementConstraints determineElementConstraints(const ElementConstraints& parentinfo, const Arraylist<ElementConstraint>& constraints) {

        ElementConstraints result = parentinfo;

        //width and height are only given to child when specified
        result.floatingregion = parentinfo.availablefloatingregion;
        result.region = parentinfo.availableregion;
        result.editedheight = false;
        result.editedwidth = false;

        result.availableregion.z += 1.0f;

        for (ElementConstraint constraint : constraints) {

            switch (constraint.type)
            {
            case TEXT_COLOR: translateTextColorConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case WIDTH: translateWidthConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case HEIGHT: translateHeightConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case BACKGROUND_COLOR: translateBackGroundColorConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case TEXT_HEIGHT: translateTextHeightConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case PADDING: translatePaddingConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case MARGIN: translateMarginConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case FLOAT: translateFloatConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case BORDER: translateBorderConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            case BORDER_COLOR: translateBorderColorConstraint(constraint.value.getCstring(), &result, parentinfo); break;
            default: break;
            }
        }
        return result;
    }

    bool32 addConstraint(ElementPtr element, ElementConstraintType type, const char* value) {

        ElementConstraint constraint;
        constraint.type = type;
        constraint.value.set(value);

        ((ElementParameters*)element.data)->elementconstraints.push_back(constraint); return true;
    }

    bool32 removeConstraint(ElementPtr element, ElementConstraintType type) {
        bool32 result = false;
        for(uint32 i = 0; i < ((ElementParameters*)element.data)->elementconstraints.getCount(); i++) {
            if(((ElementParameters*)element.data)->elementconstraints[i].type == type) {
                ((ElementParameters*)element.data)->elementconstraints[i].value.clear();
                ((ElementParameters*)element.data)->elementconstraints.remove(i);
                result = true;
            }
        }
        return result;
    }

    Layer 
    createLayer(gl_id composingshader, PlatformInterface* platform, Assets::Font* basefont, Assets::Library* assets) {
        Layer layer;
        layer.windows.init(1);
        layer.basefont = basefont;
        layer.composingshader = composingshader;
        layer.assets = assets;
        layer.tagmap.init();
        layer.platform = platform;

        layer.windoworder.init(0);
        
        Region baseregion;
        baseregion.topleft = {0.0f, (real32)platform->window.width, -1.0f };
        baseregion.size = { (real32)platform->window.height, (real32)platform->window.width };
        layer.basewindow = addWindowToLayer(baseregion, composingshader, "basewindow", *platform, &layer, NO_TOPBAR | NO_RESIZING | NO_DRAGGING);

        return layer;
    }

    global_var real32 topbarheight = 20;
    global_var vec4f topbarcolor = {0.3f, 0.3f, 0.7f, 1.0f};

    ElementPtr addParagraph(const char* text, ElementPtr parent, Layer* layer) {

        Paragraph result;
        result.text.set(text);
        result.parameters.elementconstraints.init(0);
        result.parameters.parent = parent;

        ElementPtr ptr = {nullptr, UI::PARAGRAPH};

        if(parent.type == WINDOW) {

            Window* window = (Window*)parent.data;
            Paragraph* p = (Paragraph*)Memory::alloc(sizeof(Paragraph));
            *p = result;
            ptr.data = (void*)p;
            window->children.push_back(ptr);
        }
        return ptr;
    }

    ElementPtr addInputField(const char* initialtext, ElementPtr parent, Layer* uilayer) {
        InputField result;
        result.text.set(initialtext);
        result.parameters.parent = parent;
        result.parameters.elementconstraints.init(0);
        result.cursorposition = 0;
        result.origin = 0.0f;
        
        ElementPtr ptr = {nullptr, UI::INPUTFIELD};

        if(parent.type == UI::WINDOW) {
            Window* window = (Window*)parent.data;
            InputField* i = (InputField*)Memory::alloc(sizeof(InputField));
            *i = result;
            ptr.data = (void*)i;
            window->children.push_back(ptr);
        }

        return ptr;
    }

    ElementPtr addImage(Assets::Texture* image, ElementPtr parent, Layer* uilayer) {
        Image result;
        result.parameters.onmouseclick = nullptr;
        result.texture = image;
        result.parameters.elementconstraints.init(0);

        ElementPtr ptr = {nullptr, UI::IMAGE};

        if(parent.type == UI::WINDOW) {
            Window* window = (Window*)parent.data;
            Image* i = (Image*)Memory::alloc(sizeof(Image));
            *i = result;
            ptr.data = (void*)i;
            window->children.push_back(ptr);
        }

        return ptr;
    }

    ElementPtr addDockingSpace(ElementPtr parent, Layer* uilayer) {
        DockingSpace result;
        result.containers.init(0);
        result.parameters.parent = parent;
        result.parameters.elementconstraints.init(0);
        result.iscontainerresizing = false;
        result.edge = 0;

        ElementPtr ptr = {nullptr, DOCKINGSPACE};

        if(parent.type == WINDOW) {
            Window* w = (Window*)parent.data;
            DockingSpace* d = (DockingSpace*)Memory::alloc(sizeof(DockingSpace));
            *d = result;
            ptr.data = (void*)d;
            w->children.push_back(ptr);
        }

        uilayer->dockingspaces.push_back(ptr);

        return ptr;
    }

    void destroyElementParameters(ElementParameters* p, Layer* layer) {
        if(p->tag) {
            layer->tagmap.removeKey(*p->tag);
        }
        if(p->elementconstraints.getCapacity() > 0) {
            p->elementconstraints.destroy();
        }
    }

    void destroyDockingSpace(ElementPtr el, Layer* layer) {
        DockingSpace* d = (DockingSpace*)el.data;

        d->containers.destroy();
        layer->dockingspaces.remove(el);

        destroyElementParameters(&d->parameters, layer);

        Memory::dealloc(sizeof(DockingSpace), (byte*)el.data);
    }

    void destroyParagraph(ElementPtr el, Layer* layer) {
        Paragraph* p = (Paragraph*)el.data;

        p->text.clear();
        destroyElementParameters(&p->parameters, layer);
        Memory::dealloc(sizeof(Paragraph), (byte*)el.data);
    }

    void destroyElement(ElementPtr el, Layer* layer) {
        switch(el.type) {
            case DOCKINGSPACE: destroyDockingSpace(el, layer);
            case PARAGRAPH: destroyParagraph(el, layer);
        }
    }

    Region subtractBordersFromRegion(Region region, const vec4f& borders);

    ElementPtr addWindowToLayer(Region windowregion, gl_id shader, const char* title, const PlatformInterface& platform, Layer* layer, uint32 flags) {
        Window window;
        window.region = windowregion;
        window.flags = flags;

        /*
        window.topbarregion = { 0.0f, windowregion.height, 0.0f, windowregion.width, topbarheight};
        window.parameters.borderregion = {0.0f, windowregion.height - topbarheight, 0.0f, windowregion.width, windowregion.height - topbarheight};
        window.parameters.contentregion = subtractBordersFromRegion(window.parameters.borderregion, {1.0f, 1.0f, 0.0f, 1.0f});
        window.parameters.contentregion.z += 0.2f;*/

        window.children.init(0);
        window.title.set(title);
        window.parameters.elementconstraints.init(0);

        window.scroll = 0.0f;
        window.maxscroll = 0.0f;

        TextureProperties props;
        props.format = IME_RGBA;
        props.generatemipmaps = false;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;

        char buffer[128];
        sprintf_s(buffer, 128, "%s_framebuffer", title);
        window.framebuffer = IME::Assets::createFrameBuffer(windowregion.width, windowregion.height, platform, buffer, layer->assets);
        sprintf_s(buffer, 128, "%s_colorbuffer", title);
        IME::Assets::addTextureAttachmentToFramebuffer(window.framebuffer, IME_RGB, IME_COLOR_ATTACHMENT0, platform, buffer, layer->assets);
        sprintf_s(buffer, 128, "%s_depthbuffer", title);
        IME::Assets::addRenderBufferAttachmentToFramebuffer(window.framebuffer, IME_DEPTH24_STENCIL8, IME_DEPTH_STENCIL_ATTACHMENT, platform, buffer, layer->assets);
        

        Window* w = (Window*)Memory::alloc(sizeof(Window));
        *w = window;

        layer->windows.push_back(w);
        ElementPtr result = {(void*)w, ElementType::WINDOW};

        layer->windoworder.push_back(result);

        return result;
    }

    bool32 addOnClickEventHandler(ElementPtr element, on_mouse_click* handler, Layer* uilayer) {
        ((ElementParameters*)element.data)->onmouseclick = handler;
        return true;
    }

    bool32 addOnResizeEventHandler(ElementPtr element, on_resize* handler, Layer* uilayer) {
        ((ElementParameters*)element.data)->onresize = handler;
        return true;
    }

    void dockWindow(ElementPtr window, ElementPtr dockingspace, bool horizontal, uint32 index, UI::Layer* uilayer) {

        Window* w = (Window*)window.data;
        DockingSpace* d = (DockingSpace*)dockingspace.data;

        uilayer->windoworder.remove(window);
        uilayer->windoworder.push_front(window);
        w->isdocked = true;
        w->parameters.parent = dockingspace;

        if(d->containers.getCount() > 0) {
            if(d->containers.getCount() == 1) {
                d->containers[0].v2 = 0.5f;
                d->containers.insert({window, 0.5f},  index);
                d->horizontal = horizontal;
            } else {
                //case a new dockingspace needs to be created
                if(d->horizontal != horizontal) {

                    if(d->parameters.parent.type == WINDOW) {
                        Window* parentw = (Window*)d->parameters.parent.data;
                        
                        DockingSpace result;
                        result.parameters.parent = d->parameters.parent;
                        result.containers.init(0);
                        result.parameters.elementconstraints = d->parameters.elementconstraints;
                        result.iscontainerresizing = false;
                        result.edge = 0;

                        d->parameters.elementconstraints.init(0);
                        addConstraint(dockingspace, WIDTH, "100%");
                        addConstraint(dockingspace, HEIGHT, "100%");

                        ElementPtr ptr = {nullptr, DOCKINGSPACE};

                        //create new dockingspace
                        DockingSpace* newdock = (DockingSpace*)Memory::alloc(sizeof(DockingSpace));
                        *newdock = result;
                        ptr.data = (void*)newdock;

                        //replace window with old window
                        for(uint32 i = 0; i < parentw->children.getCount(); i++) {
                            if(parentw->children[i] == dockingspace) {
                                parentw->children[i] = ptr;
                            }
                        }
                        
                        newdock->containers.push_back({dockingspace, 1.0f});
                        d->parameters.parent = ptr;
                        dockWindow(window, ptr, horizontal, index, uilayer);

                        uilayer->dockingspaces.push_back(ptr);
                        

                    } else if(d->parameters.parent.type == DOCKINGSPACE){
                        DockingSpace* parentd = (DockingSpace*)d->parameters.parent.data;
                        
                        DockingSpace result;
                        result.parameters.parent = d->parameters.parent;
                        result.containers.init(0);
                        result.parameters.elementconstraints = d->parameters.elementconstraints;
                        result.iscontainerresizing = false;
                        result.edge = 0;

                        d->parameters.elementconstraints.init(0);
                        addConstraint(dockingspace, WIDTH, "100%");
                        addConstraint(dockingspace, HEIGHT, "100%");

                        ElementPtr ptr = {nullptr, DOCKINGSPACE};

                        //create new dockingspace
                        DockingSpace* newdock = (DockingSpace*)Memory::alloc(sizeof(DockingSpace));
                        *newdock = result;
                        ptr.data = (void*)newdock;

                        //replace window with old window
                        for(uint32 i = 0; i < parentd->containers.getCount(); i++) {
                            if(parentd->containers[i].v1 == dockingspace) {
                                parentd->containers[i].v1 = ptr;
                            }
                        }
                        
                        newdock->containers.push_back({dockingspace, 1.0f});
                        d->parameters.parent = ptr;
                        dockWindow(window, ptr, horizontal, index, uilayer);

                        uilayer->dockingspaces.push_back(ptr);                        



                    } else {
                        IME_DEBUG_BREAK()
                    }
                }
                //no dockingspace needs to be created 
                else {
                    if(index == 0) {
                        real32 size = d->containers[0].v2 / 2.0f;
                        d->containers[0].v2 = size;
                        d->containers.insert({window, size}, index);
                    } else if(index == d->containers.getCount()) {
                        real32 size = d->containers[index - 1].v2 / 2.0f;
                        d->containers[index-1].v2 = size;
                        d->containers.insert({window, size}, index);
                    } else {
                        real32 size = (1.0f/3.0f) * d->containers[index - 1].v2 + (1.0f/3.0f) * d->containers[index + 1].v2;
                        d->containers[index - 1].v2 -= size / 2.0f;
                        d->containers[index].v2 -= size / 2.0f;
                        d->containers.insert({window, size}, index);
                    }
                }
            }
        } else {
            d->containers.push_back({window, 1.0f});
        }
    }

    inline bool operator==(Pair<ElementPtr, real32> left, Pair<ElementPtr, real32> right) {
        return left.v1 == right.v1;
    }

    ElementPtr undockWindow(ElementPtr window, UI::Layer* uilayer) {

        Window* w = (Window*)window.data;
        ElementPtr dockingspace = w->parameters.parent;
        DockingSpace* d = (DockingSpace*)w->parameters.parent.data;

        ElementPtr recalc = w->parameters.parent;

        for(uint32 i = 0 ; i < d->containers.getCount(); i++) {
            real32 size = d->containers[i].v2;
            if(d->containers[i].v1.data == window.data) {

                d->containers.remove(i);

                //if the dockinspace is unnecessary
                if(d->containers.getCount() == 1 && d->parameters.parent.type == DOCKINGSPACE) {
                    
                    ElementPtr parentdockingspace = d->parameters.parent;
                    DockingSpace* pd = (DockingSpace*)parentdockingspace.data;
                    
                    for(uint32 i = 0; i < pd->containers.getCount(); i++) {
                        if(pd->containers[i].v1 == dockingspace) {
                            pd->containers[i].v1 = d->containers[0].v1;
                            ((ElementParameters*)pd->containers[i].v1.data)->parent = parentdockingspace;

                            d->containers.clear();
                            destroyDockingSpace(dockingspace, uilayer);

                            recalc = parentdockingspace;
                            w->isdocked = false;
                            w->parameters.parent = {nullptr, NONE};
                            return recalc;
                        }
                    }
                    IME_DEBUG_BREAK()
                } 

                if(d->containers.getCount() == 1 && d->containers[0].v1.type == DOCKINGSPACE) {

                    DockingSpace* child = (DockingSpace*)d->containers[0].v1.data;

                    ElementPtr parent = d->parameters.parent;
                    d->containers.destroy();
                    d->parameters.elementconstraints.destroy();

                    *d = *child;
                    d->parameters.parent = parent;

                    for(uint32 i = 0; i < d->containers.getCount(); i++) {

                        ((ElementParameters*)d->containers[i].v1.data)->parent = {(void*)d, DOCKINGSPACE};
                    }

                    Memory::dealloc(sizeof(dockingspace), (byte*)child);

                    recalc = parent;

                    w->isdocked = false;
                    w->parameters.parent = {nullptr, NONE};
                    return recalc;
                }

                if(d->containers.getCount() == 0) {
                    break;
                }

                if(i == d->containers.getCount()) {
                    d->containers[i - 1].v2 += size;
                } else {
                    d->containers[i].v2 += size;
                }
            }
        }

        w->isdocked = false;
        w->parameters.parent = {nullptr, NONE};
        return recalc;
    }



    Region getContentRegion(ElementPtr element) {
        return ((ElementParameters*)element.data)->contentregion;
    }

    inline void 
    pushRegionToRQ(Region region, vec4f color, gl_id shader, Arraylist<Rendering::SimpleQuadCommand>* rq) {
        Rendering::pushColoredQuadTL(region.topleft, region.size, shader, color, rq);
    }

    inline void 
    pushTexturedRegionToRQ(Region region, gl_id texture, gl_id shader, Arraylist<Rendering::SimpleQuadCommand>* rq) {
        Rendering::pushTexturedQuadTL(region.topleft, region.size, shader, texture, false, rq);
    }

    //calculates the parameters of a generic element
    Region
    calculateElementParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer);

    Region subtractBordersFromRegion(Region region, const vec4f& borders) {
        region.x += borders.left;
        region.width -= (borders.left + borders.right);
        region.y -= borders.top;
        region.height -= (borders.top + borders.bottom);
        return region;
    }

    Region addBordersToRegion(Region region, const vec4f& borders) {
        region.x -= borders.left;
        region.width += (borders.left + borders.right);
        region.y += borders.top;
        region.height += (borders.top + borders.bottom);
        return region;
    }

    Region calcvailableContentRegion(const ElementConstraints& constraints) {
        Region result;
         if(constraints.floattype == UNSET) {
            result = subtractBordersFromRegion(subtractBordersFromRegion(subtractBordersFromRegion(constraints.region, constraints.margin), constraints.border), constraints.padding);
        }
        if(constraints.floattype == LEFT) {
            if(constraints.floatingregion.width <= constraints.availablefloatingregion.width) {
                result = subtractBordersFromRegion(subtractBordersFromRegion(constraints.floatingregion, constraints.margin), constraints.padding);
            } else {
                result = subtractBordersFromRegion(subtractBordersFromRegion(constraints.region, constraints.margin), constraints.padding);
            }
        }
        return result;
    }

    void calcRegions(ElementParameters* parameters, const ElementConstraints& constraints) {
        parameters->backgroundcolor = constraints.backgroundcolor;
        parameters->bordercolor = constraints.bordercolor;
        parameters->contentregion.z += 0.4f;
        parameters->backgroundregion = addBordersToRegion(parameters->contentregion, constraints.padding);
        parameters->backgroundregion.z -= 0.2f;
        parameters->borderregion = addBordersToRegion(parameters->backgroundregion, constraints.border);
        parameters->borderregion.z -= 0.2f;
        parameters->marginregion = addBordersToRegion(parameters->borderregion, constraints.margin);
    }

    inline Region 
    calculateWindowParameters(ElementPtr ptr, ElementConstraints parentinfo, const Layer& uilayer) {

        Window* window = (Window*)ptr.data;
        Region windowregion = window->region;

        if(window->isdocked) {
            window->region = parentinfo.availableregion;
            windowregion = window->region;
        }
        if(window->flags & NO_TOPBAR) {
            window->topbarregion = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        } else {
            window->topbarregion = { 0.0f, windowregion.height, 40.0f, windowregion.width, topbarheight};
        }

        window->parameters.borderregion = {0.0f, windowregion.height - window->topbarregion.height, 0.0f, windowregion.width, windowregion.height - window->topbarregion.height};
        window->parameters.contentregion = subtractBordersFromRegion(window->parameters.borderregion, {1.0f, 1.0f, 0.0f, 1.0f});
        window->parameters.contentregion.z += 0.2f;

        if(window->region.width != window->framebuffer->width || window->region.height != window->framebuffer->height) {
            Assets::resizeFrameBuffer((uint32)window->region.width, (uint32)window->region.height, window->framebuffer, *uilayer.platform);
            if(window->parameters.onresize) {
                window->parameters.onresize(ptr, {window->parameters.borderregion.width, window->parameters.borderregion.height}, (Layer*)(&uilayer), *uilayer.platform);
            }
        }

        parentinfo.availableregion = window->parameters.contentregion;
        parentinfo.availableregion.height = INFINITY;
        parentinfo.availableregion.z += 1;
        
        //reset window region
        ElementConstraints constraints = determineElementConstraints(parentinfo, window->parameters.elementconstraints);

        parentinfo.availableregion = calcvailableContentRegion(constraints);
        parentinfo.availableregion.y += window->scroll;

        parentinfo.availablefloatingregion = parentinfo.availableregion;

        for (ElementPtr ptr : window->children) {

            Region childregion = calculateElementParameters(ptr, parentinfo, uilayer);

            if(toVec2(childregion.topleft) == toVec2(parentinfo.availablefloatingregion.topleft)) {
                parentinfo.availablefloatingregion.x = childregion.x + childregion.width;
                parentinfo.availablefloatingregion.width -= childregion.width;

                real32 y = minReal32(parentinfo.availableregion.y, childregion.y - childregion.height);

                parentinfo.availableregion.height -= parentinfo.availableregion.y - y;
                parentinfo.availableregion.y = y;

                if(parentinfo.availablefloatingregion.width <= 0.0f) {
                    parentinfo.availablefloatingregion = parentinfo.availableregion;
                }

            } else if(toVec2(childregion.topleft) == toVec2(parentinfo.availableregion.topleft)) {

                parentinfo.availableregion.y = childregion.y - childregion.height;
                parentinfo.availableregion.height -= childregion.height;

                parentinfo.availablefloatingregion.x = childregion.x + childregion.width;
                parentinfo.availablefloatingregion.y = childregion.y;
                parentinfo.availablefloatingregion.width = parentinfo.availableregion.width - childregion.width;
                parentinfo.availablefloatingregion.height = parentinfo.availableregion.height;
                parentinfo.availablefloatingregion.z = parentinfo.availableregion.z;

                if(parentinfo.availablefloatingregion.width <= 0.0f) {
                    parentinfo.availablefloatingregion = parentinfo.availableregion;
                }
            }
        }

        window->maxscroll = (real32)floorReal32(maxReal32(window->parameters.contentregion.y - window->parameters.contentregion.height - parentinfo.availableregion.y + window->scroll, 0.0f));

        calcRegions(&window->parameters, constraints);

        return window->parameters.marginregion;
    }


    inline Region
    calculateDockingSpaceParameters(ElementPtr ptr, ElementConstraints parentinfo, const Layer& uilayer) {

        DockingSpace* ds = (DockingSpace*)ptr.data;
        ds->parameters.parentcache = parentinfo;

        ElementConstraints constraints = determineElementConstraints(parentinfo, ds->parameters.elementconstraints);

        ds->parameters.contentregion = calcvailableContentRegion(constraints);

        real32 offset = 0;

        if(!ds->horizontal) {
            for(auto[el, size] : ds->containers) {
                ElementConstraints childinfo = parentinfo;
                childinfo.availableregion = ds->parameters.contentregion;
                childinfo.availableregion.x += offset;
                childinfo.availableregion.width = ds->parameters.contentregion.width * size;
                childinfo.availableregion.z = ds->parameters.contentregion.z + 1;
                offset += childinfo.availableregion.width;

                if(el.type == WINDOW) {
                    calculateWindowParameters(el, childinfo, uilayer);
                }
                if(el.type == DOCKINGSPACE) {
                    calculateDockingSpaceParameters(el, childinfo, uilayer);
                }
            }
        } else {
            for(auto[el, size] : ds->containers) {
                ElementConstraints childinfo = parentinfo;
                childinfo.availableregion = ds->parameters.contentregion;
                childinfo.availableregion.y -= offset;
                childinfo.availableregion.height = ds->parameters.contentregion.height * size;
                childinfo.availableregion.z = ds->parameters.contentregion.z + 1;
                offset += childinfo.availableregion.height;

                if(el.type == WINDOW) {
                    calculateWindowParameters(el, childinfo, uilayer);
                }
                if(el.type == DOCKINGSPACE) {
                    calculateDockingSpaceParameters(el, childinfo, uilayer);
                }
            }
        }

        calcRegions(&ds->parameters, constraints);

        //return the occupied region;
        return ds->parameters.marginregion;
    }

    inline Region 
    calculateParagraphParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer) {

        Paragraph* paragraph = (Paragraph*)ptr.data;
        paragraph->parameters.parentcache = parentinfo;

        ElementConstraints constraints = determineElementConstraints(parentinfo, paragraph->parameters.elementconstraints);

        //determine contentregion;
        paragraph->parameters.contentregion = calcvailableContentRegion(constraints);


        //push the font to the layer
        paragraph->textcolor = constraints.textcolor;
        paragraph->textsize = constraints.textsize;
        real32 scalefactor = paragraph->textsize / uilayer.basefont->pixelheight;
        vec2f size = calcTextSizeFromFont(paragraph->text.getCstring(), paragraph->parameters.contentregion.width, 0.0f, scalefactor, *uilayer.basefont);

        if(!constraints.editedwidth) {
            paragraph->parameters.contentregion.width = size.width;
        }
        if(!constraints.editedheight) {
            paragraph->parameters.contentregion.height = size.height; 
        }

        paragraph->textcolor = constraints.textcolor;

        calcRegions(&paragraph->parameters, constraints);

        //return the occupied region;
        return paragraph->parameters.marginregion;
    } 

    inline Region calculateInputFieldParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer) {

        InputField* f = (InputField*)ptr.data;

        f->parameters.parentcache = parentinfo;
        ElementConstraints constraints = determineElementConstraints(parentinfo, f->parameters.elementconstraints);

        //determine contentregion;
        f->parameters.contentregion = calcvailableContentRegion(constraints);

        f->textsize = constraints.textsize;
        f->textcolor = constraints.textcolor;

        if(!constraints.editedwidth) {
            f->parameters.contentregion.width = 200.0f;
        }
        if(!constraints.editedheight) {
            f->parameters.contentregion.height = f->textsize;
        }

        calcRegions(&f->parameters, constraints);
        return f->parameters.marginregion;
    }

    inline Region calculateImageParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer) {

        UI::Image* im = (Image*)ptr.data;

        im->parameters.parentcache = parentinfo;
        ElementConstraints constraints = determineElementConstraints(parentinfo, im->parameters.elementconstraints);

        //determine contentregion;
        im->parameters.contentregion = calcvailableContentRegion(constraints);

        if(!constraints.editedwidth) {
            im->parameters.contentregion.width = (real32)im->texture->props.width;
        }
        if(!constraints.editedheight) {
            im->parameters.contentregion.height = (real32)im->texture->props.height;
        }

        calcRegions(&im->parameters, constraints);
        return im->parameters.marginregion;
    }

    void addTag(ElementPtr element, const char* tag, Layer* layer) {
        auto keyvalue = layer->tagmap.add(String::create(tag), element);
        ((ElementParameters*)element.data)->tag = &keyvalue->k;
    }

    ElementPtr getElementByTag(const char* tag, Layer* layer) {
        String string;
        string.set(tag); 
        ElementPtr el = layer->tagmap.get(string)->v;
        string.clear();
        return el;
    }

    Region 
    calculateElementParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer) {

        if(ptr.type == UI::WINDOW) { return calculateWindowParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::PARAGRAPH) { return calculateParagraphParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::INPUTFIELD) { return calculateInputFieldParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::IMAGE) { return calculateImageParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::DOCKINGSPACE) { return calculateDockingSpaceParameters(ptr, parentinfo, uilayer); }
        return parentinfo.availableregion;
    }

    inline void 
    pushParagraphToRenderQueue(
            ElementPtr ptr, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrq, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrqt, 
            const Layer& uilayer
    ) {

        Paragraph paragraph = *(Paragraph*)ptr.data;
        real32 scalefactor = paragraph.textsize / uilayer.basefont->pixelheight;

        pushTextFromFontToRQ(
            paragraph.text.getCstring(), 
            paragraph.parameters.contentregion.topleft, 
            paragraph.parameters.contentregion.width, 0.0f, paragraph.textcolor, scalefactor, 
            *uilayer.basefont, 
            quadrqt
        );

        if(paragraph.parameters.bordercolor.w > 0.0f) {
            pushRegionToRQ(paragraph.parameters.borderregion, paragraph.parameters.bordercolor, uilayer.composingshader, quadrq);
        }
        if(paragraph.parameters.backgroundcolor.w > 0.0f) {
            pushRegionToRQ(paragraph.parameters.backgroundregion, paragraph.parameters.backgroundcolor, uilayer.composingshader, quadrq);
        }
        return;
    }

    inline void
    pushInputFieldToRenderQueue(ElementPtr ptr, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrq, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrqt, 
            const Layer& uilayer) {

        InputField f = *(InputField*)ptr.data;
        real32 scalefactor = f.textsize / uilayer.basefont->pixelheight;

        pushInputTextFromFontToRQ(
            f.text.getCstring(), 
            f.parameters.contentregion.topleft,
            f.parameters.contentregion.width,
            scalefactor,
            f.origin,
            f.cursorposition,
            f.textcolor,
            *uilayer.basefont,
            quadrqt);

        if(f.recievesinput) {
            Region region;
            region.topleft = {f.origin + f.parameters.contentregion.x, f.parameters.contentregion.y, 10.0f};
            region.size = {2.0f, f.parameters.contentregion.height};
            pushRegionToRQ(region, f.textcolor, uilayer.composingshader, quadrq);
        }

        if(f.parameters.bordercolor.w > 0.0f) {
            pushRegionToRQ(f.parameters.borderregion, f.parameters.bordercolor, uilayer.composingshader, quadrq);
        }
        if(f.parameters.backgroundcolor.w > 0.0f) {
            pushRegionToRQ(f.parameters.backgroundregion, f.parameters.backgroundcolor, uilayer.composingshader, quadrq);
        }
    }

    inline void
    pushImageToRenderQueue(
            ElementPtr ptr, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrq, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrqt, 
            const Layer& layer
    ) {
        Image e = *(Image*)ptr.data;

        pushTexturedRegionToRQ(e.parameters.contentregion, e.texture->id, layer.composingshader, quadrq);

        if(e.parameters.bordercolor.w > 0.0f) {
            pushRegionToRQ(e.parameters.borderregion, e.parameters.bordercolor, layer.composingshader, quadrq);
        }
        if(e.parameters.backgroundcolor.w > 0.0f) {
            pushRegionToRQ(e.parameters.backgroundregion, e.parameters.backgroundcolor, layer.composingshader, quadrq);
        }
    }

    real32 dheight = 25.0;
    real32 dwidth = 80.0;

    vec4f dcolor = {0.3, 0.3f, 0.9f, 1.0f};

    inline void 
    pushDockingSpaceToRenderQueue(
        ElementPtr ptr, 
        Arraylist<Rendering::SimpleQuadCommand>* quadrq, 
        Arraylist<Rendering::SimpleQuadCommand>* quadrqt, 
        const Layer& layer
    ) {
        DockingSpace e = *(DockingSpace*)ptr.data;

        if(e.containers.getCount() == 0) {
            pushRegionToRQ(e.parameters.backgroundregion, e.parameters.backgroundcolor, layer.composingshader, quadrq);
        }

        real32 offset = 0;
        for(auto[el, size] : e.containers) {
            if(el.type == DOCKINGSPACE) {
                pushDockingSpaceToRenderQueue(el, quadrq, quadrqt, layer);
            }
            if(el.type == WINDOW) {
                Window* w = (Window*)el.data;
                
                /*
                char buffer[256];
                sprintf_s(buffer, 256, "(%f, %f)", w->region.width, w->region.height);
                pushTextFromFontToRQ(buffer, w->region.topleft + vec3f{0.0f, 0.0f, 1.0f}, w->region.width, 0, {1.0f, 0.0f, 0.0f, 1.0f}, 0.5f, *layer.basefont, quadrqt);
                */
                pushTexturedRegionToRQ(w->region, ((Assets::Texture*)w->framebuffer->colorattachements[IME_COLOR_ATTACHMENT0].data)->id, layer.composingshader, quadrq);
            }
        }
        
        if(layer.elementgrabbing) {

            Region cr = e.parameters.contentregion;

            if(e.containers.getCount() == 0) {
                pushRegionToRQ(
                    {cr.topleft.x + cr.width / 2.0f - dwidth / 2.0f, cr.topleft.y - cr.height / 2.0f + dwidth / 2.0f, cr.z + 3.0f, dwidth, dwidth},
                    dcolor, layer.composingshader, quadrq);
            } else {
                pushRegionToRQ({cr.x, cr.y - cr.height / 2.0f + dwidth / 2.0f, cr.z + 5.0f, dheight, dwidth}, 
                    dcolor, layer.composingshader, quadrq);

                pushRegionToRQ({cr.x + cr.width - dheight, cr.y - cr.height / 2.0f + dwidth / 2.0f, cr.z + 5.0f, dheight, dwidth}, 
                    dcolor, layer.composingshader, quadrq);

                pushRegionToRQ({cr.x + cr.width / 2.0f - dwidth / 2.0f, cr.y, cr.z + 5.0f, dwidth, dheight}, 
                    dcolor, layer.composingshader, quadrq);

                pushRegionToRQ({cr.x + cr.width / 2.0f - dwidth / 2.0f, cr.y - cr.height + dheight, cr.z + 5.0f, dwidth, dheight}, 
                    dcolor, layer.composingshader, quadrq);
            }

        }


        return;
    }

    inline void 
    pushElementToRenderQueue(
            ElementPtr ptr, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrq, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrqt, 
            const Layer& layer
    ) {    
        if(ptr.type == UI::PARAGRAPH) { return pushParagraphToRenderQueue(ptr, quadrq, quadrqt, layer);}
        if(ptr.type == UI::INPUTFIELD) { return pushInputFieldToRenderQueue(ptr, quadrq, quadrqt, layer);}
        if(ptr.type == UI::IMAGE) { return pushImageToRenderQueue(ptr, quadrq, quadrqt, layer);}
        if(ptr.type == UI::DOCKINGSPACE) { return pushDockingSpaceToRenderQueue(ptr, quadrq, quadrqt, layer); }
    }

    void calculateElementsForWindow(ElementPtr window, const Layer& layer) {
        ElementConstraints constraints;
        calculateElementParameters(window, constraints, layer);
    }

    void calculateAllElements(const Layer& layer) {
        for(ElementPtr window : layer.windoworder) {
            if(!((Window*)window.data)->isdocked) {
                calculateElementsForWindow(window, layer);
            }
        }
    }

    void pushLayerToRenderSet(const Layer& layer, Rendering::RenderSet* renderset, const mat4& projection, const PlatformInterface& platform) {

        Arraylist<Rendering::SimpleQuadCommand> composingrq;            
        composingrq.init(0);

        real32 z = 0.0f;
        for(ElementPtr w : layer.windoworder) {

            Window window = *(Window*)w.data; 
                
            //initializing the local renderqueues
            Arraylist<Rendering::SimpleQuadCommand> quadrq;            
            quadrq.init(0);
            Arraylist<Rendering::SimpleQuadCommand> quadrqt;            
            quadrqt.init(0);
            pushRegionToRQ(window.parameters.borderregion, topbarcolor, layer.composingshader, &quadrq);
            pushRegionToRQ(window.parameters.contentregion, window.parameters.backgroundcolor, layer.composingshader, &quadrq);

            if(window.scroll > 0.0f) {
                pushRegionToRQ({0.0f, 0.0f, 10.0f, 100.0f, 100.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, layer.composingshader, &quadrq);
            }

            if(!(window.flags & NO_TOPBAR)) {
                pushRegionToRQ(window.topbarregion, topbarcolor, layer.composingshader, &quadrq);
            }

            real32 scalefactor = 12.0f / layer.basefont->pixelheight;
            real32 yoffset = (topbarheight - 12.0f) / 2.0f;
            pushTextFromFontToRQ(
                    window.title.getCstring(), 
                    window.topbarregion.topleft + vec3f{yoffset, -yoffset, 2.0f}, 
                    window.topbarregion.width, 0.0f, 
                    vec4f{1.0f, 1.0f, 1.0f, 1.0f}, scalefactor, 
                    *layer.basefont, 
                    &quadrqt
            );
            for (ElementPtr ptr : window.children) {
                pushElementToRenderQueue(ptr, &quadrq, &quadrqt, layer);
            }
            mat4 windowprojection = OrthographicMat4(0.0f, window.region.width, 0.0f, window.region.height, -100.0f, 100.0f);
            //building the queues for the rendertarget of the window
            Rendering::RenderQueue queue;
            queue.data1 = (byte*)&quadrq[0];
            queue.count1 = quadrq.getCount();
            queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
            queue.commandtype = IME::Rendering::SIMPLE_QUAD;
            queue.depthtesting = true;
            queue.bufferstoclear = IME::IME_DEPTH_BUFFER | IME::IME_COLOR_BUFFER;
            queue.projection = windowprojection;
            queue.view = identity();
            queue.viewheight = window.region.height;
            queue.viewwidth = window.region.width;
            queue.rendertarget = window.framebuffer->id;
            queue.updatescene = true;
            queue.viewx = 0;
            queue.viewy = 0;
            queue.dealloc = true;
            renderset->renderqueues.push_back(queue);
            queue.data1 = (byte*)&quadrqt[0];
            queue.count1 = quadrqt.getCount();
            queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
            queue.commandtype = IME::Rendering::SIMPLE_QUAD;
            queue.depthtesting = true;
            queue.bufferstoclear = 0;
            queue.projection = windowprojection;
            queue.view = identity();
            queue.viewheight = window.region.height;
            queue.viewwidth = window.region.width;
            queue.rendertarget = window.framebuffer->id;
            queue.updatescene = true;
            queue.viewx = 0;
            queue.viewy = 0;    
            queue.dealloc = true;
            renderset->renderqueues.push_back(queue);

            if(!window.isdocked) {
                pushTexturedRegionToRQ({window.region.x, window.region.y, z, window.region.width, window.region.height}, ((Assets::Texture*)window.framebuffer->colorattachements[IME_COLOR_ATTACHMENT0].data)->id, layer.composingshader, &composingrq);
                z += 1.0f;
            }
        }

        //building the composing renderqueue
        Rendering::RenderQueue queue;
        queue.data1 = (byte*)&composingrq[0];
        queue.count1 = composingrq.getCount();
        queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
        queue.commandtype = IME::Rendering::SIMPLE_QUAD;
        queue.depthtesting = true;
        queue.bufferstoclear = IME::IME_DEPTH_BUFFER | IME::IME_COLOR_BUFFER;
        queue.projection = projection;
        queue.view = identity();
        queue.viewheight = platform.window.height;
        queue.viewwidth = platform.window.width;
        queue.rendertarget = 0;
        queue.updatescene = true;
        queue.viewx = 0;
        queue.viewy = 0;
        queue.dealloc = true;


        renderset->renderqueues.push_back(queue);
        return;
    }

    vec2<uint32> unzip(uint64 values) {
        return { (values >> 32) & 0xFFFFFFFF, (values) & 0xFFFFFFFF };
    }

    ///////event part propagation
    //eventrsult: 

    inline eventresult operator|(eventresult e1, eventresult e2) {
        return {e1.v1 || e2.v1, e1.v2 || e2.v2};
    }

    eventresult elementOnMouseScrollEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform);

    eventresult windowOnMouseScrollEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        Window* w = (Window*)element.data;

        if(isPointInRegion(relativemousepos, w->region)) {

            vec2f newrelativemousepos = relativemousepos - (toVec2(w->region.topleft) - vec2f{0.0f, w->region.height}); 
            
            eventresult result = {false, false};
            for(ElementPtr child : w->children) {
                result = result | elementOnMouseScrollEvent(child, e, newrelativemousepos, layer, platform);
                if(result.v1) {
                    if(result.v2) {
                        calculateWindowParameters(element, w->parameters.parentcache, *layer);
                    }
                    return {true, false};
                }
            }

            real32 oldscroll = w->scroll;
            real32 scrollspeed = 20.0f;
            real32 scroll = e.param1 == 1 ? scrollspeed : -scrollspeed;

            w->scroll += scroll;
            if(w->scroll < 0.0f) {
                w->scroll = 0.0f;
            }
            if(w->scroll > w->maxscroll) {
                w->scroll = w->maxscroll;
            }

            if(w->scroll != oldscroll || result.v2) {
                calculateWindowParameters(element, w->parameters.parentcache, *layer);
            }
            return {true, false};
        }
        return {false, false};
    }

    eventresult dockingSpaceOnMouseScrollEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        DockingSpace* d = (DockingSpace*)element.data;

        if(isPointInRegion(relativemousepos, d->parameters.contentregion)) {
            eventresult result = {false, false};
            for(auto [child, size] : d->containers) {
                result = result | elementOnMouseScrollEvent(child, e, relativemousepos, layer, platform);
                if(result.v1) {
                    return result;
                }
            }
            return result;
        }
        return {false, false};
    }

    eventresult elementOnMouseScrollEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        switch (element.type) {
            case WINDOW: return windowOnMouseScrollEvent(element, e, relativemousepos, layer, platform);
            case DOCKINGSPACE: return dockingSpaceOnMouseScrollEvent(element, e, relativemousepos, layer, platform);
        }
        return {false, false};
    }

    eventresult elementOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform);

    eventresult paragraphOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        Paragraph* p = (Paragraph*)element.data;
        if (isPointInRegion(relativemousepos, p->parameters.backgroundregion)){
            p->parameters.hovered = true;
        } else {
            p->parameters.hovered = false;
        }
        return eventresult{false, false};
    }

    eventresult windowOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        Window* w = (Window*)element.data;

        if(isPointInRegion(relativemousepos, w->region)) {

            if(isPointInRegion(relativemousepos, w->topbarregion)) {
                return {true, false};
            }

            vec2f newrelativemousepos = relativemousepos - (toVec2(w->region.topleft) - vec2f{0.0f, w->region.height}); 
            for(ElementPtr child : w->children) {
                elementOnMouseMoveEvent(child, e, newrelativemousepos, layer, platform);
            }

        }
    }

    eventresult dockingSpaceOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        DockingSpace* d = (DockingSpace*)element.data;

        if(isPointInRegion(relativemousepos, d->parameters.contentregion)) {
            if(d->iscontainerresizing) {
                if(!d->horizontal) {
                    real32 offset = 0.0f;
                    for(uint32 i = 0; i < d->edge; i++) {
                        offset += d->containers[i].v2 * d->parameters.contentregion.width;
                    }

                    real32 leftwidth = d->containers[d->edge].v2;
                    real32 rightwidth = d->containers[d->edge + 1].v2;
                    real32 totalrelativewidth = leftwidth + rightwidth;
                    real32 totalwidth = totalrelativewidth * d->parameters.contentregion.width;

                    real32 x = relativemousepos.x - d->parameters.contentregion.x - offset;

                    if(x < 0.0f + 40.0f || x > totalwidth - 40.0f) {
                        return {true, false};
                    }

                    d->containers[d->edge].v2 = (x / totalwidth) * totalrelativewidth;
                    d->containers[d->edge + 1].v2 = ((totalwidth - x) / totalwidth) * totalrelativewidth;
                    
                    calculateDockingSpaceParameters(element, d->parameters.parentcache, *layer);
                } else {
                    real32 offset = 0.0f;
                    for(uint32 i = 0; i < d->edge; i++) {
                        offset += d->containers[i].v2 * d->parameters.contentregion.height;
                    }

                    real32 leftwidth = d->containers[d->edge].v2;
                    real32 rightwidth = d->containers[d->edge + 1].v2;
                    real32 totalrelativewidth = leftwidth + rightwidth;
                    real32 totalwidth = totalrelativewidth * d->parameters.contentregion.height;

                    real32 y = d->parameters.contentregion.y - relativemousepos.y - offset;

                    if(y < 0.0f + 40.0f || y > totalwidth - 40.0f) {
                        return {true, false};
                    }

                    d->containers[d->edge].v2 = (y / totalwidth) * totalrelativewidth;
                    d->containers[d->edge + 1].v2 = ((totalwidth - y) / totalwidth) * totalrelativewidth;
                    
                    calculateDockingSpaceParameters(element, d->parameters.parentcache, *layer);
                }

            }

            for(auto [container, size] : d->containers) {
                elementOnMouseMoveEvent(container, e, relativemousepos, layer, platform);
            }
        }

    }

    eventresult elementOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        switch (element.type)
        {
        case PARAGRAPH: return paragraphOnMouseMoveEvent(element, e, relativemousepos, layer, platform); break;
        case WINDOW: return windowOnMouseMoveEvent(element, e, relativemousepos, layer, platform); break;
        case DOCKINGSPACE: return dockingSpaceOnMouseMoveEvent(element, e, relativemousepos, layer, platform); break;
        default:
            return {false, false};
        }
    }

    eventresult inputFielOnClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        InputField* f = (InputField*)element.data;

        real32 pos = f->origin + f->parameters.contentregion.x;
        real32 scalefactor = f->textsize / layer->basefont->pixelheight;

        if(isPointInRegion(relativemousepos, f->parameters.contentregion)) {
            layer->focussedinput = element;
            layer->editinginputfield = true;

             //finding the cursorposition;
            if(relativemousepos.x < f->origin + f->parameters.contentregion.x) {
                for(int32 i = f->cursorposition - 1; i >= 0; i--) {

                    real32 charlength = getCharacterLength(*(f->text.getCstring() + i), scalefactor, *layer->basefont);
                    pos -= charlength;
                    
                    if(relativemousepos.x > pos && relativemousepos.x < pos + charlength) {
                        f->origin = pos - f->parameters.contentregion.x;
                        f->cursorposition = i;
                        f->recievesinput = true;
                        break;
                    }

                }            
            } else if(relativemousepos.x > f->origin + f->parameters.contentregion.x) {
                for(int32 i = f->cursorposition; i < f->text.getLength(); i++) {

                    real32 charlength = getCharacterLength(*(f->text.getCstring() + i), scalefactor, *layer->basefont);
                    if(relativemousepos.x > pos && relativemousepos.x < pos + charlength) {
                        f->origin = pos - f->parameters.contentregion.x;
                        f->cursorposition = i;
                        f->recievesinput = true;
                        break;
                    }
                    pos += charlength;
                }
                if(!f->recievesinput) {
                    f->origin = pos - f->parameters.contentregion.x;
                    f->cursorposition = f->text.getLength();
                    f->recievesinput = true;
                }
            }
        return {true, false};
        }
        return {false, false};
    }

    eventresult elementOnMouseClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform);

    eventresult paragraphOnClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        Paragraph* p = (Paragraph*)element.data;
        vec2f mousepos = toVec2f(unzip(e.param2));
        mousepos.y = platform.window.height - mousepos.y;

        if(isPointInRegion(relativemousepos, p->parameters.backgroundregion)) {
            if(p->parameters.onmouseclick) {
                eventresult result = p->parameters.onmouseclick(element, e, layer, platform);
                Region oldregion = p->parameters.marginregion;
                if(result.v2 == true) {
                    Region newregion = calculateParagraphParameters(element, p->parameters.parentcache, *layer);
                    if(oldregion != newregion) {
                        return result;
                    }
                } else {
                    return {(bool)result.v1, false};
                }
            }
            return {false, false};
        }
        return {false, false};
    }

    eventresult windowOnClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        uint32 index = 0;
        bool32 recalculate = false;

        if(e.param1 == IME_LEFT_MB) {
            bool32 ishandled = false;   
            Window* window = (Window*)element.data;
            vec2f mousepos = relativemousepos;

            Region windowregion = window->region;
            vec2f bottomcorner = {windowregion.x + windowregion.width, windowregion.y - windowregion.height};
            
            //push onclick event to children
            if(isPointInRegion(mousepos, window->region)) {
                if(element.data != layer->basewindow.data && !window->isdocked) {
                    layer->windoworder.remove(element);
                    layer->windoworder.push_back(element);
                }
            
                //see if the topbar is clicked
                Region absolutetopbarregion = window->topbarregion;
                absolutetopbarregion.topleft = window->region.topleft;
                //window is grabbed if the topbar is clicked

                if(!(window->flags & NO_TOPBAR)) {
                    if(isPointInRegion(mousepos, absolutetopbarregion)) {
                        layer->elementgrabbing = true;
                        layer->editedelement = element;
                        layer->clickingoffset = toVec2(window->region.topleft) - mousepos;
                        if(window->isdocked) {
                            window->isdocked = false;
                            DockingSpace* d = (DockingSpace*)window->parameters.parent.data;
                            ElementPtr recalc = UI::undockWindow(element, layer);
                            if(recalc.type == WINDOW) {
                                calculateWindowParameters(recalc, ((Window*)recalc.data)->parameters.parentcache, *layer);
                            }
                            if(recalc.type == DOCKINGSPACE) {
                                calculateDockingSpaceParameters(recalc, ((DockingSpace*)recalc.data)->parameters.parentcache, *layer);
                            }
                            layer->windoworder.remove(element);
                            layer->windoworder.push_back(element);
                        }
                        return {true, false};
                    }
                }
                
                vec2f relativemousepos = mousepos - (toVec2(window->region.topleft) - vec2f{0.0f, window->region.height}); 
                for(ElementPtr child : window->children) {
                    auto[ishandled_, recalculate_] = elementOnMouseClickEvent(child, e, relativemousepos, layer, platform);

                    if(recalculate_) {
                        Region oldregion = ((ElementParameters*)child.data)->marginregion;
                        if(calculateElementParameters(child, ((ElementParameters*)child.data)->parentcache, *layer) != oldregion) {
                            recalculate = true;
                        }
                    }

                    if(ishandled_) {
                        return {true, recalculate};
                    }
                }
            
                ishandled = true;
            }

            if(window->flags & NO_RESIZING) {
                return {ishandled, recalculate};
            }
            
            //check if start with window resizing
            real32 clickingbounds = 20.0;
            Region clinckingregion = { 
                bottomcorner.x - clickingbounds / 2.0f, 
                bottomcorner.y + clickingbounds / 2.0f,
                0.0f,
                bottomcorner.width = clickingbounds,
                bottomcorner.height = clickingbounds
            };
            
            ////set check if starting with resizing
            if(isPointInRegion(mousepos, clinckingregion)) {
                layer->elementresizing = true;
                layer->editedelement = element;
                ishandled = true;
            }
            
            return {ishandled, recalculate};
        }
    }

    eventresult dockingspaceOnClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        DockingSpace* d = (DockingSpace*)element.data;
        if(isPointInRegion(relativemousepos, d->parameters.contentregion)) {

            Region edge = d->parameters.contentregion;

            real32 offset = 0.0f;
            if(!d->horizontal) {
                for(uint32 i = 0; i < ((int64)d->containers.getCount()) - 1; i++) {

                    real32 size = d->containers[i].v2;
                    offset+=size * d->parameters.contentregion.width;
                    edge.width = 20.0f;
                    edge.x = d->parameters.contentregion.x + offset - 10.0f;

                    if(isPointInRegion(relativemousepos, edge)) {
                        layer->editedcontaineredge = i;
                        layer->resizingdockedcontainer = true;
                        layer->editedelement = element;

                        d->iscontainerresizing = true;
                        d->edge = i;

                        return {true, false};
                    }
                }
            } else {
                for(uint32 i = 0; i < ((int64)d->containers.getCount()) - 1; i++) {
                    real32 size = d->containers[i].v2;
                    offset+=size * d->parameters.contentregion.height;
                    edge.height = 20.0f;
                    edge.y = d->parameters.contentregion.y - (offset - 10.0f);

                    if(isPointInRegion(relativemousepos, edge)) {
                        layer->editedcontaineredge = i;
                        layer->resizingdockedcontainer = true;
                        layer->editedelement = element;

                        d->iscontainerresizing = true;
                        d->edge = i;

                        return {true, false};
                    }
                }
            }

            eventresult result = {false, false};
            for(auto [el, size] : d->containers) {
                result = result | elementOnMouseClickEvent(el, e, relativemousepos, layer, platform);
                if(result.v1) {
                    return result;
                }
            }

            result.v1 = true;
            return result;
        }
        return {false, false};
    }

    eventresult elementOnMouseClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        switch (element.type) {
            case PARAGRAPH: return paragraphOnClickEvent(element, e, relativemousepos, layer, platform); 
            case INPUTFIELD: return inputFielOnClickEvent(element, e, relativemousepos, layer, platform); 
            case DOCKINGSPACE: return dockingspaceOnClickEvent(element, e, relativemousepos, layer, platform);
            case WINDOW: return windowOnClickEvent(element, e, relativemousepos, layer, platform);
            default: return {false, false};
        }
    }

    eventresult elementOnWindowDropEvent(ElementPtr element, ElementPtr window, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform);

    eventresult windowOnWindowDropEvent(ElementPtr element, ElementPtr window, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        Window* w = (Window*)element.data;

        eventresult result = {false, false};

        for(ElementPtr el : w->children) {
            result = result | elementOnWindowDropEvent(el, window, relativemousepos, layer, platform);
        }
        return result;
    }

    eventresult dockingSpaceOnWindowDropEvent(ElementPtr element, ElementPtr window, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        DockingSpace* d = (DockingSpace*)element.data;
        Region cr = d->parameters.contentregion;

        if(d->containers.getCount() == 0) {
                
            Region middle = {cr.topleft.x + cr.width / 2.0f - dwidth / 2.0f, cr.topleft.y - cr.height / 2.0f + dwidth / 2.0f, cr.z + 3.0f, dwidth, dwidth};
            if(isPointInRegion(relativemousepos, middle)) {
                dockWindow(window, element, false, 0, layer);

                return {true, true};
            }

        } else {

            Region left = {cr.x, cr.y - cr.height / 2.0f + dwidth / 2.0f, cr.z + 5.0f, dheight, dwidth};
            if(isPointInRegion(relativemousepos, left)) {
                dockWindow(window, element, false, 0, layer);
                return {true, true};
            }
            Region right = {cr.x + cr.width - dheight, cr.y - cr.height / 2.0f + dwidth / 2.0f, cr.z + 5.0f, dheight, dwidth};
            if(isPointInRegion(relativemousepos, right)) {
                if(d->horizontal) {
                    dockWindow(window, element, false, 1, layer);
                } else {
                    dockWindow(window, element, false, d->containers.getCount(), layer);
                }
                return {true, true};
            }

            Region top = {cr.x + cr.width / 2.0f - dwidth / 2.0f, cr.y, cr.z + 5.0f, dwidth, dheight}; 
            if(isPointInRegion(relativemousepos, top)) {
                dockWindow(window, element, true, 0, layer);
                return {true, true};
            }
            Region bottom = {cr.x + cr.width / 2.0f - dwidth / 2.0f, cr.y - cr.height + dheight, cr.z + 5.0f, dwidth, dheight};
            if(isPointInRegion(relativemousepos, bottom)) {
                if(!d->horizontal) {
                    dockWindow(window, element, true, 1, layer);
                } else {
                    dockWindow(window, element, true, d->containers.getCount(), layer);
                }
                return {true, true};
            }

            eventresult eresult = {false, false};
            for(auto [container, size] : d->containers) {
                if(container.type == DOCKINGSPACE) {
                    eresult = eresult | dockingSpaceOnWindowDropEvent(container, window, relativemousepos, layer, platform);
                }
            }
            return eresult;
        }
        return {false, false};

    }

    eventresult elementOnWindowDropEvent(ElementPtr element, ElementPtr window, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        switch (element.type)
        {
        case WINDOW: return windowOnWindowDropEvent(element, window, relativemousepos, layer, platform);
        case DOCKINGSPACE: return dockingSpaceOnWindowDropEvent(element, window, relativemousepos, layer, platform);
        default: return {false, false};
        }
    }


    bool32 
    propagateEventToLayer(Event e, Layer* layer, const PlatformInterface& platform) {

        if(e.type == EventType::IME_MOUSE_SCROLLED) {
            vec2f mousepos = toVec2f(platform.mouse.relativemousepos);
            mousepos.y = ((real32)platform.window.height - mousepos.y);

            eventresult result = {false, false};
            for (ElementPtr window : layer->windoworder) {
                if(!((Window*)window.data)->isdocked) {
                    windowOnMouseScrollEvent(window, e, mousepos, layer, platform);
                }
            }
        }
        
        if(e.type == EventType::IME_MOUSE_BUTTON_PRESSED) {

            if(e.param1 == IME_LEFT_MB) {
                //edit inputfield

                if(layer->editinginputfield) {
                    InputField *f = (InputField*)layer->focussedinput.data;
                    f->recievesinput = false;
                }

                layer->focussedinput = {0, NONE};
                layer->editedelement = {0, NONE};

                layer->elementresizing = false;
                layer->elementgrabbing = false;
                layer->editinginputfield = false;
                layer->resizingdockedcontainer = false;
            }

            for(int32 i = layer->windoworder.getCount() - 1; i >= 0; i--) {
                
                ElementPtr w = layer->windoworder[i];
                
                if(!((Window*)w.data)->isdocked) {
                    vec2f mousepos = toVec2f(platform.mouse.relativemousepos);
                    mousepos.y = ((real32)platform.window.height - mousepos.y);
                    
                    auto [ishandled, recalculate] = windowOnClickEvent(w, e, mousepos, layer, platform);
                    if(recalculate) {
                        calculateWindowParameters(w, ((ElementParameters*)w.data)->parentcache, *layer);
                    }
                    if(ishandled) {
                        break;
                    }
                }
            }
        }

        if(e.type == IME_KEY_PRESSED) {

            if(layer->editinginputfield == true) {
                InputField* f = (InputField*)layer->focussedinput.data;

                real32 scalefactor = f->textsize / layer->basefont->pixelheight;

                char ascii = (char)(e.param2 >> 32);

                if(e.param1 == IME_LEFT) {

                    if(f->cursorposition > 0) {
                        if(platform.keyboard.isKeyPressed(IME_CONTROL)) {
                            char* strptr = f->text.getCstring();
                            for (uint32 i = f->cursorposition - 1; i >= 0; i--) {
                                f->origin -= getCharacterLength(strptr[i], scalefactor, *layer->basefont);
                                if(strptr[i] == ' ' || i == 0) {
                                    f->cursorposition = i;
                                    break;
                                }
                            }
                        } else {
                            f->cursorposition -= 1;
                            f->origin -= getCharacterLength(f->text[f->cursorposition], scalefactor, *layer->basefont);
                        }

                    }
                }
                else if(e.param1 == IME_RIGHT) {
                    if(f->cursorposition < f->text.getLength()) {
                        if(platform.keyboard.isKeyPressed(IME_CONTROL)) {
                            char* strptr = f->text.getCstring();
                            for (uint32 i = f->cursorposition; i < f->text.getLength(); i++) {
                                f->origin += getCharacterLength(strptr[i], scalefactor, *layer->basefont);
                                if(strptr[i] == ' ' || i == f->text.getLength() - 1) {
                                    f->cursorposition = i + 1;
                                    break;
                                }
                            }
                        } else {
                            f->cursorposition += 1;
                            f->origin += getCharacterLength(f->text[f->cursorposition - 1], scalefactor, *layer->basefont);
                        }
                    }
                } else if(e.param1 == IME_BACK) {
                    if(f->cursorposition > 0) {
        
                        //if ctrl is pressed then it should remove entire words
                        if(platform.keyboard.isKeyPressed(IME_CONTROL)) {
                            char* strptr = f->text.getCstring();
                            for (uint32 i = f->cursorposition - 1; i >= 0; i--) {
                                f->origin -= getCharacterLength(strptr[i], scalefactor, *layer->basefont);
                                if(strptr[i] == ' ' || i == 0) {
                                    f->text.remove(i, f->cursorposition);
                                    f->cursorposition = i;
                                    break;
                                }
                            }
                        } else {
                            char c = f->text[f->cursorposition - 1];
                            f->text.remove(f->cursorposition - 1);

                            f->cursorposition -= 1;
                            f->origin -= getCharacterLength(c, scalefactor, *layer->basefont);
                        }
                    }
                } 
                else if(ascii >= 32 && ascii <= 126) {
                    f->text.insert(ascii, f->cursorposition);
                    f->cursorposition++;
                    f->origin += getCharacterLength(ascii, scalefactor, *layer->basefont);
                }
                if(f->origin > f->parameters.contentregion.width) {
                    f->origin = f->parameters.contentregion.width;
                }
                if(f->origin < 0.0f) {
                    f->origin = 0.0f;
                }
                calculateInputFieldParameters(layer->focussedinput, f->parameters.parentcache, *layer);
            }
        }

        if(e.type == IME_WINDOW_RESIZE) {
            Window* window = (Window*)layer->basewindow.data;
            window->region.topleft = {0.0f, (real32)e.param2, -1.0f};
            window->region.size = {(real32)e.param1, (real32)e.param2};

            ElementConstraints parent = window->parameters.parentcache;
            calculateWindowParameters(layer->basewindow, parent, *layer);
        }

        if(e.type == IME_MOUSE_BUTTON_RELEASED) {
            if(layer->elementgrabbing && layer->editedelement.type == UI::WINDOW) {
                
                real32 posx = (real32)platform.mouse.relativemousepos.x;
                real32 posy = platform.window.height - (real32)platform.mouse.relativemousepos.y;
                vec2f mousepos = {posx, posy};


                for(ElementPtr window : layer->windoworder) {
                    Window* w = (Window*)window.data;
                    if(isPointInRegion(mousepos, w->region)) {

                        vec2f relativemousepos = mousepos - (toVec2(w->region.topleft) - vec2f{0.0f, w->region.height});
                        eventresult result = windowOnWindowDropEvent(window, layer->editedelement, relativemousepos, layer, platform);

                        if(result.v2 == true) {
                            calculateWindowParameters(window, w->parameters.parentcache, *layer);
                        }

                        if(result.v1 == true) {
                            break;
                        }

                    }
                }

            }

            if(e.param1 == IME_LEFT_MB) {
                if(layer->editedelement.type == DOCKINGSPACE) {
                    DockingSpace* d = (DockingSpace*)layer->editedelement.data;
                    d->iscontainerresizing = false;
                    d->edge = 0;
                }
                layer->editedelement = {0, NONE};
                layer->elementresizing = false;
                layer->elementgrabbing = false;
                layer->resizingdockedcontainer = false;

                
            }

        }

        if(e.type == IME_MOUSE_MOVED) {
            real32 posx = (real32)e.param1;
            real32 posy = platform.window.height - (real32)e.param2;
            vec2f mousepos = {posx, posy};

            //check wether there are window children are hovered
            for(uint32 i = 0; i < layer->windows.getCount(); i++) {

                Window* window = layer->windows[i];

                if(isPointInRegion({posx, posy}, window->region)) { window->hovered = true; }
                else { window->hovered = false; }
                bool32 recalculate = false;

                for(ElementPtr child : window->children) {
                    vec2f relativemousepos = mousepos - (toVec2(window->region.topleft) - vec2f{0.0f, window->region.height}); 
                    auto[ishandled, recalculate_] = elementOnMouseMoveEvent(child, e, relativemousepos, layer, platform);
                    recalculate |= recalculate_;
                }
                if(recalculate) {
                    calculateElementsForWindow({(void*)window, WINDOW}, *layer);
                }
            }

            if(layer->editedelement.type == UI::WINDOW) {
                Window* window = (Window*)layer->editedelement.data;

                if(layer->elementresizing) {
                    window->region.width = posx - window->region.x > 30.0f ?  posx - window->region.x : 30.0f;
                    window->region.height = window->region.y - posy > 30.0f ? window->region.y - posy : 30.0f;

                    calculateWindowParameters(layer->editedelement, window->parameters.parentcache, *layer);

                    return true;
                }
                if(layer->elementgrabbing) {
                    window->region.x = posx + layer->clickingoffset.x;
                    window->region.y = posy + layer->clickingoffset.y;
                    return true;
                }

            }
        }
        return false;
    }
}

