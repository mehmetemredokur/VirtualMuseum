#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Model.h"
#include <glad/glad.h>

class Scene
{
public:
    void init();
    void draw(Shader &shader);

    void getSceneBounds(glm::vec3 &center, float &radius) const
    {
        center = sceneCenter;
        radius = sceneRadius;
    }

private:
    unsigned int floorVAO = 0, floorVBO = 0;
    unsigned int wallVAO[4] = {0}, wallVBO[4] = {0};

    std::vector<Model> models;

    void initRoom();
    void initModels();

    void computeBounds();
    glm::vec3 sceneCenter{0.0f};
    float sceneRadius = 1.0f;
};

#endif
