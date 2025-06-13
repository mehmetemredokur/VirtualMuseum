// Model.cpp
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <iostream>
#include <stdexcept>                    // For error handling
#include <glm/gtc/matrix_transform.hpp> // translate için

Model::Model(const std::string &path)
{
    loadModel(path);
}

void Model::setPosition(const glm::vec3 &pos)
{
    position = pos;
}

void Model::draw(Shader &shader)
{
    // 1) Model uzayından dünya uzayına dönüşüm
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), position);
    modelMat = glm::scale(modelMat, glm::vec3(scale)); // <-- YENİ SATIR

    // 2) Shader’a gönder
    shader.setMat4("model", modelMat);

    // 3) Tüm mesh’leri çiz
    for (auto &mesh : meshes)
        mesh.draw(shader);
}

void Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // Improved error handling
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string error = "Assimp error: ";
        error += importer.GetErrorString();
        std::cerr << error << std::endl;
        throw std::runtime_error(error);
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    std::cout << "Successfully loaded model: " << path << " (" << meshes.size() << " meshes)" << std::endl;
    // yükleme tamam; AABB hesapla
    bbMin = glm::vec3(std::numeric_limits<float>::max());
    bbMax = -bbMin;
    for (const auto &m : meshes)
        for (const auto &v : m.vertices)
        {
            bbMin = glm::min(bbMin, v.Position);
            bbMax = glm::max(bbMax, v.Position);
        }
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // Bu düğüme ait tüm mesh'leri işle
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // Alt düğümleri dolaş
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Vertex verisini oku
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        // Pozisyon
        vertex.Position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z};
        // Normaller
        vertex.Normal = mesh->HasNormals()
                            ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                            : glm::vec3(0.0f);

        // Tekstür koordinatları
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y};
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // İndeks verisini oku
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Materyal ve texture’ları yükle
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // Sadece diffuse ve specular örneği
        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    // Mesh nesnesini oluştur
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string filename = std::string(str.C_Str());
        std::string fullPath = directory + "/" + filename;

        Texture texture;
        glGenTextures(1, &texture.id);
        int width, height, nrComponents;
        unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = (nrComponents == 1 ? GL_RED : nrComponents == 3 ? GL_RGB
                                                                            : GL_RGBA);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Failed to load texture at path: " << fullPath << "\n";
            stbi_image_free(data);
        }

        texture.type = typeName;
        texture.path = fullPath;
        textures.push_back(texture);
    }
    return textures;
}

const std::vector<Mesh> &Model::getMeshes() const
{
    return meshes;
}

glm::mat4 Model::getTransformMatrix() const
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(scale));   // <-- ölçek eklendi
    return model;
}

void Model::autoGround(float desiredHeight)
{
    position.y = desiredHeight - bbMin.y * scale; // tabanı y=desiredHeight’e yasla
}
void Model::setUniformScale(float targetHeight)
{
    float currentH = (bbMax.y - bbMin.y);
    scale = targetHeight / currentH;
}
