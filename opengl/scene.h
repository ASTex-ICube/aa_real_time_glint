#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb/stb_image_write.h>
#include <tinyexr.h>
#include <string>
#include "openglogl.h"

struct SceneSettings {
    std::string model_path;
    glm::vec3   camera_position;
    float       camera_yaw;
    float       camera_pitch;
    float       scale;
    glm::vec4   point_light_position;
    float       point_light_intensity;
    glm::vec2   directional_light_direction;
    float       directional_light_intensity;
    float       scale_intensity_envmap;
};

class Scene
{
protected:
	glm::mat4 model, view, projection;

public:
    int width;
    int height;
    int samples;

	Scene() : width(800), height(600) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    width = w;
	    height = h;
	}
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      Return true to stop the loop.
      */
    virtual bool update( float t, GLFWwindow* window) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;

    /**
      Utility function to save the current frame as a png file 
    */
    int saveScreen(const std::string& filename) {
        char* data = (char*)malloc((size_t)(width * height * 4));
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_flip_vertically_on_write(1);
        int r = stbi_write_png(filename.c_str(), width, height, 4, data, 0);
        free(data);
        return r;
    }

    int saveScreenToPNG(const std::string& filename) {
        char* data = (char*)malloc((size_t)(width * height * 4));
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_flip_vertically_on_write(1);
        int r = stbi_write_png((filename + ".png").c_str(), width, height, 4, data, 0);
        free(data);
        return r;
    }

    int saveScreenToEXR(const std::string& filename) {
        float* data = (float*)malloc((width * height * 3) * sizeof(float));
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, data);
        const char** err = NULL;
        int r = SaveEXR(data, width, height, 3, 0, (filename + ".exr").c_str(), err);
        free(data);
        return r;
    }

};
