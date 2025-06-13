// Robot.h
#ifndef ROBOT_H
#define ROBOT_H

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include <glad/glad.h>

class Robot {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float speed = 2.5f;

    // Mesh handles
    unsigned int VAO = 0, VBO = 0;

    Robot();
    ~Robot();
    void update(float deltaTime);
    void draw(Shader &shader);
    void setPath(const std::vector<glm::vec3> &waypoints);

private:
    std::vector<glm::vec3> waypoints;
    int currentTarget = 0;

    void initMesh();
};

#endif // ROBOT_H