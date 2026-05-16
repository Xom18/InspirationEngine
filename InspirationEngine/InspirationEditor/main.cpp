#ifdef _WIN32
#include <windows.h>
#endif
#include "IEMainEditorWindow.h"
#include "IEAtlasEditorWindow.h"

int main(int /*argc*/, char* /*argv*/[])
{
#if defined(_WIN32) && defined(_DEBUG)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");

    if (const char* bp = SDL_GetBasePath())
    {
        SetCurrentDirectoryA(bp);
        SDL_free(const_cast<char*>(bp));
    }

    IELocalize::Load("Data/Text/en.json");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    constexpr int32_t kTickRate = 1000 / 60;

    auto* mainWin  = new IEMainEditorWindow();
    auto* atlasWin = new IEAtlasEditorWindow();

    mainWin->CreateWindow(IELocalize::Get("win.editor"), 1280, 720, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(mainWin->GetSDLWindow(), IEMainEditorWindow::kMinW, IEMainEditorWindow::kMinH);
    IECore::AddNewWindow("main", mainWin);
    IECore::AddWindowIndex(mainWin);

    atlasWin->CreateWindow(IELocalize::Get("win.atlas_editor"), 1280, 720);
    atlasWin->HideWindow();
    IECore::AddNewWindow("atlas", atlasWin);
    IECore::AddWindowIndex(atlasWin);

    IECore::SetTickRate(kTickRate);

    IECore::GetFont().AddNewFont(0, "Data/H2PORL.TTF", 14);
    IEFont* font = IECore::GetFont().GetFont(0);

    mainWin->InitWindow(font, atlasWin);
    atlasWin->InitWindow(font);

    IECore::BeginEngine();

    SDL_Window* lastFocusWin = nullptr;

    while (IECore::IsRunning())
    {
        SDL_Event event;
        if (SDL_WaitEventTimeout(&event, kTickRate))
        {
            IECore::EventPushBack(&event);

            while (SDL_PollEvent(&event))
                IECore::EventPushBack(&event);
        }

        if (event.type == SDL_EVENT_QUIT)
            IECore::StopEngine();

        SDL_Window* focusWin = SDL_GetKeyboardFocus();
        if (focusWin != lastFocusWin)
        {
            if (lastFocusWin != nullptr)
                SDL_StopTextInput(lastFocusWin);
            if (focusWin != nullptr)
                SDL_StartTextInput(focusWin);
            lastFocusWin = focusWin;
        }

        auto rect = IECore::GetTextEditPosition();
        if (rect.has_value() && focusWin != nullptr)
            SDL_SetTextInputArea(focusWin, &rect.value(), 0);

        IECore::PollPlatformIME(focusWin);
    }

    IECore::EndEngine();
    return 0;
}
