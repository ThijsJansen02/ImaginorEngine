#include "context.h"
#include "Window.h"
#include <stdio.h>
#include "src/textrenderering.h"
#include "../functionality.h"

namespace IME::UI {

    real32 slidervaluesize = 6.0f;

    template<typename T>
    Bounds addBorderToBounds(const Bounds& bounds, const T& border) {
        Bounds result = bounds;
        result.top += border.top;
        result.left -= border.left;
        result.right += border.right;
        result.bottom -= border.bottom;

        return result;
    }

    template<typename T>
    Bounds subtractBorderFromBounds(const Bounds& bounds, const T& border) {
        Bounds result = bounds;
        result.top -= border.top;
        result.left += border.left;
        result.right -= border.right;
        result.bottom += border.bottom;

        return result;
    }

    Context createContext() {
        Context result = {};
        result.divs = UiElementList<Div>::createFragmentedArrayList(1);
        result.paragraphs = UiElementList<Paragraph>::createFragmentedArrayList(1);
        result.floatsliders = UiElementList<FloatSlider>::createFragmentedArrayList(1);
        result.isresizing = false;
        return result;
    }

    inline 
    mat4 calcTransformFromBounds(const Bounds& bounds, real32 depth) {

        IME::vec2f center = centerOfBounds(bounds);
        IME::vec2f size = sizeOfBounds(bounds);
        return transformMat4(vec3f{center.x, center.y, depth}, vec3f{size.x, size.y, 1.0f});

    }

    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style, const char* id) {
        Paragraph p;
        p.parent = parent;
        p.textlength = strlen(text);
        p.text = (char*)Memory::alloc(p.textlength + 1);
        copy((byte*)text, (byte*)p.text, p.textlength + 1);

        p.props.id = IME::copyString(id);

        //all style related things
        p.glyphsize = style.glyphsize;
        p.atlas = style.font;
        p.linespacing = style.linespacing;
        p.textcolor = style.textcolor;
        p.props.background = style.background;
        p.props.border = style.border;
        p.props.bordercolor = style.bordercolor;
        p.props.padding = style.padding;
        p.props.shader = style.shader;
        p.props.margin = style.margin;
        p.props.width = style.width;

        ElementPtr result;
        result.dataptr = context->paragraphs.add(p);
        result.type = UI_PARAGRAPH;
        if(parent.type == UI_DIV) {
            context->divs[parent.dataptr].data.children.push_back(result);
        }
        return result;
    }

    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style) {
        return addParagraph(context, parent, text, style, "");
    }

    ElementPtr addDiv(Context* context, ElementPtr parent, const StyleProperties& style, const char* id) {
        Div div;
        //all style related things
        div.props.background = style.background;
        div.props.border = style.border;
        div.props.bordercolor = style.bordercolor;
        div.props.padding = style.padding;
        div.props.shader = style.shader;
        div.props.margin = style.margin;
        div.children = Data::ArrayList_<ElementPtr, Memory::alloc, Memory::dealloc>::create(0);
        div.props.width = style.width;

        div.props.id = IME::copyString(id);

        ElementPtr result;
        result.dataptr = context->divs.add(div);
        result.type = UI_DIV;
        if(parent.type == UI_DIV) {
            context->divs[parent.dataptr].data.children.push_back(result);
        }
        return result;
    }

    ElementPtr addDiv(Context* context, ElementPtr parent, const StyleProperties& style) {
        return addDiv(context, parent, style, "");
    }

    ElementPtr addFloatSlider(Context* context, ElementPtr parent, const StyleProperties& style, uint32 nfloats, real32* values, const char* tag, const char* id) {
        FloatSlider fs;

        fs.value = values;
        fs.nvalues = nfloats;
        fs.glyphsize = style.glyphsize;
        fs.atlas = style.font;
        
        fs.taglength = strlen(tag);
        fs.tag = (char*)Memory::alloc(fs.taglength + 1);
        copy((byte*)tag, (byte*)fs.tag, fs.taglength + 1);

        //all style related things
        fs.props.background = style.background;
        fs.props.border = style.border;
        fs.props.bordercolor = style.bordercolor;
        fs.props.padding = style.padding;
        fs.props.shader = style.shader;
        fs.props.margin = style.margin;
        fs.props.width = style.width;

        fs.props.id = copyString(id);

        ElementPtr result;
        result.dataptr = context->floatsliders.add(fs);
        result.type = UI_FLOAT_SLIDER;
        if(parent.type == UI_DIV) {
            context->divs[parent.dataptr].data.children.push_back(result);
        }
        return result;
    }

    ElementPtr addFloatSlider(Context* context, ElementPtr parent, const StyleProperties& style, uint32 nfloats, real32* values, const char* tag) {
        return addFloatSlider(context, parent, style, nfloats, values, tag, "");
    }

    bool32 addOnClickToElement(ElementPtr elptr, Context* context, onClickCallback* callback) {
        if(elptr.type == UI_PARAGRAPH) {
            Paragraph* el = &context->paragraphs[elptr.dataptr].data;
            el->props.onclick = callback;
            return true;
        }
        if(elptr.type == UI_FLOAT_SLIDER) {
            FloatSlider* el = &context->floatsliders[elptr.dataptr].data;
            el->props.onclick = callback;
            return true;
        }
        if(elptr.type == UI_DIV) {
            Div* el = &context->divs[elptr.dataptr].data;
            el->props.onclick = callback;
            return true;
        }
    }

    bool32 addOnHoverToElement(ElementPtr elptr, Context* context, onHoverCallback* callback) {
        if(elptr.type == UI_PARAGRAPH) {
            Paragraph* el = &context->paragraphs[elptr.dataptr].data;
            el->props.onHover = callback;
            return true;
        }
        if(elptr.type == UI_FLOAT_SLIDER) {
            FloatSlider* el = &context->floatsliders[elptr.dataptr].data;
            el->props.onHover = callback;
            return true;
        }
        if(elptr.type == UI_DIV) {
            Div* el = &context->divs[elptr.dataptr].data;
            el->props.onHover = callback;
            return true;
        }
        return false;
    }

    bool32 addOfHoverToElement(ElementPtr elptr, Context* context, ofHoverCallback* callback) {
        if(elptr.type == UI_PARAGRAPH) {
            Paragraph* el = &context->paragraphs[elptr.dataptr].data;
            el->props.ofHover = callback;
            return true;
        }
        if(elptr.type == UI_FLOAT_SLIDER) {
            FloatSlider* el = &context->floatsliders[elptr.dataptr].data;
            el->props.ofHover = callback;
            return true;
        }
        if(elptr.type == UI_DIV) {
            Div* el = &context->divs[elptr.dataptr].data;
            el->props.ofHover = callback;
            return true;
        }
    }

    Bounds calculateUiComponent(Context* context, ElementPtr element, ElementPtr parent, Bounds maxbounds, real32 depth);

    inline vec2f 
    mouseSpaceToUispace(vec2<int32> in) {
        return vec2f{(real32)in.x, -(real32)in.y};
    }

    inline bool32 
    isBetween(vec2f test, vec2f topleft, vec2f bottomright) {
        return test.x > topleft.x && test.x < bottomright.x && test.y < topleft.y && test.y > bottomright.y;
    }

    IME::bool32 onMouseMovedEvent(ElementPtr element, Context* context, vec2f mousepos, Event e);
    IME::bool32 onMBpressedEvent(ElementPtr element, Context* context, vec2f mousepos, Event e);

    global_var real32 barheight_ = 15.0f;
    void uiOnEvent(Context* context, Event e, const PlatformInterface& platform) {

        IME::vec2f buttonbounds = {5.0f, -5.0f};
        if(e.type == IME_MOUSE_BUTTON_PRESSED) {

            IME::vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos);

            if(e.param1 == IME_LEFT_MB) {

                for(uint32 i = 0; i < context->uiwindows.getCount(); i++) {

                    Window* window = &context->uiwindows[i];
                    if(isBetween(mousepos, window->bounds.bottomright - buttonbounds, window->bounds.bottomright + buttonbounds)) {
                        context->isresizing = true;
                        context->selectedWindowptr = i;
                    }

                    Bounds topbar = {};
                    topbar.topleft = window->bounds.topleft;
                    topbar.bottomright = {window->bounds.bottomright.x, window->bounds.topleft.y - barheight_};

                    if(isInBounds(mousepos, topbar)) {
                        context->isgrabbing = true;
                        context->selectedWindowptr = i;
                        context->relativegrab = mousepos - window->bounds.topleft;
                    }

                    //vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos
                    if (isInBounds(mousepos, window->bounds)) {
                        onMBpressedEvent(window->rootelement, context, mousepos, e);
                    }
                }
            }
        }

        if(e.type == IME_MOUSE_MOVED) {
            IME::vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos);

            for(const Window& window : context->uiwindows) {
                if (isInBounds(mousepos, window.bounds)) {
                    onMouseMovedEvent(window.rootelement, context, mousepos, e);
                }
            }
        }

        if(e.type == IME_MOUSE_BUTTON_RELEASED) {
            IME::vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos);
            if(e.param1 == IME_LEFT_MB) {
                context->isresizing = false;
                context->isgrabbing = false;
                context->isSliding = false;
            }
        }
    }

    void updateUi(Context* context, const PlatformInterface& platform) {
        
        Window* window = &context->uiwindows[context->selectedWindowptr];
        vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos);
        if(context->isresizing) {
            window->bounds.bottomright = mousepos;
            calculateUiComponentsForWindow(context, *window);
        }
        if(context->isgrabbing) {
            IME::vec2f windowsize = sizeOfBounds(window->bounds);
            window->bounds.topleft = mousepos - context->relativegrab;
            window->bounds.bottomright = mousepos - context->relativegrab + vec2f{windowsize.x, -windowsize.y};
            calculateUiComponentsForWindow(context, *window);
        }
        if(context->isSliding) {
            real32 dist = mousepos.x - context->mouseslideorigin.x;
            FloatSlider* fs = &context->floatsliders[context->selectedsliderptr.dataptr].data;
            fs->value[context->nvalue] = context->originalvalue + (int32)(dist * 0.05) * context->multiplier;
        }
    }

    void calculateUiComponents(Context* context) {
        for(uint32 i = 0; i < context->uiwindows.getCount(); i++) {
            calculateUiComponentsForWindow(context, context->uiwindows[i]);
        }
    }

    real32 topbarheight = 15.0f;

    void calculateUiComponentsForWindow(Context* context, const Window& window) {
        
        real32 depth = 3.0f;
        Div* main = &context->divs[window.rootelement.dataptr].data;

        Bounds mainbounds = subtractBorderFromBounds(window.bounds, main->props.margin);
        mainbounds.top -= topbarheight;

        main->props.contentbounds = subtractBorderFromBounds(mainbounds, main->props.padding);
        main->props.elementtransform = calcTransformFromBounds(mainbounds, depth);

        Bounds childspace = main->props.contentbounds;
        real32 y = 0.0f;
        for(sizeptr i = 0; i < main->children.getCount(); i++) {
            Bounds child = calculateUiComponent(context, main->children[i], window.rootelement, childspace, depth + 1);
            childspace.top = child.bottom;
        }
    }

    Bounds calculateUiComponent(Context* context, ElementPtr element, ElementPtr parent, Bounds maxbounds, real32 depth) {
        
        Bounds result;
        if(element.type == UI_PARAGRAPH) {
            Paragraph* p = &context->paragraphs[element.dataptr].data;
            p->parent = parent;

            Bounds maxtextspace = subtractBorderFromBounds(subtractBorderFromBounds(maxbounds, p->props.padding), p->props.margin);
            vec2f textsize = calculateStringSize(p->text, maxtextspace.right - maxtextspace.left, p->glyphsize, p->linespacing);

            p->props.contentbounds = {maxtextspace.topleft, {maxtextspace.topleft.x + textsize.x, maxtextspace.topleft.y - textsize.y}};
            p->props.depth = depth;

            Bounds fullbounds = addBorderToBounds(p->props.contentbounds, p->props.padding);

            if(p->props.width != 0.0f) {
                real32 width = maxbounds.right - maxbounds.left;
                fullbounds.right = fullbounds.left + width * (p->props.width / 100.0f) - p->props.margin.right;
                p->props.contentbounds = subtractBorderFromBounds(fullbounds, p->props.padding);
            }

            p->props.elementtransform = calcTransformFromBounds(fullbounds, depth);

            return fullbounds;
        }

        if(element.type == UI_DIV) {
            Div* div = &context->divs[element.dataptr].data;

            Bounds childspace = subtractBorderFromBounds(subtractBorderFromBounds(maxbounds, div->props.padding), div->props.margin);
            Bounds usedspace = {maxbounds.topleft, maxbounds.topleft};

            div->props.depth = depth;

            for(sizeptr i = 0; i < div->children.getCount(); i++) {
                Bounds child = calculateUiComponent(context, div->children[i], element, childspace, depth + 1);
                childspace.topleft = { child.bottom, child.left };
                usedspace.bottomright = { maxReal32(childspace.right, usedspace.right), childspace.top };
            }

            Bounds fullbounds = addBorderToBounds(usedspace, div->props.padding);

            if(div->props.width != 0.0f) {
                real32 width = maxbounds.right - maxbounds.left;
                fullbounds.right = fullbounds.left + width * (div->props.width / 100.0f) - div->props.margin.right;
            }

            div->props.elementtransform = calcTransformFromBounds(fullbounds, depth);
            return fullbounds;
        }

        if(element.type == UI_FLOAT_SLIDER) {
            
            FloatSlider* fs = &context->floatsliders[element.dataptr].data;

            fs->props.contentbounds = subtractBorderFromBounds(subtractBorderFromBounds(maxbounds, fs->props.padding), fs->props.margin);
            fs->props.contentbounds.bottomright.y = fs->props.contentbounds.topleft.y - fs->glyphsize.y;

            fs->props.depth = depth;

            Bounds fullbounds = addBorderToBounds(fs->props.contentbounds, fs->props.padding);

            if(fs->props.width != 0.0f) {
                real32 width = maxbounds.right - maxbounds.left;
                fullbounds.right = fullbounds.left + width * (fs->props.width / 100.0f) - fs->props.margin.right;
            }

            fs->props.elementtransform = calcTransformFromBounds(fullbounds, depth);

            return fullbounds;
        }
    }

    void pushElementsToRQ(const Context& context, RenderQueue2D* renderqueue, gl_id shader) {

        for(Window& window : context.uiwindows) {

            RendererCommand2D command;

            Bounds topbar = window.bounds;
            topbar.bottom = topbar.top - topbarheight;

            mat4 transform = calcTransformFromBounds(topbar, 2.0f);
            command.shader = shader;
            command.texture = 0;
            command.transform = transform;
            command.color = {0.0f, 0.0f, 1.0f, 1.0f};
            renderqueue->commands.push_back(command);   

            transform = calcTransformFromBounds(window.bounds, 1.0f);
            command.shader = shader;
            command.texture = 0;
            command.transform = transform;
            command.color = {1.0f, 1.0f, 1.0f, 1.0f};
            renderqueue->commands.push_back(command);   
        }

        for(sizeptr i = 0; i < context.divs.getCount(); i++) {
        const UiElementList<Div>::DataChunk* chunk = &context.divs[i];

            if(chunk->isoccupied) {
                RendererCommand2D command;
                command.shader = chunk->data.props.shader;
                command.texture = 0;
                command.transform = chunk->data.props.elementtransform;
                command.color = chunk->data.props.background;
                renderqueue->commands.push_back(command);            
            }
        }

        for(sizeptr i = 0; i < context.paragraphs.getCount(); i++) {
            const UiElementList<Paragraph>::DataChunk* chunk = &context.paragraphs[i];

            if(chunk->isoccupied) {
                //drawing the background
                RendererCommand2D command;
                command.shader = chunk->data.props.shader;
                command.texture = 0;
                command.transform = chunk->data.props.elementtransform;
                command.color = chunk->data.props.background;
                renderqueue->commands.push_back(command);

                //drawing the content
                drawStringFromTextureAtlas(
                    chunk->data.text, 
                    chunk->data.props.contentbounds.topleft, 
                    chunk->data.glyphsize, 
                    *chunk->data.atlas, 
                    chunk->data.props.contentbounds.right - chunk->data.props.contentbounds.left, 
                    chunk->data.textcolor,
                    renderqueue, 
                    chunk->data.props.depth + 1, 
                    chunk->data.linespacing);    
            }
        }

        for(sizeptr i = 0; i < context.floatsliders.getCount(); i++) {
            const UiElementList<FloatSlider>::DataChunk* chunk = &context.floatsliders[i];
            const FloatSlider* fs = &chunk->data;

            if(chunk->isoccupied) {
                //drawing the background
                RendererCommand2D command;
                command.shader = chunk->data.props.shader;
                command.texture = 0;
                command.transform = chunk->data.props.elementtransform;
                command.color = chunk->data.props.background;
                renderqueue->commands.push_back(command);

                //calculating space for value

                Bounds valuebounds;
                valuebounds.top = fs->props.contentbounds.top;
                valuebounds.bottom = fs->props.contentbounds.bottom;
                for(uint32 i = 0; i < fs->nvalues; i++) {
                    valuebounds.right = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + 10.0f);
                    valuebounds.left = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + 10.0f) - slidervaluesize * fs->glyphsize.x;

                    mat4 valueboundstransform = calcTransformFromBounds(valuebounds, fs->props.depth + 1);
                    command.shader = chunk->data.props.shader;
                    command.texture = 0;
                    command.transform = valueboundstransform;
                    command.color = {0.0f, 0.0f, 0.0f, 1.0f};
                    renderqueue->commands.push_back(command);

                    char buffer[16];
                    sprintf_s(buffer, 256, "%.*g", (int32)slidervaluesize - 2,  fs->value[fs->nvalues - 1 - i]);

                    drawStringFromTextureAtlas(buffer, 
                        valuebounds.topleft, 
                        chunk->data.glyphsize, 
                        *chunk->data.atlas, 
                        valuebounds.right - valuebounds.left, 
                        {1.0f, 1.0f, 1.0f, 1.0f},
                        renderqueue, 
                        chunk->data.props.depth + 2.0f, 
                        0.0f);
                }

                //drawing the content
                drawStringFromTextureAtlas(
                    chunk->data.tag, 
                    chunk->data.props.contentbounds.topleft, 
                    chunk->data.glyphsize, 
                    *chunk->data.atlas, 
                    chunk->data.props.contentbounds.right - chunk->data.props.contentbounds.left, 
                    chunk->data.textcolor,
                    renderqueue, 
                    chunk->data.props.depth + 1, 
                    0.0f);
            }
        }
    }

    IME::bool32 onMouseMovedEvent(ElementPtr element, Context* context, vec2f mousepos, Event e) {

        if(element.type == UI::UI_DIV) {
            Div* el = &context->divs[element.dataptr].data;

            if(isInBounds(mousepos, addBorderToBounds(el->props.contentbounds, el->props.padding))) {
                if(el->props.onHover && el->props.hovered == false) {
                    el->props.onHover(el->props.id, element, context, context->userptr, e);
                    el->props.hovered = true;

                }
                for(ElementPtr el : el->children) {
                    onMouseMovedEvent(el, context, mousepos, e);
                }
                return true;
            } else {
                if(el->props.ofHover && (bool)el->props.hovered == true) {
                    el->props.ofHover(el->props.id, element, context, context->userptr, e);
                    el->props.hovered = false;
                }
                return true;
            }
        }

        if(element.type == UI::UI_PARAGRAPH) {
            Paragraph* el = &context->paragraphs[element.dataptr].data;

            if(isInBounds(mousepos, addBorderToBounds(el->props.contentbounds, el->props.padding))) {
                    if(el->props.onHover && (bool)el->props.hovered == false) {
                    el->props.onHover(el->props.id, element, context, context->userptr, e);
                    el->props.hovered = true;

                }
                return true;
            } else {
                if(el->props.ofHover && (bool)el->props.hovered == true) {
                    el->props.ofHover(el->props.id, element, context, context->userptr, e);
                    el->props.hovered = false;
                }
                return true;
            }
        }

        if(element.type == UI::UI_FLOAT_SLIDER) {
            FloatSlider* el = &context->floatsliders[element.dataptr].data;

            if(isInBounds(mousepos, addBorderToBounds(el->props.contentbounds, el->props.padding))) {
                if(el->props.onHover && el->props.hovered == false) {
                    el->props.onHover(el->props.id, element, context, context->userptr, e);
                    el->props.hovered = true;

                }
                return true;
            } else {
                if(el->props.ofHover && el->props.hovered == true) {
                    el->props.ofHover(el->props.id, element, context, context->userptr, e);
                    el->props.hovered = false;
                }
                return true;
            }
        }

    }

    IME::bool32 onMBpressedEvent(ElementPtr element, Context* context, vec2f mousepos, Event e) {

        if(element.type == UI::UI_DIV) {
            Div* div = &context->divs[element.dataptr].data;

            if(isInBounds(mousepos, addBorderToBounds(div->props.contentbounds, div->props.padding))) {

                if(div->props.onclick) {
                    div->props.onclick(div->props.id, element, context, context->userptr, e);
                }

                for(ElementPtr el : div->children) {
                    onMBpressedEvent(el, context, mousepos, e);
                }

                return true;
            }
            return false;
        }

        if(element.type == UI::UI_FLOAT_SLIDER) {
            FloatSlider* fs = &context->floatsliders[element.dataptr].data;
            if(isInBounds(mousepos, addBorderToBounds(fs->props.contentbounds, fs->props.padding))) {
            
                Bounds valuebounds;
                valuebounds.top = fs->props.contentbounds.top;
                valuebounds.bottom = fs->props.contentbounds.bottom;

                for(uint32 i = 0; i < fs->nvalues; i++) {
                    valuebounds.right = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + 10.0f);
                    valuebounds.left = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + 10.0f) - slidervaluesize * fs->glyphsize.x;

                    if(isInBounds(mousepos, valuebounds)) {
                        if(e.param1 == IME_LEFT_MB) {
                            context->isSliding = true;
                            context->selectedsliderptr = element;
                            context->mouseslideorigin = mousepos;
                            context->originalvalue = fs->value[fs->nvalues - 1 - i];
                            context->nvalue = fs->nvalues - 1 - i;

                            int32 characterplace = (uint32)(slidervaluesize * (mousepos.x - valuebounds.left) / (slidervaluesize * fs->glyphsize.x));
                            int32 valuesize = log10f(fs->value[fs->nvalues - 1 - i]);
                            real32 exp = (real32)(valuesize - characterplace);
                            context->multiplier = powerReal32(10.0f, exp);
                        }
                    }
                }
                if(fs->props.onclick) {
                    fs->props.onclick(fs->props.id, element, context, context->userptr, e);
                }
            }
        }
        if(element.type == UI::UI_PARAGRAPH) {
            Paragraph* p = &context->paragraphs[element.dataptr].data;
            if(isInBounds(mousepos, addBorderToBounds(p->props.contentbounds, p->props.padding))) {
                if(p->props.onclick) {
                    p->props.onclick(p->props.id, element, context, context->userptr, e);
                }
            }
        }
        return false;
    }
}