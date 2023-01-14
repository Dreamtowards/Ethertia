//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUI_H
#define ETHERTIA_GUI_H

#include <vector>
#include <stack>

#include <ethertia/util/Mth.h>
#include <ethertia/util/UnifiedTypes.h>
#include <ethertia/util/Collections.h>
#include <ethertia/event/EventBus.h>
#include <ethertia/render/Texture.h>
#include <ethertia/util/Colors.h>

#include <ethertia/Ethertia.h>

class Gui
{
    float x = 0;
    float y = 0;

    // Inf: Match Parent, -Inf: Wrap Children
    // Why not use NaN? because NaN cannot directly compare/determine, needs an extra isnan() call.
    float width = 0;
    float height = 0;

    Gui* parent = nullptr;
    glm::vec2 childbound{0};

    bool hovered = false;
    bool pressed = false;
    bool focused = false;  // useful for TextBox etc.
    bool enable = true;    // effect to onClick().

    bool visible = true;
    bool clipping = false;

    void* tag = nullptr;


public:

    // always use Inf to represents nil arguments. not NaN. because NaN needs an extra isnan() call when validating.
    class Constraint
    {
    public:

        virtual void layout(Gui* g) = 0;

    };

    class AlignConstraint : public Gui::Constraint
    {
    public:
        float m_AlignX = Inf;
        float m_AlignY = Inf;

        AlignConstraint(float alignX, float alignY) : m_AlignX(alignX), m_AlignY(alignY) {}

        void layout(Gui* g) override
        {
            if (m_AlignX != Inf) {
                g->setRelativeX(
                        (g->getParent()->getWidth() - g->getWidth()) * m_AlignX
                );
            }
            if (m_AlignY != Inf) {
                g->setRelativeY(
                        (g->getParent()->getHeight() - g->getHeight()) * m_AlignY
                );
            }
        }
    };

    class LTRBConstraint : public Gui::Constraint
    {
    public:
        float m_Left = Inf,
              m_Top = Inf,
              m_Right = Inf,
              m_Bottom = Inf;

        // Stretch instead of move when set only one side.
        bool m_Stretch = false;

        LTRBConstraint(float l, float t, float r, float b, bool priorStretch) : m_Left(l),
                                                                                m_Top(t),
                                                                                m_Right(r),
                                                                                m_Bottom(b),
                                                                                m_Stretch(priorStretch) {}

        void layout(Gui* g) override
        {
            if (m_Left != Inf) { g->setRelativeX( m_Left ); }
            if (m_Top != Inf) {  g->setRelativeY( m_Top );  }

            if (m_Right != Inf) {
                float pw = g->getParent()->getWidth();
                if (m_Left != Inf || m_Stretch) {
                    g->setWidth( pw - g->getRelativeX() - m_Right );
                } else {
                    g->setRelativeX( pw - g->getWidth() - m_Right );
                }
            }

            if (m_Bottom != Inf) {
                float ph = g->getParent()->getHeight();
                if (m_Top != Inf || m_Stretch) {
                    g->setHeight( ph - g->getRelativeY() - m_Top );
                }
            }
        }

    };

    std::vector<Constraint*> m_Constraints;

    void addConstraintAlign(float alignX, float alignY) {
        m_Constraints.push_back(new AlignConstraint(alignX, alignY));
    }

    void addConstraintLTRB(float l, float t, float r, float b, bool priorStretch = false) {
        m_Constraints.push_back(new LTRBConstraint(l,t,r,b,priorStretch));
    }


public:
    EventBus eventbus;
    // Events: OnClick, OnHoverChanged, OnPressChanged, OnFocusChanged, OnAttachChanged, OnVisibleChanged
    // OnMouseIn/Out, OnDraw, OnLayout

    // cachedAbsXY?
    // 1. active: update-child-cachedpos when setPos
    // 2. lazy: Child-CachedPosInvalid, lazy getXY ToUpdate

public:
    static constexpr float NaN = Mth::NaN;
    static constexpr float Inf = Mth::Inf;

    class OnHover{};
    class OnFocus {};
    class OnPressed {};
    class OnReleased {};
    class OnClick {};
    // OnAttached, OnDetached

    struct OnDraw { Gui* gui; };
    class OnLayout {};

    Gui() {}

    Gui(float _rx, float _ry) : x(_rx), y(_ry) {}

    Gui(float _rx, float _ry, float _w, float _h) : x(_rx), y(_ry), width(_w), height(_h) {}

    ////////////// SIZES //////////////

    float getRelativeX() const { return x; }
    float getRelativeY() const { return y; }

    void setRelativeX(float _rx) {
        if (_rx == x) return;
        x = _rx;
    }
    void setRelativeY(float _ry) {
        if (_ry == y) return;
        y = _ry;
    }
    void setRelativeXY(float _rx, float _ry) {
        setRelativeX(_rx);
        setRelativeY(_ry);
    }

    float getX() const {
        if (!parent) return x;
        return parent->getX() + x;
    }
    float getY() const {
        if (!parent) return y;
        return parent->getY() + y;
    }

    void setX(float _x) {
        float px = parent ? parent->getX() : 0;
        setRelativeX(_x - px);
    }
    void setY(float _y) {
        float py = parent ? parent->getY() : 0;
        setRelativeY(_y - py);
    }

    virtual float getWidth() const {
        if (!visible) return 0;
        if (width == -Inf) return childbound.x;
        if (width == Inf) { assert(parent->width != -Inf); return parent->getWidth() - x; } // sub x?
        return width;
    }
    void setWidth(float w) {
        width = w;
    }
    virtual float getHeight() const {
        if (!visible) return 0;
        if (height == -Inf) return childbound.y;
        if (height == Inf) { assert(parent->height != -Inf); return parent->getHeight() - y; }
        return height;
    }
    void setHeight(float h) {
        height = h;
    }
    void setWidthHeight(float w, float h) {
        setWidth(w);
        setHeight(h);
    }


    // Universal Interface for Initiative Iteration

    virtual int count() const {
        return 0;
    }
    virtual Gui* at(int i) {
        return nullptr;
    }

    class _GIterable {public:
        Gui* g;
        _GIterable(Gui *g) : g(g) {}
        class Iter {public:
            Gui* g;
            int i;
            Iter(Gui *g, int i) : g(g), i(i) {}
            Gui* operator*() const { return g->at(i); }
            Iter operator++() { ++i; return *this; }
            Iter operator--() { --i; return *this; }
            friend bool operator!=(const Iter& lhs, const Iter& rhs) { return lhs.i!=rhs.i || lhs.g!=rhs.g; }
        };
        Iter begin() const { return Iter(g, 0); }
        Iter end() const { return Iter(g, g->count()); }
    };

    _GIterable children() {
        return _GIterable(this);
    }


    ////////////// CHILDREN //////////////

    Gui* getParent() const {
        return parent;
    }
    void setParent(Gui* p) {
        parent = p;
    }
//    void detach() {
//        parent = nullptr;
//    }
//    void attach(Gui* p) {
//        parent = p;
//    }
//    void removeFromParent() {}

    static void forParents(Gui* g, const std::function<void(Gui*)>& visitor) {
        if (!g) return;
        visitor(g);

        while ((g=g->parent)) {
            visitor(g);
        }
    }

    static void forChildren(Gui* g, const std::function<void(Gui*)>& visitor) {
        if (!g) return;
        visitor(g);

        for (auto it = g->children().begin(); it != g->children().end(); ++it) {
            Gui* child = *it;
            Gui::forChildren(child, visitor);
        }
    }




    bool isHover() const { return hovered; }
    void setHover(bool h) {
        if (h == hovered) return;
        hovered = h;

        fireEvent(OnHover());
    }

    bool isPressed() { return pressed; }
    void setPressed(bool p) {
        if (p == pressed) return;
        pressed = p;

        if (p) fireEvent(OnPressed());
        else   fireEvent(OnReleased());
    }

    bool isFocused() const { return focused; }
    void setFocused(bool f) {
        if (f == focused) return;
        focused = f;

        fireEvent(OnFocus());
    }

    bool isVisible() {
        return visible;
    }
    void setVisible(bool v) {
        if (v == visible) return;
        visible = v;

//        fireEvent(OnVisible());
    }
    void toggleVisible() {
        setVisible(!isVisible());
    }



    virtual void onKeyboard(int key, bool pressed) {

    }
    virtual void onMouseButton(int button, bool pressed) {

    }
    virtual void onCharInput(int ch) {

    }
    virtual void onScroll(float dx, float dy) {

    }



    EventBus& getEventBus() {
        return eventbus;
    }

    template<typename E>
    void fireEvent(E e) {
        eventbus.post(&e);
    }

    template<typename E>
    void broadcastEvent() {
        E e{};
        Gui::forChildren(this, [&e](Gui* g) {
            g->eventbus.post(&e);
        });
    }

    ////////////// ?? //////////////

    bool isPointOver(float px, float py) {
        float x = getX(), y = getY();
        return px >= x && px < x + getWidth() &&
               py >= y && py < y + getHeight();
    }
    bool isPointOver(const glm::vec2& p) {
        return isPointOver(p.x, p.y);
    }

    static Gui* pointing(Gui* g, glm::vec2 p) {
        if (!g->isPointOver(p))
            return nullptr;

        for (int i = g->count()-1; i >= 0; --i) {
            if (Gui* r = pointing(g->at(i), p))
                return r;
        }
        // except GuiAlign. which just an adjuster regardless self-size/interaction.
        return g;
    }


    template<typename LAMBDA, typename T = EventBus::InternalUtil::GetLambdaEventType<LAMBDA>>
    T* exec(LAMBDA l) {
        l.operator()((T*)this);
        return (T*)this;
    }

    // override this instead of onDraw(). to keep the arch, OnDrawPre, OnDrawPost, visibility.
    virtual void implDraw() {}

    virtual void onDraw()
    {
        if (!visible) return;

        fireEvent(OnDraw{this});

        implDraw();

        for (Gui* g : children())
        {
            g->onDraw();
        }
    }


    virtual void onLayout()
    {
        if (!visible) return;

        _update_childbound();

        for (Constraint* c : m_Constraints)
        {
            c->layout(this);
        }

        for (Gui* g : children())
        {
            g->onLayout();
        }
    }

    void _update_childbound() {
        if (width != -Inf && height != -Inf)
            return;
        glm::vec2 mx(0);
        for (auto* g : children()) {
            mx.x = Mth::max(mx.x, g->getRelativeX() + g->getWidth());  // what if child size is MATCH_PARENT? then should ignore size but consider pos, or throw logical exception
            mx.y = Mth::max(mx.y, g->getRelativeY() + g->getHeight());
        }
        childbound = mx;
    }



    void addOnDrawListener(const std::function<void(OnDraw*)>& lsr) {

        eventbus.listen(lsr);
    }
    void addDrawBackground(glm::vec4 color) {
        addOnDrawListener([=](OnDraw* e){
            Gui* g = e->gui;
            Gui::drawRect(g->getX(), g->getY(), g->getWidth(), g->getHeight(), color);
        });
    }
    void addOnClickListener(const std::function<void(OnReleased*)>& lsr) {

        eventbus.listen(lsr);
    }
    void addOnPressedListener(const std::function<void(OnPressed*)>& lsr) {

        eventbus.listen(lsr);
    }




    static float maxWidth();
    static float maxHeight();

    static float mfbWidth();   // Main Framebuffer size.
    static float mfbHeight();

    static float cursorX();
    static float cursorY();

    bool isCursorOver()  {
        return isPointOver(Gui::cursorX(), Gui::cursorY());
    }

    static bool isCursorOver(float x, float y, float w, float h) {
        float cx = Gui::cursorX();
        float cy = Gui::cursorY();
        return cx >= x && cx < x+w &&
               cy >= y && cy < y+h;
    }

    struct DrawRectArgs {
        inline static const int C_RGBA = 0,  // Channel Modes
                                C_RGB = 1,
                                C_AAA = 2;
        glm::vec4 color = Colors::WHITE;
        Texture* tex = nullptr;
        float round = 0;
        float border = 0;
        int channel_mode = 0;
        glm::vec2 tex_pos = {0,0};  // GL Tex Coords. 0,0: LB, 1,1: RT
        glm::vec2 tex_size = {1,1}; // Dir toward: RT
    };
    static void drawRect(float x, float y, float w, float h, Gui::DrawRectArgs args);

    static void drawRect(float x, float y, float w, float h, glm::vec4 color) {
        Gui::drawRect(x,y,w,h,{
            .color = color
        });
    }
    static void drawRect(float x, float y, float w, float h, Texture* tex) {
        Gui::drawRect(x,y,w,h,{
                .tex = tex
        });
    }


    // alignX: x+=LineWidth *f  e.g.  0.5 = Center, 1.0 = Right
    // alignY: y+=FullTextHeight*f   -0.5 = Center, -1.0 = Upward.
    static void drawString(float x, float y, const std::string& str,
                           glm::vec4 color  =Colors::WHITE,
                           float textHeight =16,
                           glm::vec2 align  ={0,0},
                           bool drawShadow  =true);

    static void drawWorldpoint(const glm::vec3& worldpos, const std::function<void(glm::vec2)>& fn);


    //
    static void drawStretchCorners(float x, float y, float w, float h, Texture* tex, float thickness = 16, bool onlyLTCornerTex = false) {

        float tk = thickness;
        float iw = w-tk-tk;  // inner width
        float ih = h-tk-tk;

        if (onlyLTCornerTex)
        {
            // 4 Corners
            Gui::drawRect(x,y,tk,tk, {.tex=tex, .tex_pos={0,1.0}, .tex_size={1.0,1.0}});  // LT
            Gui::drawRect(x+w-tk,y,tk,tk, {.tex=tex, .tex_pos={1.0,1.0}, .tex_size={-1.0,1.0}});  // RT
            Gui::drawRect(x,y+h-tk,tk,tk, {.tex=tex, .tex_pos={0,0}, .tex_size={1.0,-1.0}});  // LB
            Gui::drawRect(x+w-tk,y+h-tk,tk,tk, {.tex=tex, .tex_pos={1.0, 0}, .tex_size={-1.0,-1.0}});  // RB
            
            // Center
            Gui::drawRect(x+tk,y+tk,iw,ih, {.tex=tex, .tex_pos={0.99,0.01}, .tex_size={0,0}});  // RB

            // 4 Borders
            Gui::drawRect(x,y+tk,tk,ih, {.tex=tex, .tex_pos={0,0}, .tex_size={1.0,0}});  // L
            Gui::drawRect(x+w-tk,y+tk,tk,ih, {.tex=tex, .tex_pos={0,0}, .tex_size={-1.0,0}});  // R
            Gui::drawRect(x+tk,y,iw,tk, {.tex=tex, .tex_pos={0.99,1.0}, .tex_size={0,1.0}});  // T
            Gui::drawRect(x+tk,y+h-tk,iw,tk, {.tex=tex, .tex_pos={0.99,0}, .tex_size={0,-1.0}});  // B
        } 
        else 
        {
            // 4 Corners
            Gui::drawRect(x,y,tk,tk, {.tex=tex, .tex_pos={0,0.5}, .tex_size={0.5,0.5}});  // LT
            Gui::drawRect(x+w-tk,y,tk,tk, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0.5,0.5}});  // RT
            Gui::drawRect(x,y+h-tk,tk,tk, {.tex=tex, .tex_pos={0,0}, .tex_size={0.5,0.5}});  // LB
            Gui::drawRect(x+w-tk,y+h-tk,tk,tk, {.tex=tex, .tex_pos={0.5, 0}, .tex_size={0.5,0.5}});  // RB

            // Center
            Gui::drawRect(x+tk,y+tk,iw,ih, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0,0}});  // RB

            // 4 Borders
            Gui::drawRect(x,y+tk,tk,ih, {.tex=tex, .tex_pos={0,0.5}, .tex_size={0.5,0}});  // L
            Gui::drawRect(x+w-tk,y+tk,tk,ih, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0.5,0}});  // R
            Gui::drawRect(x+tk,y,iw,tk, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0,0.5}});  // T
            Gui::drawRect(x+tk,y+h-tk,iw,tk, {.tex=tex, .tex_pos={0.5,0}, .tex_size={0,0.5}});  // B
        }
    }


    inline static std::vector<glm::vec4> g_Scissors;

    static void gPushScissor(float x, float y, float w, float h) {
        if (g_Scissors.empty()) {
            glEnable(GL_SCISSOR_TEST);
        }
        // final clip Overlapped Area (max(a.min, b.min), min(a.max, b.max), and ensure min < max.)
        float begX = x, begY = y, endX = x+w, endY = y+h;
        for (glm::vec4& v : g_Scissors) {
            begX = Mth::max(begX, v.x);
            begY = Mth::max(begY, v.y);
            endX = Mth::min(endX, v.z);
            endY = Mth::min(endY, v.w);
        }
        float clipWidth  = endX - begX;
        float clipHeight = endY - begY;

        glm::vec4 s(begX, begY, clipWidth, clipHeight);
        g_Scissors.push_back(s);
        glfScissor(begX, begY, clipWidth, clipHeight);
    }

    static void gPopScissor() {
        g_Scissors.pop_back();
        if (g_Scissors.empty()) {
            glDisable(GL_SCISSOR_TEST);
        } else {
            glm::vec4& v = g_Scissors.back();
            glfScissor(v.x, v.y, v.z, v.w);
        }
    }

    static void glfScissor(float x, float y, float w, float h) {
        glScissor(Gui::toFramebufferCoords(x),
                  Gui::toFramebufferCoords(Gui::maxHeight()-y-h),
                  Gui::toFramebufferCoords(w),
                  Gui::toFramebufferCoords(h));
    }

    static float toFramebufferCoords(float f);

// drawCornerStretchTexture

// needs GuiGroup?
// what if a Gui only have one content-child?, or non-child, then children shows confuse, waste and logical non-compact.
// but if separate GuiGroup, then the Overall-Through-Iteration might become a problem. since interface diff.

// Gui* getSub() {
// }

// GuiCheckBox
// GuiComboBox
// GuiDrag
// GuiExpander
// GuiTexture
// GuiPopupMenu
// GuiRadioButton
// GuiScrollbar
// GuiScrollBox
// GuiSlider
// GuiSwitch
// GuiText
// GuiTextBox

};

#endif //ETHERTIA_GUI_H
