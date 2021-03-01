// The MIT License
// Copyright © 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Real-Time Geometric Glint Anti-Aliasing with Normal Map Filtering
// 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [i3D 2021](http://i3dsymposium.github.io/2021/) and for CFG special issue.

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