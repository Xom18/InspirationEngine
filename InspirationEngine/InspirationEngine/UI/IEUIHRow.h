#pragma once
#include <vector>
#include <string>
#include <memory>

/// <summary>
/// 여러 (레이블 + 위젯) 쌍을 가로로 배치하는 행 컨테이너.
/// IEUISection::AddHRow() 로 생성 후 AddItem() 으로 위젯 추가.
/// </summary>
class IEUIHRow : public IEUIBase
{
public:
    bool IsRow()           const override { return true; }
    void SetRowParity(bool even) override { m_even = even; }

    template<typename T>
    T* AddItem(const std::string& label, std::unique_ptr<T> widget)
    {
        m_labels.push_back(label);
        return AddChild(std::move(widget));
    }

    int32_t Layout(int32_t x, int32_t y, int32_t w) override;
    void    Draw()        override;
    void    Update()      override;
    void    DrawOverlay() override;

private:
    std::vector<std::string> m_labels;
    bool m_even = true;

    static constexpr int32_t   kRowH      = 24;
    static constexpr int32_t   kLabelW    = 25;
    static constexpr SDL_Color kColRowBg  = {  32,  32,  38, 255 };
    static constexpr SDL_Color kColRowAlt = {  36,  36,  42, 255 };
    static constexpr SDL_Color kColLabel  = { 140, 140, 150, 255 };
};
