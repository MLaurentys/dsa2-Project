#include "ApplicationManager.h"
ApplicationManager::ApplicationManager() {


    state = GameState::MAIN_MENU;
}
ApplicationManager::~ApplicationManager() {}


void ApplicationManager::Run(const std::string& name, int size,
    bool fullscreen, bool borderless) {
    using namespace Simplex;
    Application* app = new Application();
    app->Init(name, size, fullscreen, borderless);
    while (state != GameState::CLOSE) {
        switch (state)
        {
        case GameState::MAIN_MENU:
            state = GameState::GAME;
            break;
        case GameState::GAME:
            state = app->RunFrame();
            break;
        case GameState::PAUSE_MENU:
            break;
        case GameState::END_MENU:
            break;
        default:
            break;
        }
    }
    SafeDelete(app);
}