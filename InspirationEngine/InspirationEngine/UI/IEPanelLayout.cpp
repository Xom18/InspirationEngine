#include "../InspirationEngine.h"
#include <nlohmann/json.hpp>
#include <fstream>

// --- Anchor / mode helpers ---

SDL_FPoint IEPanelLayout::ResolveAnchorPoint(IEAnchor a,
    float ox, float oy, float w, float h) const
{
    float col = static_cast<float>(static_cast<int>(a) % 3) * 0.5f;
    float row = static_cast<float>(static_cast<int>(a) / 3) * 0.5f;
    return { ox + col * w, oy + row * h };
}

IEAnchor IEPanelLayout::ParseAnchor(const std::string& s)
{
    if (s == "TopCenter")    return IEAnchor::TopCenter;
    if (s == "TopRight")     return IEAnchor::TopRight;
    if (s == "MiddleLeft")   return IEAnchor::MiddleLeft;
    if (s == "Center")       return IEAnchor::Center;
    if (s == "MiddleRight")  return IEAnchor::MiddleRight;
    if (s == "BottomLeft")   return IEAnchor::BottomLeft;
    if (s == "BottomCenter") return IEAnchor::BottomCenter;
    if (s == "BottomRight")  return IEAnchor::BottomRight;
    return IEAnchor::TopLeft;
}

IESizeMode IEPanelLayout::ParseMode(const std::string& s)
{
    if (s == "ratio") return IESizeMode::Ratio;
    return IESizeMode::Pixel;
}

// --- Core layout calculation ---

void IEPanelLayout::ApplyElement(const IELayoutElement& e) const
{
    if (e.isLabel || e.widget == nullptr)
        return;

    float ox = static_cast<float>(m_originX);
    float oy = static_cast<float>(m_originY);
    float pw = static_cast<float>(m_panelW);
    float ph = static_cast<float>(m_panelH);

    SDL_FPoint axisPoint = ResolveAnchorPoint(e.axis, ox, oy, pw, ph);

    float ax = e.x;
    float ay = e.y;
    if (e.posMode == IESizeMode::Ratio)
    {
        ax *= pw;
        ay *= ph;
    }

    float absW, absH;

    // width < 0: 오른쪽 경계까지 stretch, |width| = 오른쪽 여백
    if (e.width < 0.0f)
    {
        float rightEdge = ox + pw + e.width;
        float leftEdge  = axisPoint.x + ax;
        absW = rightEdge - leftEdge;
        if (absW < 0.0f) absW = 0.0f;
    }
    else if (e.sizeMode == IESizeMode::Ratio)
        absW = e.width * pw;
    else
        absW = e.width;

    // height < 0: 아래쪽 경계까지 stretch
    if (e.height < 0.0f)
    {
        float bottomEdge = oy + ph + e.height;
        float topEdge    = axisPoint.y + ay;
        absH = bottomEdge - topEdge;
        if (absH < 0.0f) absH = 0.0f;
    }
    else if (e.sizeMode == IESizeMode::Ratio)
        absH = e.height * ph;
    else
        absH = e.height;

    // stretch일 때는 pivot 오프셋 없음(위젯 좌상단이 axisPoint + offset)
    SDL_FPoint pivotPoint = { 0.0f, 0.0f };
    if (e.width >= 0.0f && e.height >= 0.0f)
        pivotPoint = ResolveAnchorPoint(e.pivot, 0.0f, 0.0f, absW, absH);

    float left = axisPoint.x + ax - pivotPoint.x;
    float top  = axisPoint.y + ay - pivotPoint.y;

    e.widget->SetRect(
        static_cast<int32_t>(left), static_cast<int32_t>(top),
        static_cast<int32_t>(absW), static_cast<int32_t>(absH));
}

// --- Internal helpers ---

void IEPanelLayout::PropagateCommon(IEUIBase* widget) const
{
    if (m_renderer != nullptr) widget->SetRenderer(m_renderer);
    if (m_font     != nullptr) widget->SetFont(m_font);
    if (m_ownerWin != nullptr) widget->SetOwnerWindow(m_ownerWin);
}

// --- JSON loading ---

bool IEPanelLayout::LoadJson(const char* filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        SDL_Log("IEPanelLayout: cannot open '%s'", filePath);
        return false;
    }

    nlohmann::json j;
    try
    {
        j = nlohmann::json::parse(file);
    }
    catch (...)
    {
        SDL_Log("IEPanelLayout: JSON parse error in '%s'", filePath);
        return false;
    }

    auto& elems = j["elements"];
    if (!elems.is_array())
        return false;

    for (auto& elem : elems)
    {
        IELayoutElement e;
        e.name     = elem.value("name",    "");
        e.axis     = ParseAnchor(elem.value("axis",    "TopLeft"));
        e.pivot    = ParseAnchor(elem.value("pivot",   "TopLeft"));
        e.x        = elem.value("x",       0.0f);
        e.y        = elem.value("y",       0.0f);
        e.width    = elem.value("width",   0.0f);
        e.height   = elem.value("height",  0.0f);
        e.posMode  = ParseMode(elem.value("posMode",  "pixel"));
        e.sizeMode = ParseMode(elem.value("sizeMode", "pixel"));
        e.isLabel  = (elem.value("type", "widget") == "label");

        if (e.isLabel)
        {
            e.labelText = elem.value("text", "");
            if (elem.contains("color"))
            {
                auto& c    = elem["color"];
                e.labelColor = {
                    static_cast<Uint8>(c.value("r", 180)),
                    static_cast<Uint8>(c.value("g", 180)),
                    static_cast<Uint8>(c.value("b", 180)),
                    static_cast<Uint8>(c.value("a", 255))
                };
            }
        }
        m_elements.push_back(std::move(e));
    }
    return true;
}

// --- Public API ---

void IEPanelLayout::Bind(const char* name, IEUIBase* widget)
{
    for (auto& e : m_elements)
    {
        if (e.name != name)
            continue;
        e.widget = widget;
        PropagateCommon(widget);
        ApplyElement(e);
        return;
    }
}

void IEPanelLayout::Add(IEUIBase* widget,
    IEAnchor axis, IEAnchor pivot,
    float x, float y, float w, float h,
    IESizeMode posMode, IESizeMode sizeMode)
{
    IELayoutElement e;
    e.widget   = widget;
    e.axis     = axis;
    e.pivot    = pivot;
    e.x        = x;
    e.y        = y;
    e.width    = w;
    e.height   = h;
    e.posMode  = posMode;
    e.sizeMode = sizeMode;
    m_elements.push_back(std::move(e));
    if (widget != nullptr)
        PropagateCommon(widget);
    ApplyElement(m_elements.back());
}

void IEPanelLayout::AddLabel(const char* text,
    IEAnchor axis, IEAnchor pivot,
    float x, float y, SDL_Color color)
{
    IELayoutElement e;
    e.isLabel    = true;
    e.labelText  = text;
    e.labelColor = color;
    e.axis       = axis;
    e.pivot      = pivot;
    e.x          = x;
    e.y          = y;
    m_elements.push_back(std::move(e));
}

void IEPanelLayout::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_originX = x;
    m_originY = y;
    m_panelW  = w;
    m_panelH  = h;
    for (const auto& e : m_elements)
        ApplyElement(e);
}

void IEPanelLayout::SetFont(IEFont* font)
{
    m_font = font;
    for (const auto& e : m_elements)
        if (!e.isLabel && e.widget != nullptr)
            e.widget->SetFont(font);
}

void IEPanelLayout::SetOwnerWindow(IEWindow* win)
{
    m_ownerWin = win;
    for (const auto& e : m_elements)
        if (!e.isLabel && e.widget != nullptr)
            e.widget->SetOwnerWindow(win);
}

void IEPanelLayout::SetRenderer(IERenderer* renderer)
{
    m_renderer = renderer;
    for (const auto& e : m_elements)
        if (!e.isLabel && e.widget != nullptr)
            e.widget->SetRenderer(renderer);
}

void IEPanelLayout::Draw()
{
    for (const auto& e : m_elements)
    {
        if (e.isLabel)
        {
            if (m_renderer == nullptr || m_font == nullptr || e.labelText.empty())
                continue;

            float ox = static_cast<float>(m_originX);
            float oy = static_cast<float>(m_originY);
            float pw = static_cast<float>(m_panelW);
            float ph = static_cast<float>(m_panelH);
            float fh = static_cast<float>(m_font->GetHeight());

            SDL_FPoint axisPoint  = ResolveAnchorPoint(e.axis,  ox, oy, pw, ph);
            SDL_FPoint pivotPoint = ResolveAnchorPoint(e.pivot, 0.0f, 0.0f, 0.0f, fh);

            float drawX = axisPoint.x + e.x - pivotPoint.x;
            float drawY = axisPoint.y + e.y - pivotPoint.y;

            m_renderer->DrawText(m_font, e.labelText.c_str(), e.labelColor,
                static_cast<int32_t>(drawX), static_cast<int32_t>(drawY));
        }
        else if (e.widget != nullptr)
        {
            e.widget->Draw();
        }
    }
}

void IEPanelLayout::Update()
{
    for (const auto& e : m_elements)
        if (!e.isLabel && e.widget != nullptr)
            e.widget->Update();
}
