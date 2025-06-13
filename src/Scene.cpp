// Scene.cpp
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <iostream>
#include <limits>

void Scene::init()
{
    initRoom();
    initModels();
    computeBounds();
}

void Scene::initRoom()
{
    // Floor vertices: pos(3), normal(3), texcoords(2)
    float floorVertices[] = {
        -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,
        5.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 5.0f,
        -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        5.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 5.0f,
        -5.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f};
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    // attrib 0: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // attrib 1: normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // attrib 2: texcoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Walls: four walls around floor
    std::array<glm::vec3, 4> wallNormals = {
        glm::vec3(0, 0, 1),  // back wall
        glm::vec3(1, 0, 0),  // left wall
        glm::vec3(0, 0, -1), // front wall
        glm::vec3(-1, 0, 0)  // right wall
    };
    std::array<glm::vec3, 4> wallPositions = {
        glm::vec3(0, 0, -5),
        glm::vec3(-5, 0, 0),
        glm::vec3(0, 0, 5),
        glm::vec3(5, 0, 0)};
    for (int i = 0; i < 4; ++i)
    {
        float nx = wallNormals[i].x;
        float nz = wallNormals[i].z;
        // vertical plane size 10x3
        float wallVertices[] = {
            -5.0f * nz + wallPositions[i].x, 0.0f, -5.0f * nx + wallPositions[i].z, nx, 0.0f, nz, 0.0f, 0.0f,
            5.0f * nz + wallPositions[i].x, 0.0f, 5.0f * nx + wallPositions[i].z, nx, 0.0f, nz, 5.0f, 0.0f,
            5.0f * nz + wallPositions[i].x, 3.0f, 5.0f * nx + wallPositions[i].z, nx, 0.0f, nz, 5.0f, 3.0f,
            -5.0f * nz + wallPositions[i].x, 0.0f, -5.0f * nx + wallPositions[i].z, nx, 0.0f, nz, 0.0f, 0.0f,
            5.0f * nz + wallPositions[i].x, 3.0f, 5.0f * nx + wallPositions[i].z, nx, 0.0f, nz, 5.0f, 3.0f,
            -5.0f * nz + wallPositions[i].x, 3.0f, -5.0f * nx + wallPositions[i].z, nx, 0.0f, nz, 0.0f, 3.0f};
        glGenVertexArrays(1, &wallVAO[i]);
        glGenBuffers(1, &wallVBO[i]);
        glBindVertexArray(wallVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }
}

void Scene::initModels()
{
    std::vector<std::string> modelPaths = {
        "models/heykel.obj",
        "models/manstatue.obj",
        "models/modelleme.obj",
        "models/roma_mezar_modelleme.obj",
        "models/roma_yeni.obj"};
    std::vector<glm::vec3> positions = {
        {2.0f, 0.0f, 3.0f},
        {-3.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -2.5f},
        {4.0f, 0.0f, -1.0f},
        {-1.5f, 0.0f, -3.5f}};
    models.clear(); // Ensure we start with empty models
    for (size_t i = 0; i < modelPaths.size(); ++i)
    {
        try
        {
            Model model(modelPaths[i]);
            model.setUniformScale(1.8f); // örnek: her heykeli 1.8 m yüksekliğe göre ölçekle
            model.autoGround(0.0f);      // tabanı zemine yasla
            model.setPosition(positions[i]);
            models.push_back(model);
            std::cout << "Loaded model: " << modelPaths[i] << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "ERROR loading model " << modelPaths[i] << ": " << e.what() << std::endl;
        }
    }
}

void Scene::draw(Shader &shader)
{
    // Draw floor
    shader.setMat4("model", glm::mat4(1.0f));
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Draw walls
    for (int i = 0; i < 4; ++i)
    {
        glBindVertexArray(wallVAO[i]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Draw models - FIXED: Only draw if we have models
    if (!models.empty())
    {
        for (auto &model : models)
        {
            model.draw(shader);
        }
    }
    else
    {
        std::cerr << "WARNING: No models to draw!" << std::endl;
    }
}

/*void Scene::getSceneBounds(glm::vec3 &center, float &radius) const
{
    // 1) Önce AABB min/max değerleri hazırla
    glm::vec3 bbMin(std::numeric_limits<float>::max());
    glm::vec3 bbMax(-std::numeric_limits<float>::max());

    // 2) Tüm modelleri dolaş, her bir mesh için
    for (const auto &model : models)
    {
        // Model’in world-transform’ini al (Model sınıfınıza göre uyarlayın)
        glm::mat4 modelMatrix = model.getTransformMatrix();

        for (const auto &mesh : model.getMeshes())
        {
            for (const auto &v : mesh.vertices)
            {
                // vertex’i dünya koordinatına taşı
                glm::vec3 worldPos = glm::vec3(modelMatrix * glm::vec4(v.Position, 1.0f));
                bbMin = glm::min(bbMin, worldPos);
                bbMax = glm::max(bbMax, worldPos);
            }
        }
    }

    // 3) Merkez ve yarıçapı hesapla
    center = (bbMin + bbMax) * 0.5f;
    radius = glm::length(bbMax - center);
}*/

void Scene::computeBounds()
{
    glm::vec3 bbMin(std::numeric_limits<float>::max());
    glm::vec3 bbMax(-std::numeric_limits<float>::max());

    for (const auto &model : models)
    {
        // Model sınıfınız world‑transform veriyorsa kullanın; yoksa identity.
        glm::mat4 M = model.getTransformMatrix(); // ← kendi API’nıza uyarlayın

        for (const auto &mesh : model.getMeshes())
        {
            for (const auto &v : mesh.vertices)
            {
                glm::vec3 worldPos = glm::vec3(M * glm::vec4(v.Position, 1.0f));
                bbMin = glm::min(bbMin, worldPos);
                bbMax = glm::max(bbMax, worldPos);
            }
        }
    }

    // Eğer hiç model yoksa, odanın sınırlarını kullanabilirsiniz (isteğe bağlı)
    sceneCenter = (bbMin + bbMax) * 0.5f;
    sceneRadius = glm::length(bbMax - sceneCenter);

    // Koruma: radius çok küçükse minimal değer ata
    if (sceneRadius < 0.1f)
        sceneRadius = 0.1f;
}
