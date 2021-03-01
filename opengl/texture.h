#pragma once

#include "openglogl.h"
#include <string>
#include "glslprogram.h"
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Texture {
// Static declarations
public:
    enum class Type
    {
        None,
        Ambiant,
        Diffuse,
        Specular,
        Height,
        Slope,
        SecondMoment,
        Mask
    };

    static GLuint loadMultiscaleMarginalDistributions(const std::string& baseName, const unsigned int nlevels, const GLsizei ndists);

    static GLuint loadTexture(const std::string& fName, bool generate_mipmap, bool flip, int& width_out, int& height_out);
    static GLuint loadTexture1D(const std::string& fName, bool generate_mipmap = true, bool flip = false);

    static GLuint loadHdrCubeMap(const std::string& fName, bool generate_mipmap = true);

    // Vertex/Fragment Shader standard version
    // output1 : texture 2D RG  -> x,y
    // output2 : texture 2D RGB -> x^2,y^2,x*y
    static bool generateLeanTextureFromBumpMapFS(const GLuint& input, const int& sizex, const int& sizey, GLuint& output1, GLuint& output2, float scale = 1.f);

};

class Texture2D {
public:

    Texture2D():
        m_id(0),
        m_width(0),
        m_height(0),
        m_type(Texture::Type::None),
        m_name("")
    {};
    Texture2D(const GLuint& id, const int& width, const int& height, const Texture::Type& type, const std::string& name) :
        m_id(id),
        m_width(width),
        m_height(height),
        m_type(type),
        m_name(name)
    {};
    Texture2D(const std::string& fName, bool generate_mipmap = true, bool flip = false) :
        m_id(0),
        m_width(0),
        m_height(0),
        m_type(Texture::Type::None),
        m_name(fName)
    {
        m_id = Texture::loadTexture(fName, generate_mipmap, flip, m_width, m_height);
    }
    ~Texture2D(){ glDeleteTextures(1, &m_id); }

    
    void Bind(){ glBindTexture(GL_TEXTURE_2D, m_id); }

    int GetId() { return m_id; }
    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }
    void SetType(const Texture::Type& type) { m_type = type; }
    void SetName(const std::string& name) { m_name = name; }
    const Texture::Type& GetType() { return m_type; }
    const std::string& GetName() { return m_name; }

private:

    int             m_width;
    int             m_height;
    GLuint          m_id;
    Texture::Type   m_type;
    std::string     m_name;

};
