#pragma once

#include <vector>
#include <stdio.h>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

// Texture struct
struct Texture
{
    unsigned int id;
    std::string type;
};

class Model
{
public:
    // Model attributes
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    // Define light source properties
    glm::vec3 lightPosition = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
    std::vector<Texture>   textures;
    unsigned int textureID;
    float ka, kd, ks, Ns;
    float uvScale = 1.0f;
    // Constructor
    Model(const char* path);

    // Draw model
    void draw(unsigned int& shaderID);

    // Add textures
    void addTexture(const char* path, const std::string type);

    // Cleanup
    void deleteBuffers();
    // Calculate tangents and bitangents
    void calculateTangents();
private:

    // Array buffers
    unsigned int VAO;
    unsigned int vertexBuffer;
    unsigned int uvBuffer;
    unsigned int normalBuffer;
    unsigned int tangentBuffer;
    unsigned int bitangentBuffer;
    // Load .obj file method
    bool loadObj(const char* path,
        std::vector<glm::vec3>& inVertices,
        std::vector<glm::vec2>& inUVs,
        std::vector<glm::vec3>& inNormals);

    // Setup buffers
    void setupBuffers();

    // Load texture
    unsigned int loadTexture(const char* path);
};
