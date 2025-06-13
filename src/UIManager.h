// UIManager.h
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Robot.h"
#include "Scene.h"
#include "Shader.h"

class UIManager {
public:
    UIManager(Robot* robot, Scene* scene, Shader* shader);
    void render();

private:
    Robot* robot;
    Scene* scene;
    Shader* shader;

    bool autoTour = false;
    bool showInfo = false;
    int currentObject = -1;

    float scanThreshold = 2.0f;
    std::vector<glm::vec3> objectPositions;
    std::vector<std::string> infoStrings;
};

#endif // UIMANAGER_H


