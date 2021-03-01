// https://learnopengl.com/Model-Loading/Mesh

#pragma once

#include "openglogl.h"

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <memory>

#include "glslprogram.h"
#include "texture.h"
#include "texturepool.h"

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
};

class Mesh {
public:
    // texture data;
    TexturePool* texturePool;
    
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    std::vector<int>    diffuseTextures;
    std::vector<int>    heightTextures;
    std::vector<int>    slopeTextures;
    std::vector<int>    secondMomentTextures;
    std::vector<int>    specularTextures;
    std::vector<int>    maskTextures;

    glm::vec2 scaleUV;
    float logMicrofacetDensity;
    float microfacetRelativeArea;

    glm::vec3 Kd, Ks;
    float Ns;

    unsigned int VAO;
    std::string name;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, TexturePool* texturePool,
         std::vector<int> diffuseTextures,
         std::vector<int> heightTextures,
         std::vector<int> slopeTextures,
         std::vector<int> secondMomentTextures,
         std::vector<int> specularTextures,
         std::vector<int> maskTextures,
         glm::vec2 scaleUV,
         float logMicrofacetDensity,
         float microfacetRelativeArea,
         glm::vec3 Kd,
         glm::vec3 Ks,
         float Ns,
         const std::string& name);

    void Draw(GLSLProgram& shader);
private:
    //  render data
    unsigned int VBO, EBO;

    void setupMesh();
};
