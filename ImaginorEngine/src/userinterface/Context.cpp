#include "context.h"
#include "Window.h"
#include <stdio.h>
#include "src/textrenderering.h"
#include "../functionality.h"

namespace IME::UI {

    real32 slidervaluesize = 5.0f;

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
        result.uiwindows = UiElementList<Window>::createFragmentedArrayList(1);
        result.images = UiElementList<Image>::createFragmentedArrayList(1);
        result.isresizing = false;
        return result;
    }

    inline 
    mat4 calcTransformFromBounds(const Bounds& bounds, real32 depth) {

        IME::vec2f center = centerOfBounds(bounds);
        IME::vec2f size = sizeOfBounds(bounds);
        return transformMat4(vec3f{center.x, center.y, depth}, vec3f{size.x, size.y, 1.0f});

    }

    StaticElementProperties setStaticProperties( const StyleProperties& style, ElementPtr parent, const char* id) {
        StaticElementProperties props;

        props.height = style.height;
        props.width = style.width;
        props.absoluteheight = style.absoluteheight;
        props.absolutewidth = style.absolutewidth;

        props.padding = style.padding;
        props.border = style.border;
        props.margin = style.margin;

        props.background = style.background;
        props.bordercolor = style.bordercolor;

        props.shader = style.shader;

        props.id.set(id);

        return props;
    }

    ElementPtr addImage(Context* context, ElementPtr parent, const StyleProperties& style, Texture* texture, const char* id) {

        Image i;
        i.props = setStaticProperties(style, parent, id);
        
        i.imagesize = { (real32)texture->props.width, (real32)texture->props.height };
        i.image = texture->texture;
        i.texture = texture;

        ElementPtr result;
        result.dataptr = context->images.add(i);
        result.type = UI_IMAGE;

        if(parent.type == UI_DIV) {
            context->divs[parent.dataptr].data.children.push_back(result);
        }
        return result;
    }

    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style, const char* id) {

        Paragraph p;
        p.props.parent = parent;
        p.textlength = strlen(text);
        p.text.set(text);
        
        //all style related things
        p.glyphsize = style.glyphsize;
        p.atlas = style.font;
        p.linespacing = style.linespacing;
        p.textcolor = style.textcolor;

        p.props = setStaticProperties(style, parent, id);

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
        div.children = Data::ArrayList_<ElementPtr, Memory::alloc, Memory::dealloc>::create(0);

        div.props = setStaticProperties(style, parent, id);

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

        fs.textcolor = style.textcolor;
        fs.tag.set(tag);

        fs.props = setStaticProperties(style, parent, id);

        ElementPtr result;
        result.dataptr = context->floatsliders.add(fs);
        result.type = UI_FLOAT_SLIDER;
        if(parent.type == UI_DIV) {
            context->divs[parent.dataptr].data.children.push_back(result);
        }
        return result;
    }

    void removeElementRecursive(ElementPtr element, Context* context) {
        if(element.type == UI_PARAGRAPH) {
            Paragraph* el = &context->paragraphs[element.dataptr].data;

            if(el->text) {
                el->text.clear();
            }
            if(el->props.id) {
                el->props.id.clear();
            }

            context->paragraphs.remove(element.dataptr);
            return;
        }

        if(element.type == UI_FLOAT_SLIDER) {

            FloatSlider* el = &context->floatsliders[element.dataptr].data;

            if(el->props.id) {
                el->props.id.clear();
            }
            if(el->tag) {
                el->tag.clear();
            }

            context->floatsliders.remove(element.dataptr);
            return;
        }

        if(element.type == UI_DIV) {
            Div* el = &context->divs[element.dataptr].data;

            if(el->props.id) {
                el->props.id.clear();
            }
            for(ElementPtr child : el->children) {
                removeElementRecursive(child, context);
            }
            
            if(el->children.getCapacity() > 0) {
                Data::ArrayList_<ElementPtr, Memory::alloc, Memory::dealloc>::destroy(el->children);
            }
            context->divs.remove(element.dataptr);
            return;
        }
        
        if(element.type == UI_IMAGE) {
            Image* el = &context->images[element.dataptr].data;

            if(el->props.id) {
                el->props.id.clear();
            }

            context->images.remove(element.dataptr);
        }
    }

    ElementPtr addFloatSlider(Context* context, ElementPtr parent, const StyleProperties& style, uint32 nfloats, real32* values, const char* tag) {
        return addFloatSlider(context, parent, style, nfloats, values, tag, "");
    }

    StaticElementProperties* getStaticElementProperties(ElementPtr elptr, Context* context) {
        if(elptr.type == UI_PARAGRAPH) {
            return &context->paragraphs[elptr.dataptr].data.props;
        }
        if(elptr.type == UI_FLOAT_SLIDER) {
            return &context->floatsliders[elptr.dataptr].data.props;
        }
        if(elptr.type == UI_DIV) {
            return &context->divs[elptr.dataptr].data.props;
        }
        if(elptr.type == UI_IMAGE) {
            return &context->images[elptr.dataptr].data.props;
        }
        return nullptr;
    }

    bool32 addOnUpdateToFloatSlider(ElementPtr elptr, Context* context, onUpdateFloatSlider* callback) {
        IME_DEBUG_ASSERT_BREAK(elptr.type == UI_FLOAT_SLIDER)
        FloatSlider& fs = context->floatsliders[elptr.dataptr].data;
        fs.onupdate = callback;
        return true;
    }

    bool32 addOnClickToElement(ElementPtr elptr, Context* context, onClickCallback* callback) {

        StaticElementProperties* props = getStaticElementProperties(elptr, context);
        props->onclick = callback;
        return true;
    }

    bool32 addOnHoverToElement(ElementPtr elptr, Context* context, onHoverCallback* callback) {
        
        StaticElementProperties* props = getStaticElementProperties(elptr, context);
        props->onHover = callback;
        return true;
    }

    bool32 addOfHoverToElement(ElementPtr elptr, Context* context, ofHoverCallback* callback) {
        StaticElementProperties* props = getStaticElementProperties(elptr, context);
        props->ofHover = callback;
        return true;
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

                    if(!context->uiwindows[i].isoccupied) {
                        continue;
                    }

                    Window* window = &context->uiwindows[i].data;

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

            for(uint32 i = 0; i < context->uiwindows.getCount(); i++) {
                if(!context->uiwindows[i].isoccupied) {
                    continue;
                }
                if (isInBounds(mousepos, context->uiwindows[i].data.bounds)) {
                    onMouseMovedEvent(context->uiwindows[i].data.rootelement, context, mousepos, e);
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
        
        Window* window = &context->uiwindows[context->selectedWindowptr].data;
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

            Event e = {0};
            real32 dist = mousepos.x - context->mouseslideorigin.x;
            FloatSlider* fs = &context->floatsliders[context->selectedsliderptr.dataptr].data;
            fs->value[context->nvalue] = context->originalvalue + (int32)(dist * 0.05) * context->multiplier;
            if(fs->onupdate) {
                fs->onupdate(fs->props.id, context->selectedsliderptr, fs->value, fs->nvalues, context, context->userptr, e);
            }
        }
    }

    void calculateUiComponents(Context* context) {
        for(uint32 i = 0; i < context->uiwindows.getCount(); i++) {
            if(!context->uiwindows.getUnchecked(i).isoccupied) {
                continue;
            }
            calculateUiComponentsForWindow(context, context->uiwindows[i].data);
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
            p->props.parent = parent;

            Bounds maxtextspace = subtractBorderFromBounds(subtractBorderFromBounds(maxbounds, p->props.padding), p->props.margin);
            vec2f textsize = calculateStringSize(p->text.getCstring(), maxtextspace.right - maxtextspace.left, p->glyphsize, p->linespacing);

            p->props.contentbounds = {maxtextspace.topleft, {maxtextspace.topleft.x + textsize.x, maxtextspace.topleft.y - textsize.y}};
            p->props.depth = depth;

            Bounds fullbounds = addBorderToBounds(p->props.contentbounds, p->props.padding);

            if(p->props.width != 0.0f) {
                real32 width = maxbounds.right - maxbounds.left;
                fullbounds.right = fullbounds.left + width * (p->props.width / 100.0f) - p->props.margin.right;
                p->props.contentbounds = subtractBorderFromBounds(fullbounds, p->props.padding);
            }

            p->props.elementtransform = calcTransformFromBounds(fullbounds, depth);

            return addBorderToBounds(fullbounds, p->props.margin);
        }

        if(element.type == UI_DIV) {

            Div* div = &context->divs[element.dataptr].data;

            Bounds childspace = subtractBorderFromBounds(subtractBorderFromBounds(maxbounds, div->props.padding), div->props.margin);
            Bounds usedspace = subtractBorderFromBounds(subtractBorderFromBounds( Bounds{maxbounds.topleft, maxbounds.topleft}, div->props.padding), div->props.margin);

            div->props.depth = depth;

            for(ElementPtr el : div->children) {
                Bounds child = calculateUiComponent(context, el, element, childspace, depth + 1);
                childspace.topleft = { child.left, child.bottom };
                usedspace.bottomright = { maxReal32(childspace.right, usedspace.right), childspace.top };
            }

            div->props.contentbounds = usedspace;

            Bounds fullbounds = addBorderToBounds(usedspace, div->props.padding);

            if(div->props.width != 0.0f) {
                real32 width = maxbounds.right - maxbounds.left;
                fullbounds.right = fullbounds.left + width * (div->props.width / 100.0f) - div->props.margin.right;
            }

            div->props.elementtransform = calcTransformFromBounds(fullbounds, depth);
            return addBorderToBounds(fullbounds, div->props.margin);
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

            return addBorderToBounds(fullbounds, fs->props.margin);
        }

        if(element.type == UI_IMAGE) {
            
            Image* i = &context->images[element.dataptr].data;

            Bounds maxcontentbounds = subtractBorderFromBounds(subtractBorderFromBounds(maxbounds, i->props.margin), i->props.padding);

            i->props.contentbounds = { maxcontentbounds.topleft, {maxcontentbounds.topleft.x + i->imagesize.x, maxcontentbounds.topleft.y - i->imagesize.y} };
            i->props.depth = depth;

            real32 aspectratio = i->imagesize.x / i->imagesize.y;

            if(i->props.absolutewidth != 0.0f) {
                i->props.contentbounds.right = i->props.contentbounds.left + i->props.absolutewidth;
                if(i->props.absoluteheight == 0.0f && i->props.height == 0.0f) {
                    i->props.contentbounds.bottom = i->props.contentbounds.top - (i->props.absolutewidth / aspectratio);
                }
            }

            if(i->props.absoluteheight != 0.0f) {
                i->props.contentbounds.bottom = i->props.contentbounds.top - i->props.absoluteheight;
                if(i->props.absolutewidth == 0.0f && i->props.width == 0.0f) {
                    i->props.contentbounds.right = i->props.contentbounds.left + (i->props.absoluteheight * aspectratio);
                }
            }

            Bounds fullbounds = addBorderToBounds(i->props.contentbounds, i->props.padding);
            i->props.elementtransform = calcTransformFromBounds(fullbounds, i->props.depth);

            return addBorderToBounds(fullbounds, i->props.margin);
        }
    }

    real32 inputpadding = 5.0f;

    void pushElementsToRQ(const Context& context, RenderQueue2D* renderqueue, gl_id shader) {

        for(uint32 i = 0; i < context.uiwindows.getCount(); i++) {
            if(context.uiwindows.getUnchecked(i).isoccupied == false) {
                continue;
            }

            const Window& window = context.uiwindows[i].data;

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
        const UiElementList<Div>::DataChunk* chunk = &context.divs.getUnchecked(i);

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
            const UiElementList<Paragraph>::DataChunk* chunk = &context.paragraphs.getUnchecked(i);

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
                    chunk->data.text.getCstring(), 
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
            const UiElementList<FloatSlider>::DataChunk* chunk = &context.floatsliders.getUnchecked(i);
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
                    valuebounds.right = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + inputpadding);
                    valuebounds.left = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + inputpadding) - slidervaluesize * fs->glyphsize.x;

                    mat4 valueboundstransform = calcTransformFromBounds(valuebounds, fs->props.depth + 1);
                    command.shader = chunk->data.props.shader;
                    command.texture = 0;
                    command.transform = valueboundstransform;
                    command.color = {0.0f, 0.0f, 0.0f, 1.0f};
                    renderqueue->commands.push_back(command);

                    char buffer[16];
                    sprintf_s(buffer, 16, "%.*g", (int32)slidervaluesize,  fs->value[fs->nvalues - 1 - i]);

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
                    chunk->data.tag.getCstring(), 
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
        for(sizeptr i = 0; i < context.images.getCount(); i++) {

            const UiElementList<Image>::DataChunk* chunk = &context.images.getUnchecked(i);
            const Image* fs = &chunk->data;

            if(chunk->isoccupied) {

                RendererCommand2D command;
                command.shader = chunk->data.props.shader;
                command.texture = 0;
                command.transform = chunk->data.props.elementtransform;
                command.color = chunk->data.props.background;
                renderqueue->commands.push_back(command);

                command.shader = chunk->data.props.shader;
                command.texture = fs->image;
                command.transform = calcTransformFromBounds(fs->props.contentbounds, fs->props.depth + 1);
                command.color = {1.0f, 1.0f, 1.0f, 1.0f};
                command.texcoords[0] = {0.0f, 0.0f};
                command.texcoords[1] = {1.0f, 0.0f};
                command.texcoords[2] = {1.0f, 1.0f};
                command.texcoords[3] = {0.0f, 1.0f};


                renderqueue->commands.push_back(command);         
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
                    valuebounds.right = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + inputpadding);
                    valuebounds.left = fs->props.contentbounds.right - i * (slidervaluesize * fs->glyphsize.x + inputpadding) - slidervaluesize * fs->glyphsize.x;

                    if(isInBounds(mousepos, valuebounds)) {
                        if(e.param1 == IME_LEFT_MB) {
                            context->isSliding = true;
                            context->selectedsliderptr = element;
                            context->mouseslideorigin = mousepos;
                            context->originalvalue = fs->value[fs->nvalues - 1 - i];
                            context->nvalue = fs->nvalues - 1 - i;

                            int32 characterplace = (uint32)(slidervaluesize * (mousepos.x - valuebounds.left) / (slidervaluesize * fs->glyphsize.x));
                            if(context->originalvalue < 0.0f) {
                                characterplace -= 1;
                            }



                            real32 value = fs->value[fs->nvalues - 1 - i];
                            int32 valuesize = 0.0f;
                            if(value != 0.0f) {
                                valuesize = log10f(absoluteReal32(value));
                            }


                            int32 exp = valuesize - characterplace;
                            if(exp < 0) {
                                exp++;
                            }
                            context->multiplier = powerReal32(10.0f, (real32)exp);
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