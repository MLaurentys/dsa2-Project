#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "Simplex\Simplex.h"
#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include "SFML\OpenGL.hpp"
#include "SFML\Audio.hpp"
#include "imgui\ImGuiObject.h"

using namespace Simplex;

class Application {
public:
    Application(const String& name, uint widht = 600, uint height = 600);
    void Run();
private:
    SystemSingleton* system;
    LightManager* lightManager;
    sf::Window* window;

    void Update();
    void Display();
};

#endif