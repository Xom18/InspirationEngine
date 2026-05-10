#include "InspirationEngine.h"

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    auto* mainWin = new IEWindow();
    mainWin->CreateWindow("InspirationEditor", 1280, 720);
    IECore::AddNewWindow("main", mainWin);
    IECore::AddWindowIndex(mainWin);

    IECore::BeginEngine();

    SDL_Event event;
    while (IECore::IsRunning())
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                IECore::StopEngine();
            IECore::EventPushBack(&event);
        }
        IECore::PollPlatformIME(SDL_GetKeyboardFocus());
    }

    IECore::EndEngine();
    SDL_Quit();
    return 0;
}
