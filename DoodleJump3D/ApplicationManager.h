#ifndef __APPLICATION_MANAGER_H
#define __APPLICATION_CLASS_H

#include "Definitions.h"
#include "AppClass.h"

class ApplicationManager
{
public:
    

    ~ApplicationManager();
    ApplicationManager();
    void Run(const std::string& name, int size,
        bool fullscreen, bool borderless);
private:
    GameState state;
};
#endif

