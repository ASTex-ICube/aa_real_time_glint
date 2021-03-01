// https://learnopengl.com/Model-Loading/Mesh

#include "mesh.h"

using std::string;
using glm::vec3;
using glm::vec2;

#include <cstdlib>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
#include <map>

Mesh::Mesh( std::vector<Vertex> vertices,
            std::vector<unsigned int> indices,
            TexturePool* texturePool,
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
            const std::string& name)
{ 
    this->vertices = vertices;
    this->indices = indices;
    
    this->diffuseTextures = diffuseTextures;
    this->heightTextures = heightTextures;
    this->slopeTextures = slopeTextures;
    this->secondMomentTextures = secondMomentTextures;
    this->specularTextures = specularTextures;
    this->maskTextures = maskTextures;
    
    this->scaleUV = scaleUV;
    this->logMicrofacetDensity = logMicrofacetDensity;
    this->microfacetRelativeArea = microfacetRelativeArea;

    this->Kd = Kd;
    this->Ks = Ks;
    this->Ns = Ns;
    this->name = name;
    this->texturePool = texturePool;


    setupMesh();
}

void Mesh::Draw(GLSLProgram& shader)
{

    // Bind diffuse texture
    glActiveTexture(GL_TEXTURE1);
    if (diffuseTextures.size()) {
        shader.setUniform("UseDiffuseTex", true);
        texturePool->GetDiffuse(diffuseTextures[0])->Bind();
    }
    else { 
        // Default texture
        texturePool->GetDiffuse(0)->Bind();

        // Kd coefficient
        shader.setUniform("UseDiffuseTex",false);
        shader.setUniform("Kd", Kd);
    }

    // Bind slope texture
    glActiveTexture(GL_TEXTURE2);
    if (slopeTextures.size()) {
        texturePool->GetSlope(slopeTextures[0])->Bind();
    }
    else { // Default texture
        texturePool->GetSlope(0)->Bind();
    }

    // Bind second moment texture
    glActiveTexture(GL_TEXTURE3);
    if (secondMomentTextures.size()) {
        texturePool->GetSecondMoment(secondMomentTextures[0])->Bind();
    }
    else {
        texturePool->GetSecondMoment(0)->Bind();
    }

    glActiveTexture(GL_TEXTURE4);
    if (specularTextures.size()) {
        shader.setUniform("UseSpecularTex", true);
        texturePool->GetSpecular(specularTextures[0])->Bind();
    }
    else {
        // Default texture
        texturePool->GetSpecular(0)->Bind();

        // Ks coefficient
        shader.setUniform("UseSpecularTex", false);
        shader.setUniform("Ks", Ks);
    }

    glActiveTexture(GL_TEXTURE5);
    if (maskTextures.size()) {
        texturePool->GetMask(maskTextures[0])->Bind();
    }
    else {
        texturePool->GetMask(0)->Bind();
    }

    // alpha = sqrt(2) / sqrt(Ns + 2.)
    float alpha = 1.41421356f / sqrt(Ns + 2.f);
    shader.setUniform("Material.Alpha", alpha);

    shader.setUniform("Material.LogMicrofacetDensity", logMicrofacetDensity);
    shader.setUniform("Material.MicrofacetRelativeArea", microfacetRelativeArea);
    
    shader.setUniform("ScaleUV", scaleUV);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glBindVertexArray(0);
}

