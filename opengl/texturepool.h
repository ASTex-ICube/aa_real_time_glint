#pragma once

#include "openglogl.h"
#include <string>
#include "glslprogram.h"
#include <iostream>
#include <vector>

#include "texture.h"

class TexturePool {

private:
    std::vector<Texture2D*> m_diffuse;
    std::vector<Texture2D*> m_height;
    std::vector<Texture2D*> m_slope;
    std::vector<Texture2D*> m_secondMoment;
    std::vector<Texture2D*> m_specular;
    std::vector<Texture2D*> m_mask;

public:
    TexturePool();

    Texture2D* GetDiffuse(int i) { return m_diffuse[i]; }
    Texture2D* GetHeight(int i) { return m_height[i]; }
    Texture2D* GetSlope(int i) { return m_slope[i]; }
    Texture2D* GetSecondMoment(int i) { return m_secondMoment[i]; }
    Texture2D* GetSpecular(int i) { return m_specular[i]; }
    Texture2D* GetMask(int i) { return m_mask[i]; }

    int Push(Texture::Type type, std::string name, std::string path, const float& bump_factor = 1.f);

};