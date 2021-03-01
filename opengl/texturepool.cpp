#include "texturepool.h"

TexturePool::TexturePool() {
    std::string mpath = MEDIA_PATH + std::string("textures/");
    Push(Texture::Type::Diffuse, "default_diffuse.png", mpath);
    Push(Texture::Type::Height, "default_height.png", mpath);
    Push(Texture::Type::Specular, "default_specular.png", mpath);
    Push(Texture::Type::Mask, "default_mask.png", mpath);
};

int TexturePool::Push(Texture::Type type, std::string name, std::string path, const float& bump_factor) {
    
    std::vector<Texture2D*>* pool = nullptr;
    if (type == Texture::Type::Diffuse)
        pool = &m_diffuse;
    else if (type == Texture::Type::Height)
        pool = &m_height;
    else if (type == Texture::Type::Specular)
        pool = &m_specular;
    else if (type == Texture::Type::Mask)
        pool = &m_mask;

    bool skip = false;
    int i;

    for (int j = 0; j < pool->size(); j++) {
        if ((*pool)[j]->GetName() == name) {
            skip = true;
            i = j;
            break;
        }
    }

    if (!skip)
    {   // if texture hasn't been loaded already, load it

        pool->push_back(new Texture2D(path + name));

        (*pool)[(*pool).size() - 1]->SetType(type);
        (*pool)[(*pool).size() - 1]->SetName(name);
        i = (*pool).size() - 1; // add to loaded textures

        // Add slope and second moment to the texture pool
        if (type == Texture::Type::Height) {
            GLuint slopeTex = 0;
            GLuint secondMomentTex = 0;

            Texture::generateLeanTextureFromBumpMapFS(
                (*pool)[(*pool).size() - 1]->GetId(),
                (*pool)[(*pool).size() - 1]->GetWidth(),
                (*pool)[(*pool).size() - 1]->GetHeight(),
                slopeTex,
                secondMomentTex,
                bump_factor);

            m_secondMoment.push_back(new Texture2D(
                secondMomentTex,
                (*pool)[(*pool).size() - 1]->GetWidth(),
                (*pool)[(*pool).size() - 1]->GetHeight(),
                Texture::Type::Slope, "none"));

            m_slope.push_back(new Texture2D(
                slopeTex, 
                (*pool)[(*pool).size() - 1]->GetWidth(),
                (*pool)[(*pool).size() - 1]->GetHeight(),
                Texture::Type::SecondMoment, "none"));

        }
    }

    return i;
}