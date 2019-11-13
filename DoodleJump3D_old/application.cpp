#include "application.h"

Application::Application (const String& name, uint widht, uint height){
    // System init
    system = SystemSingleton::GetInstance();
    lightManager = LightManager::GetInstance();
    system->SetWindowName(name);
    system->SetWindowWidth(widht);
    system->SetWindowHeight(height);
    // Windows init
    window = new sf::Window(sf::VideoMode(system->GetWindowWidth(), system->GetWindowHeight(), 32),
        name, sf::Style::Default, sf::ContextSettings(24, 0, 0, 4, 5, 0, false));
    window->setVerticalSyncEnabled(true);
    window->setFramerateLimit(system->GetMaxFrameRate());
    // GLEW init
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);		// The Type Of Depth Testing To Do
    glEnable(GL_CULL_FACE);
};

void Application::Run() {
    while (true) {
        Update();
        Display();
    }
};

void Application::Update() {

};

void Application::Display() {
    window->display();
};