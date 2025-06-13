#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>

class Model
{
public:
    Model(const std::string &path);
    void draw(Shader &shader);
    void setPosition(const glm::vec3 &pos);
    const std::vector<Mesh> &getMeshes() const;
    void autoGround(float desiredHeight = 0.0f);
    void setUniformScale(float targetHeight);
    glm::mat4 getTransformMatrix() const;

private:
    // Model verisi
    std::vector<Mesh> meshes;
    std::string directory;
    glm::vec3 position{0.0f};
    glm::vec3 bbMin, bbMax;
    float scale = 1.0f;

    // Assimp işleme fonksiyonları
    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
};

#endif
