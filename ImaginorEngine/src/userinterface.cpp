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
        if(stringCompare(extbuffer, "px")) { return {stringToFloat(buffer), Extension::PIXELS};}
        if(stringCompare(extbuffer, "%"))  { return {stringToFloat(buffer), Extension::PERCENTAGE}; }
        if(stringCompare(extbuffer, "")) {return {stringToFloat(buffer), Extension::NOEXIST}; }
        return {stringToFloat(buffer), Extension::FALSE};
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

    bool32 addConstraint(ElementPtr element, ElementConstraintType type, const char* value, Layer* uilayer) {

        ElementConstraint constraint;
        constraint.type = type;
        constraint.value.set(value);

        switch (element.type)
        {
        case PARAGRAPH:
            uilayer->paragraphs[element.offset].data.elementconstraints.push_back(constraint); return true;
        case INPUTFIELD:
            uilayer->inputfields[element.offset].data.elementconstraints.push_back(constraint); return true;
        case IMAGE:
            uilayer->images[element.offset].data.elementconstraints.push_back(constraint); return true;
        default:
            return false;
        }
    }

    Layer 
    createLayer(gl_id composingshader, const PlatformInterface& platform, Assets::Font* basefont) {
        Layer layer;
        layer.windows.init(1);
        layer.paragraphs.init(1);
        layer.inputfields.init(1);
        layer.images.init(1);
        layer.basefont = basefont;
        layer.composingshader = composingshader;

        layer.windoworder.init(0);
        
        Region baseregion;
        baseregion.topleft = {0.0f, (real32)platform.window.width, -1.0f };
        baseregion.size = { (real32)platform.window.height, (real32)platform.window.width };
        layer.basewindow = addWindowToLayer(baseregion, composingshader, "basewindow", platform, &layer);

        return layer;
    }

    global_var real32 topbarheight = 20;
    global_var vec4f topbarcolor = {0.3f, 0.3f, 0.7f, 1.0f};

    ElementPtr addParagraph(const char* text, ElementPtr parent, Layer* layer) {

        Paragraph result;
        result.text.set(text);
        result.elementconstraints.init(0);
        result.parameters.parent = parent;

        uint32 offset = (uint32)layer->paragraphs.add(result);
        ElementPtr ptr = {offset, UI::PARAGRAPH};

        if(parent.type == UI::WINDOW) {
            Window* parent_ = &layer->windows[parent.offset].data;
            parent_->children.push_back(ptr);
        }

        return ptr;
    }

    ElementPtr addInputField(const char* initialtext, ElementPtr parent, Layer* uilayer) {
        InputField result;
        result.text.set(initialtext);
        result.parameters.parent = parent;
        result.elementconstraints.init(0);
        result.cursorposition = 0;
        result.origin = 0.0f;
        
        uint32 offset = (uint32)uilayer->inputfields.add(result);
        ElementPtr ptr = {offset, UI::INPUTFIELD};

        if(parent.type == UI::WINDOW) {
            Window* parent_ = &uilayer->windows[parent.offset].data;
            parent_->children.push_back(ptr);
        }

        return ptr;
    }

    ElementPtr addImage(Assets::Texture* image, ElementPtr parent, Layer* uilayer) {
        Image result;
        result.onmouseclick = nullptr;
        result.texture = image;
        result.elementconstraints.init(0);

        uint32 offset = (uint32)uilayer->images.add(result);
        ElementPtr ptr = {offset, UI::IMAGE};

        if(parent.type == UI::WINDOW) {
            Window* parent_ = &uilayer->windows[parent.offset].data;
            parent_->children.push_back(ptr);
        }

        return ptr;
    }

    Region subtractBordersFromRegion(Region region, const vec4f& borders);

    ElementPtr addWindowToLayer(Region windowregion, gl_id shader, const char* title, const PlatformInterface& platform, Layer* layer) {
        Window window;
        window.region = windowregion;

        window.topbarregion = { 0.0f, windowregion.height, 0.0f, windowregion.width, topbarheight};
        window.borderregion = {0.0f, windowregion.height - topbarheight, 0.0f, windowregion.width, windowregion.height - topbarheight};
        window.contentregion = subtractBordersFromRegion(window.borderregion, {1.0f, 1.0f, 0.0f, 1.0f});
        window.contentregion.z += 0.2f;

        window.shader = shader;
        window.children.init(0);
        window.title.set(title);

        window.framebuffer = platform.gfx.fbo_create((uint32)window.region.width, (uint32)window.region.height);

        TextureProperties props;
        props.format = IME_RGBA;
        props.generatemipmaps = false;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;

        window.colorbuffer = platform.gfx.fbo_createtextureattachment(gsframebufferattachmenttype::IME_COLOR_ATTACHMENT0, props);
        window.depthbuffer = platform.gfx.fbo_createrenderbufferattachment(gstextureformat::IME_DEPTH_COMPONENT32);

        sizeptr offset = layer->windows.add(window);
        ElementPtr result = {(uint32)offset, ElementType::WINDOW};
        layer->windoworder.push_back(result);

        return result;
    }

    bool32 addOnClickEventHandler(ElementPtr element, on_mouse_click* handler, Layer* uilayer) {

        switch(element.type) {
            case PARAGRAPH: { Paragraph* p = &uilayer->paragraphs[element.offset].data; p->onmouseclick = handler; return true; }
            case INPUTFIELD: { InputField* f = &uilayer->inputfields[element.offset].data; f->onmouseclick = handler; return true; }
            default:
                return false;
        }
    }

    bool32 addOnResizeEventHandler(ElementPtr element, on_resize* handler, Layer* uilayer) {
        switch(element.type) {
            case WINDOW: { Window* w = &uilayer->windows[element.offset].data; w->onresize = handler; return true; }
            default:
                return false;
        }
    }

    Region getContentRegion(ElementPtr element, const Layer& uilayer) {

        switch(element.type) {
            case WINDOW: {Window w = uilayer.windows[element.offset].data; return w.contentregion;}
        }

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

    inline Region 
    calculateWindowParameters(ElementPtr ptr, ElementConstraints parentinfo, const Layer& uilayer) {

        Window* window = &uilayer.windows[ptr.offset].data;

        //reset window region
        Region windowregion = window->region;
        window->topbarregion = { 0.0f, windowregion.height, 0.0f, windowregion.width, topbarheight};
        window->borderregion = {0.0f, windowregion.height - topbarheight, 0.0f, windowregion.width, windowregion.height - topbarheight};
        window->contentregion = subtractBordersFromRegion(window->borderregion, {1.0f, 1.0f, 0.0f, 1.0f});
        window->contentregion.z += 0.2f;

        parentinfo.availableregion = window->contentregion;
        parentinfo.availableregion.height = INFINITY;
        parentinfo.availableregion.z++;

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

            
        };

        return {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
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
    calculateParagraphParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer) {

        Paragraph* paragraph = &uilayer.paragraphs[ptr.offset].data;
        paragraph->parameters.parentcache = parentinfo;

        ElementConstraints constraints = determineElementConstraints(parentinfo, paragraph->elementconstraints);

        //determine contentregion;
        paragraph->parameters.contentregion = calcvailableContentRegion(constraints);


        //push the font to the layer
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

        InputField* f = &uilayer.inputfields[ptr.offset].data;

        f->parameters.parentcache = parentinfo;
        ElementConstraints constraints = determineElementConstraints(parentinfo, f->elementconstraints);

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

        UI::Image* im = &uilayer.images[ptr.offset].data;

        im->parameters.parentcache = parentinfo;
        ElementConstraints constraints = determineElementConstraints(parentinfo, im->elementconstraints);

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

    Region 
    calculateElementParameters(ElementPtr ptr, const ElementConstraints& parentinfo, const Layer& uilayer) {

        if(ptr.type == UI::WINDOW) { return calculateWindowParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::PARAGRAPH) { return calculateParagraphParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::INPUTFIELD) { return calculateInputFieldParameters(ptr, parentinfo, uilayer); }
        if(ptr.type == UI::IMAGE) { return calculateImageParameters(ptr, parentinfo, uilayer); }
        return parentinfo.availableregion;
    }

    inline void 
    pushParagraphToRenderQueue(
            ElementPtr ptr, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrq, 
            Arraylist<Rendering::SimpleQuadCommand>* quadrqt, 
            const Layer& uilayer
    ) {

        Paragraph paragraph = uilayer.paragraphs[ptr.offset].data;
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

        InputField f = uilayer.inputfields[ptr.offset].data;
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
        Image e = layer.images[ptr.offset].data;

        pushTexturedRegionToRQ(e.parameters.contentregion, e.texture->id, layer.composingshader, quadrq);

        if(e.parameters.bordercolor.w > 0.0f) {
            pushRegionToRQ(e.parameters.borderregion, e.parameters.bordercolor, layer.composingshader, quadrq);
        }
        if(e.parameters.backgroundcolor.w > 0.0f) {
            pushRegionToRQ(e.parameters.backgroundregion, e.parameters.backgroundcolor, layer.composingshader, quadrq);
        }
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
    }

    void calculateElementsForWindow(ElementPtr window, const Layer& layer) {
        ElementConstraints constraints;
        calculateElementParameters(window, constraints, layer);
    }

    void pushLayerToRenderSet(const Layer& layer, Rendering::RenderSet* renderset, const mat4& projection, const PlatformInterface& platform) {

        Arraylist<Rendering::SimpleQuadCommand> composingrq;            
        composingrq.init(0);

        real32 z = 0.0f;
        for(ElementPtr w : layer.windoworder) {
            auto ds = layer.windows[w.offset];
            if(ds.isoccupied) {
                
                //initializing the local renderqueues
                Arraylist<Rendering::SimpleQuadCommand> quadrq;            
                quadrq.init(0);

                Arraylist<Rendering::SimpleQuadCommand> quadrqt;            
                quadrqt.init(0);

                pushRegionToRQ(ds.data.topbarregion, topbarcolor, ds.data.shader, &quadrq);
                pushRegionToRQ(ds.data.borderregion, topbarcolor, ds.data.shader, &quadrq);
                pushRegionToRQ(ds.data.contentregion, {1.0f, 1.0f, 1.0f, 1.0f}, ds.data.shader, &quadrq);

                real32 scalefactor = 12.0f / layer.basefont->pixelheight;
                real32 yoffset = (topbarheight - 12.0f) / 2.0f;
                pushTextFromFontToRQ(
                        ds.data.title.getCstring(), 
                        ds.data.topbarregion.topleft + vec3f{yoffset, -yoffset, 1.0f}, 
                        ds.data.topbarregion.width, 0.0f, 
                        vec4f{1.0f, 1.0f, 1.0f, 1.0f}, scalefactor, 
                        *layer.basefont, 
                        &quadrqt
                );

                for (ElementPtr ptr : ds.data.children) {
                    pushElementToRenderQueue(ptr, &quadrq, &quadrqt, layer);
                }

                mat4 windowprojection = OrthographicMat4(0.0f, ds.data.region.width, 0.0f, ds.data.region.height, -100.0f, 100.0f);

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

                queue.viewheight = ds.data.region.height;
                queue.viewwidth = ds.data.region.width;
                queue.rendertarget = ds.data.framebuffer;
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

                queue.viewheight = ds.data.region.height;
                queue.viewwidth = ds.data.region.width;
                queue.rendertarget = ds.data.framebuffer;
                queue.updatescene = true;
                queue.viewx = 0;
                queue.viewy = 0;
                queue.dealloc = true;

                renderset->renderqueues.push_back(queue);

                pushTexturedRegionToRQ({ds.data.region.x, ds.data.region.y, z, ds.data.region.width, ds.data.region.height}, ds.data.colorbuffer, layer.composingshader, &composingrq);
                z += 1.0f;
            }
            
        }

        char buffer[128];
        sprintf_s(buffer, 128, "mousepos: (%i, %i)", platform.mouse.relativemousepos.x, platform.mouse.relativemousepos.y);
        pushTextFromFontToRQ(buffer, {0.0f, (real32)platform.window.height, 10.0f}, 500.0f, 0.0f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, *layer.basefont, &composingrq);

        //pushRegionToRQ({300.0f, 300.0f, 5.0f, 200.0f, 200.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, layer.composingshader, &composingrq);

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
    eventresult paragraphOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {

        Paragraph* p = &layer->paragraphs[element.offset].data;
        if (isPointInRegion(relativemousepos, p->parameters.backgroundregion)){
            p->parameters.hovered = true;
        } else {
            p->parameters.hovered = false;
        }
        return eventresult{false, false};
    }

    eventresult elementOnMouseMoveEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        switch (element.type)
        {
        case PARAGRAPH: 
            return paragraphOnMouseMoveEvent(element, e, relativemousepos, layer, platform); 
            break;
        default:
            return {false, false};
        }
    }

    eventresult inputFielOnClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        InputField* f = &layer->inputfields[element.offset].data;

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
        }
        return {false, true};
    }

    eventresult paragraphOnClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        Paragraph* p = &layer->paragraphs[element.offset].data;
        vec2f mousepos = toVec2f(unzip(e.param2));
        mousepos.y = platform.window.height - mousepos.y;

        if(isPointInRegion(relativemousepos, p->parameters.backgroundregion)) {
            if(p->onmouseclick) {
                eventresult result = p->onmouseclick(element, e, layer, platform);
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

    eventresult elementOnMouseClickEvent(ElementPtr element, Event e, vec2f relativemousepos, Layer* layer, const PlatformInterface& platform) {
        switch (element.type) {
            case PARAGRAPH: return paragraphOnClickEvent(element, e, relativemousepos, layer, platform); 
            case INPUTFIELD: return inputFielOnClickEvent(element, e, relativemousepos, layer, platform); 
            default: return {false, false};
        }
    }

    bool32 
    propagateEventToLayer(Event e, Layer* layer, const PlatformInterface& platform) {
        
        if(e.type == EventType::IME_MOUSE_BUTTON_PRESSED) {
            uint32 index = 0;

            for(int32 i = layer->windoworder.getCount() - 1; i >= 0; i--) {

                ElementPtr w = layer->windoworder[i];
                
                bool32 ishandled;   
                auto&[window, occupied] = layer->windows[w.offset];

                vec2f mousepos = toVec2f(unzip(e.param2));
                mousepos.height = (real32)platform.window.height - mousepos.height;

                //do caculation is case of left mouse button pressed
                if(e.param1 == IME_LEFT_MB) {
                    real32 clickingbounds = 20.0;
                    Region windowregion = window.region;
                    vec2f bottomcorner = {windowregion.x + windowregion.width, windowregion.y - windowregion.height};

                    //edit inputfield
                    if(layer->editinginputfield) {
                        layer->focussedinput = {0, NONE};
                        layer->editinginputfield = false;
                        InputField *f = &layer->inputfields[layer->focussedinput.offset].data;
                        f->recievesinput = false;
                    }

                    //check if start with window grabbing
                    Region clinckingregion = { 
                        bottomcorner.x - clickingbounds / 2.0f, 
                        bottomcorner.y + clickingbounds / 2.0f,
                        0.0f,
                        bottomcorner.width = clickingbounds,
                        bottomcorner.height = clickingbounds 
                    };
                    Region absolutetopbarregion = window.topbarregion;
                    absolutetopbarregion.topleft = window.region.topleft;

                    if(isPointInRegion(mousepos, absolutetopbarregion)) {
                        layer->elementgrabbing = true;
                        layer->editedelement = w;
                        layer->clickingoffset = toVec2(window.region.topleft) - mousepos;
                    }

                    ////set check if starting with resizing
                    else if(isPointInRegion(mousepos, clinckingregion)) {
                        layer->elementresizing = true;
                        layer->editedelement = w;
                    }
                }

                //push onclick event to children
                if(isPointInRegion(mousepos, window.region)) {
                    bool32 recalculate = false;
                    vec2f relativemousepos = mousepos - (toVec2(window.region.topleft) - vec2f{0.0f, window.region.height}); 
                    for(ElementPtr child : window.children) {
                        auto[ishandled, recalculate_] = elementOnMouseClickEvent(child, e, relativemousepos, layer, platform);
                        recalculate |= recalculate_;
                    }
                    if(recalculate) {
                        calculateElementsForWindow(w, *layer);
                    }

                    if(w.offset != 0) {
                        layer->windoworder.remove(i);
                        layer->windoworder.push_back(w);
                        ishandled = true;
                    }
                }

                if(ishandled) {
                    break;
                }
            }
        }

        if(e.type == IME_KEY_PRESSED) {

            if(layer->editinginputfield == true) {
                InputField* f = &layer->inputfields[layer->focussedinput.offset].data;

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
            Window* window = &layer->windows[layer->basewindow.offset].data;
            window->region.topleft = {0.0f, (real32)e.param2, -1.0f};
            window->region.size = {(real32)e.param1, (real32)e.param2};
            platform.gfx.fbo_bind(window->framebuffer);

            platform.gfx.fbo_resize(window->region.width, window->region.height);
            //calculateElementsForWindow(layer->editedelement, *layer);
            ElementConstraints parent;
            calculateWindowParameters(layer->basewindow, parent, *layer);

            if(window->onresize) {
                window->onresize(layer->basewindow, {window->contentregion.width, window->contentregion.height}, layer, platform);
            }
        }

        if(e.type == IME_MOUSE_BUTTON_RELEASED) {
            if(e.param1 == IME_LEFT_MB) {

                layer->editedelement = {0, NONE};
                layer->elementresizing = false;
                layer->elementgrabbing = false;

            }
        }

        if(e.type == IME_MOUSE_MOVED) {
            real32 posx = (real32)e.param1;
            real32 posy = platform.window.height - (real32)e.param2;
            vec2f mousepos = {posx, posy};
            for(uint32 i = 0; i < layer->windows.getCount(); i++) {
                auto&[window, occupied] = layer->windows.getUnchecked(i);
                if(occupied) {
                    if(isPointInRegion({posx, posy}, window.region)) { window.hovered = true; }
                    else { window.hovered = false; }

                    bool32 recalculate = false;
                    for(ElementPtr child : window.children) {
                        vec2f relativemousepos = mousepos - (toVec2(window.region.topleft) - vec2f{0.0f, window.region.height}); 
                        auto[ishandled, recalculate_] = elementOnMouseMoveEvent(child, e, relativemousepos, layer, platform);
                        recalculate |= recalculate_;
                    }
                    if(recalculate) {
                        calculateElementsForWindow({i, WINDOW}, *layer);
                    }
                }
            }

            if(layer->editedelement.type == UI::WINDOW) {
                Window* window = &layer->windows[layer->editedelement.offset].data;

                if(layer->elementresizing) {
                    window->region.width = posx - window->region.x > 30.0f ?  posx - window->region.x : 30.0f;
                    window->region.height = window->region.y - posy > 30.0f ? window->region.y - posy : 30.0f;

                    Region windowregion = window->region;
                    window->topbarregion = { 0.0f, windowregion.height, 0.0f, windowregion.width, topbarheight};
                    window->borderregion = {0.0f, windowregion.height - topbarheight, 0.0f, windowregion.width, windowregion.height - topbarheight};
                    window->contentregion = subtractBordersFromRegion(window->borderregion, {1.0f, 1.0f, 0.0f, 1.0f});
                    window->contentregion.z += 0.2f;

                    if(window->onresize) {
                        window->onresize(layer->basewindow, {window->contentregion.width, window->contentregion.height}, layer, platform);
                    }

                    platform.gfx.fbo_bind(window->framebuffer);
                    platform.gfx.fbo_resize(window->region.width, window->region.height);
                    calculateElementsForWindow(layer->editedelement, *layer);

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

