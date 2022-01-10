#include "context.h"
#include <stdio.h>
#include "src/textrenderering.h"
#include "../functionality.h"
#include "../rendering/rendering.h"

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

    ElementPtr addWindow(Context* context, ElementPtr parent, const Bounds& bounds, const PlatformInterface& platform) {

        Window window;
        window.bounds = bounds;
        window.context = context;

        window.rendertarget = platform.gfx.fbo_create(absoluteInt32(bounds.right - bounds.left), absoluteInt32(bounds.bottom - bounds.top));
        platform.gfx.fbo_bind(window.rendertarget);

        TextureProperties props;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;

        props.generatemipmaps = false;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.height = absoluteInt32(bounds.bottom - bounds.top);
        props.width = absoluteInt32(bounds.right - bounds.left);
        props.format = IME_RGBA;

        window.rendertexture = platform.gfx.fbo_createtextureattachment(IME_COLOR_ATTACHMENT0, props);
        window.depthbuffer = platform.gfx.fbo_createrenderbufferattachment(IME_DEPTH24_STENCIL8);

        ElementPtr result;
        result.dataptr = context->uiwindows.add(window);
        result.type = UI_WINDOW;
        return result;
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
            UI::Div& div = context->divs[parent.dataptr].data;
            div.children.push_back(result);
        }
        return result;
    }

    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style) {
        return addParagraph(context, parent, text, style, "");
    }

    ElementPtr addDiv(Context* context, ElementPtr parent, const StyleProperties& style, const char* id) {
        Div div;
        //all style related things
        div.children = UI::ArrayList<ElementPtr>::create(0);

        div.props = setStaticProperties(style, parent, id);

        ElementPtr result;
        result.dataptr = context->divs.add(div);
        result.type = UI_DIV;
        if(parent.type == UI_DIV) {
            context->divs[parent.dataptr].data.children.push_back(result);
        }

        if(parent.type == UI_WINDOW) {
            context->uiwindows[parent.dataptr].data.body = result;
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

            for (ElementPtr child : el->children) {
                removeElementRecursive(child, context);
            }
            
            if(el->children.getCapacity() > 0) {
                UI::ArrayList<ElementPtr>::destroy(el->children);
            }

            if (el->props.id) {
                el->props.id.clear();
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
        IME_DEBUG_ASSERT_BREAK(elptr.type == UI_FLOAT_SLIDER, "")
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
                        onMBpressedEvent(window->body, context, mousepos, e);
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
                    onMouseMovedEvent(context->uiwindows[i].data.body, context, mousepos, e);
                }
                if(context->isresizing) {

                    Window* window = &context->uiwindows[context->selectedWindowptr].data;
                    window->bounds.bottomright = mousepos;

                    platform.gfx.fbo_bind(window->rendertarget);
                    platform.gfx.fbo_resize(absoluteInt32(window->bounds.right - window->bounds.left), absoluteInt32(window->bounds.bottom - window->bounds.top));

                    calculateUiComponentsForWindow(context, *window);
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
        Div* main = &context->divs[window.body.dataptr].data;

        Bounds mainbounds = subtractBorderFromBounds(window.bounds, main->props.margin);
        mainbounds.top -= topbarheight;

        main->props.contentbounds = subtractBorderFromBounds(mainbounds, main->props.padding);

        main->props.backgroundpos = getPositionFromBounds(mainbounds);
        main->props.backgroundsize = getSizeFromBounds(mainbounds);

        main->props.depth = depth;

        Bounds childspace = main->props.contentbounds;
        real32 y = 0.0f;
        for(sizeptr i = 0; i < main->children.getCount(); i++) {
            Bounds child = calculateUiComponent(context, main->children[i], window.body, childspace, depth + 1.0f);
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

            p->props.backgroundpos = getPositionFromBounds(fullbounds);
            p->props.backgroundsize = getSizeFromBounds(fullbounds);

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

            //setting the render bounds
            div->props.backgroundpos = getPositionFromBounds(fullbounds);
            div->props.backgroundsize = getSizeFromBounds(fullbounds);

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

            fs->props.backgroundpos = getPositionFromBounds(fullbounds);
            fs->props.backgroundsize = getSizeFromBounds(fullbounds);

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
            i->props.backgroundpos = getPositionFromBounds(fullbounds);
            i->props.backgroundsize = getSizeFromBounds(fullbounds);

            return addBorderToBounds(fullbounds, i->props.margin);
        }
    }

    real32 inputpadding = 5.0f;


    void pushElementsToRenderSetHelper(ElementPtr element, const Context& context, ArrayList<SimpleQuadCommand>* quadrq, ArrayList<SimpleTextCommand>* textrq) {
        
        //draw a div element
        if(element.type == UI_DIV) {
            SimpleQuadCommand command;
            Div el = context.divs[element.dataptr].data;

            if(!el.props.background.z == 0.0f) {    
                command.shader = el.props.shader;
                command.texture = 0;
                command.position = toVec3(el.props.backgroundpos, el.props.depth);
                command.size = el.props.backgroundsize;

                command.color = el.props.background;
                quadrq->push_back(command); 
            }


            for(ElementPtr child : el.children) {
                pushElementsToRenderSetHelper(child, context, quadrq, textrq);
            }
        }

        //draw a paragraph
        if(element.type == UI_PARAGRAPH) {
            
            Paragraph el = context.paragraphs[element.dataptr].data;
            //drawing the background
            {
                SimpleQuadCommand command;
                command.shader = el.props.shader;
                command.texture = 0;
                command.position = toVec3(el.props.backgroundpos, el.props.depth);
                command.size = el.props.backgroundsize;

                command.color = el.props.background;
                quadrq->push_back(command);       
            }
            //drawing the content
            {
                SimpleTextCommand command;
                command.text = el.text.copy();
                command.position = toVec3(el.props.contentbounds.topleft, el.props.depth + 1);
                command.glyphsize = el.glyphsize;
                command.font = el.atlas;
                command.maxwidth = el.props.contentbounds.right - el.props.contentbounds.left;
                command.color = el.textcolor;
                command.linespacing = el.linespacing;

                textrq->push_back(command); 
            }
        }

        //draw a float slider
        if(element.type == UI_FLOAT_SLIDER) {
            //drawing the background

            FloatSlider el = context.floatsliders[element.dataptr].data;
            {
                SimpleQuadCommand command;
                command.shader = el.props.shader;
                command.texture = 0;
                command.position = toVec3(el.props.backgroundpos, el.props.depth);
                command.size = el.props.backgroundsize;

                command.color = el.props.background;
                quadrq->push_back(command);       
            }

            //calculating space for value

            Bounds valuebounds;
            valuebounds.top = el.props.contentbounds.top;
            valuebounds.bottom = el.props.contentbounds.bottom;

            for(uint32 i = 0; i < el.nvalues; i++) {
                valuebounds.right = el.props.contentbounds.right - i * (slidervaluesize * el.glyphsize.x + inputpadding);
                valuebounds.left = el.props.contentbounds.right - i * (slidervaluesize * el.glyphsize.x + inputpadding) - slidervaluesize * el.glyphsize.x;

                {
                    SimpleQuadCommand command;
                    command.shader = el.props.shader;
                    command.texture = 0;
                    command.position = toVec3(getPositionFromBounds(valuebounds), el.props.depth + 1);
                    command.size = getSizeFromBounds(valuebounds);
                    command.color = {0.0f, 0.0f, 0.0f, 1.0f};
                    quadrq->push_back(command);
                }

                char buffer[16];
                sprintf_s(buffer, 16, "%.*g", (int32)slidervaluesize,  el.value[el.nvalues - 1 - i]);
                {
                    SimpleTextCommand command;
                    command.text.set(buffer);
                    command.position = toVec3(valuebounds.topleft, el.props.depth + 2);
                    command.glyphsize = el.glyphsize;
                    command.font = el.atlas;
                    command.maxwidth = valuebounds.right - valuebounds.left;
                    command.color = {1.0f, 1.0f, 1.0f, 1.0f};
                    command.linespacing = 0.0f;

                    textrq->push_back(command); 
                }
            }

            {
                SimpleTextCommand command;
                command.text = el.tag.copy();
                command.position = toVec3(el.props.contentbounds.topleft, el.props.depth + 1);
                command.glyphsize = el.glyphsize;
                command.font = el.atlas;
                command.maxwidth = el.props.contentbounds.right - el.props.contentbounds.left;
                command.color = el.textcolor;
                command.linespacing = 0.0f;

                textrq->push_back(command); 
            }        
        }

        if(element.type == UI_IMAGE) {
            
            Image el = context.images[element.dataptr].data;
            //drawing the background
            {
                SimpleQuadCommand command;
                command.shader = el.props.shader;
                command.texture = 0;
                command.position = toVec3(el.props.backgroundpos, el.props.depth);
                command.size = el.props.backgroundsize;
                command.color = el.props.background;

                command.color = el.props.background;
                quadrq->push_back(command);       
            }

            {
                SimpleQuadCommand command;
                command.shader = el.props.shader;
                command.texture = el.image;
                command.position = toVec3(getPositionFromBounds(el.props.contentbounds), el.props.depth + 1);
                command.size = getSizeFromBounds(el.props.contentbounds);
                command.color = {1.0f, 1.0f, 1.0f, 1.0f};
    
                quadrq->push_back(command);         
            }
        }

    }

    void pushElementsToRenderSet(const Context& context, RenderSet* renderset, gl_id compositshader, PlatformInterface platform) {

        for(uint32 i = 0; i < context.uiwindows.getCount(); i++) {
            if(context.uiwindows.getUnchecked(i).isoccupied == false) {
                continue;
            }

            const Window& window = context.uiwindows[i].data;

            ArrayList<SimpleQuadCommand> quadrq = ArrayList<SimpleQuadCommand>::create(0);
            ArrayList<SimpleTextCommand> textrq = ArrayList<SimpleTextCommand>::create(0);

            SimpleQuadCommand command;

            Bounds topbar = window.bounds;
            topbar.bottom = topbar.top - topbarheight;

            command.shader = compositshader;
            command.texture = 0;
            command.position = toVec3(getPositionFromBounds(window.bounds), 0.0f);
            command.size = getSizeFromBounds(window.bounds);
            command.color = {1.0f, 0.0f, 1.0f, 1.0f};
            quadrq.push_back(command);

            command.shader = compositshader;
            command.texture = 0;
            command.position = toVec3(getPositionFromBounds(topbar), 0.1f);
            command.size = getSizeFromBounds(topbar);
            command.color = {0.0f, 0.0f, 1.0f, 1.0f};
            quadrq.push_back(command);

            if(window.body.type != UI_NONE) {
                pushElementsToRenderSetHelper(window.body, context, &quadrq, &textrq);
            }

            RenderQueue quads;
            quads.commandtype = SIMPLE_QUAD;
            quads.data1 = (byte*)&quadrq[0];
            quads.count1 = quadrq.getCount();
            quads.depthtesting = true;
            quads.rendertarget = window.rendertarget;
            quads.updatescene = true;
            quads.view = identity();
            quads.projection = OrthographicMat4(window.bounds.left, window.bounds.right, window.bounds.bottom, window.bounds.top, -100.0f, 100.0f);
            quads.viewheight = (uint32)(window.bounds.top - window.bounds.bottom);
            quads.viewwidth = (uint32)(window.bounds.right - window.bounds.left);
            quads.viewx = 0;
            quads.viewy = 0;
            quads.bufferstoclear = IME_COLOR_BUFFER | IME_DEPTH_BUFFER;

            renderset->renderqueues.push_back(quads);

            RenderQueue textfields;
            textfields.commandtype = SIMPLE_TEXT;
            textfields.data1 = (byte*)&textrq[0];
            textfields.count1 = textrq.getCount();
            textfields.depthtesting = true;
            textfields.rendertarget = window.rendertarget;
            textfields.updatescene = true;
            textfields.view = identity();
            textfields.projection = OrthographicMat4(window.bounds.left, window.bounds.right, window.bounds.bottom, window.bounds.top, -100.0f, 100.0f);
            textfields.viewheight = (uint32)(window.bounds.top - window.bounds.bottom);
            textfields.viewwidth = (uint32)(window.bounds.right - window.bounds.left);
            textfields.viewx = 0;
            textfields.viewy = 0;
            textfields.bufferstoclear = 0;

            renderset->renderqueues.push_back(textfields);
        }

        ArrayList<SimpleQuadCommand> windowrq = ArrayList<SimpleQuadCommand>::create(0);
        for(uint32 i = 0; i < context.uiwindows.getCount(); i++) {
            if(context.uiwindows.getUnchecked(i).isoccupied == false) {
                continue;
            }

            const Window& window = context.uiwindows[i].data;

            SimpleQuadCommand command;
            command.position = toVec3(getPositionFromBounds(window.bounds), 0.0f);
            command.size = getSizeFromBounds(window.bounds);
            command.shader = compositshader;
            command.texture = window.rendertexture;
            command.color = {1.0f, 1.0f, 1.0f, 1.0f};

            windowrq.push_back(command);
        }

        RenderQueue windows;
        windows.commandtype = SIMPLE_QUAD;
        windows.data1 = (byte*)&windowrq[0];
        windows.count1 = windowrq.getCount();
        windows.depthtesting = true;
        windows.rendertarget = 0;
        windows.updatescene = true;
        windows.view = identity();
        windows.projection = OrthographicMat4(0.0f, (real32)platform.window.width, -(real32)platform.window.height, 0.0f, -100.0f, 100.0f);
        windows.viewheight = (uint32)(platform.window.height);
        windows.viewwidth = (uint32)(platform.window.width);
        windows.viewx = 0;
        windows.viewy = 0;
        windows.bufferstoclear = IME_COLOR_BUFFER | IME_DEPTH_BUFFER;

        renderset->renderqueues.push_back(windows);
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