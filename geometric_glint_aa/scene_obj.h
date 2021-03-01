// The MIT License
// Copyright © 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Real-Time Geometric Glint Anti-Aliasing with Normal Map Filtering
// 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [i3D 2021](http://i3dsymposium.github.io/2021/).

#pragma once

#include "scene.h"
#include "glslprogram.h"
#include "openglogl.h"

#include "model.h"
#include "camera.h"
#include "texture.h"
#include "box.h"

#include <utility>

#include <glm/glm.hpp>

class SceneObj : public Scene {
private:
    void drawScene();
    void compileAndLinkShader();

    // Dictionary of marginal distributions
    GLuint dicoTex;

    // Shaders
    GLSLProgram prog_glints;
    GLSLProgram prog_quad_fullscreen;
    GLSLProgram prog_post_processing;
    GLSLProgram prog_skybox;

    // Envmap
    Box     skybox;
    GLuint  envmap_tex;
    bool    use_env_map;
    float   envmap_intensity_scale;

    // Lighting
    glm::vec4   light_pos;
    float       point_light_intensity;
    glm::vec2   dir_light_dir;
    float       dir_light_intensity;

    // Scene
    Camera      camera;
    Model       m_model;
    glm::vec3   scale;

    // Override materials parameters
    bool        override_materials_params;
    glm::vec3   sigmas_rho;
    float       log_microfacet_density;
    float       microfacet_relative_area;
    float       max_anisotropy;
    
    // GGAA
    bool    filter;
    float   kernel_size;
    bool    use_hemis_derivatives;
    
    // Post-processing
    float   max_intensity;
    bool    tonemapping;
    bool    gamma_correction;
    bool    bloom;

    
    // Record
    bool        format;
    bool        super_sampling;
    int         super_sampling_count;
    bool        show_imgui;

    // Options
    bool    only_specular;
    bool    use_bump;
    int     lean_mode;


	float   tPrev;

    // Super sampling utilities
    void    setupSuperSampling();
    GLuint  tex_super_sampling; // color buffer RGBA32F
    GLuint  rb_super_sampling_depth_buffer; // depth buffer
    GLuint  fbo_super_sampling;

    // Single sample
    GLuint  fbo_sample;
    GLuint  tex_sample; // color buffer for a unique sample
    GLuint  rb_sample_depth_buffer; // depth buffer


    // Post-processing
    void    setupPostProcessing();
    GLuint  fbo_post_processing;
    GLuint  rb_post_processing;

    // Quad
    void    setupQuad();
    GLuint  quad_vao; // used to draw a fullscreen quad


    
public:
    SceneObj(const SceneSettings& settings);
    ~SceneObj();

    void initScene();
    bool update( float t, GLFWwindow* window);
    void render();
    void resize(int, int);
};
