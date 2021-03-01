// The MIT License
// Copyright © 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Real-Time Geometric Glint Anti-Aliasing with Normal Map Filtering
// 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [i3D 2021](http://i3dsymposium.github.io/2021/) and for CFG special issue.

#include "scene.h"
#include "scenerunner.h"
#include "scene_obj.h"

std::map<std::string, std::string> sceneInfo = {
	{ "Ogre"		, "Extra"},
	{ "Arctic"		, "Figure 1"},
	{ "Sponza"		, "Figure 10"},
	{ "Tubes"		, "Figure 7"},
	{ "Sphere"		, "Figure 8 and Figure 9"}
};


int main(int argc, char *argv[])
{
	if (argc != 2)
	{ 
		std::cout << "Please launch with scene param: Sphere | Tubes | Arctic | Sponza | Ogre" << std::endl << "Launching default: Arctic." << std::endl;
	}
	std::string scene_name = (argc != 2) ? "Arctic" : SceneRunner::parseCLArgs(argc, argv, sceneInfo);

	SceneRunner runner("Real Time Glint - " + scene_name, 1600, 900);

	std::unique_ptr<Scene> scene;
	SceneSettings settings;
	if (scene_name == "Arctic") {
		settings.model_path = MEDIA_PATH + std::string("snow/snowman_applied_modifiers.obj");
		settings.camera_position = glm::vec3(-17.f, 1.751f, -1.408f);
		settings.camera_yaw = -277.589f;
		settings.camera_pitch = -16.981f;
		settings.scale = 1.f;
		settings.point_light_position = glm::vec4(200.f, 100.f, 80.f, 1.f);
		settings.point_light_intensity = 0.f;
		settings.directional_light_direction = glm::vec2(1.16f, 1.45f);
		settings.directional_light_intensity = 1.f;
		settings.scale_intensity_envmap = 1.f;
		scene = std::unique_ptr<Scene>(new SceneObj(settings));
	}
	if (scene_name == "Sponza") {
		settings.model_path = MEDIA_PATH + std::string("sponza/sponza.obj");
		settings.camera_position = glm::vec3(-23.66f, 3.371f, -0.682f);
		settings.camera_yaw = -179.591f;
		settings.camera_pitch = 1.255f;
		settings.scale = 0.02f;
		settings.point_light_position = glm::vec4(0.f, 5.f, 0.f, 1.f);
		settings.point_light_intensity = 2000.f;
		settings.directional_light_direction = glm::vec2(0.f, 0.f);
		settings.directional_light_intensity = 0.f;
		settings.scale_intensity_envmap = 0.f;
		scene = std::unique_ptr<Scene>(new SceneObj(settings));
	}
	if (scene_name == "Tubes") {
		settings.model_path = MEDIA_PATH + std::string("silver-snowflake-ornament/snowflake.obj");
		settings.camera_position = glm::vec3(0.985f, 1.638f, 0.194f);
		settings.camera_yaw = -162.f;
		settings.camera_pitch = -57.753f;
		settings.scale = 1.f;
		settings.point_light_position = glm::vec4(200.f, 100.f, 80.f, 1.f);
		settings.point_light_intensity = 0.f;
		settings.directional_light_direction = glm::vec2(1.16f, 1.45f);
		settings.directional_light_intensity = 1.f;
		settings.scale_intensity_envmap = 1.f;
		scene = std::unique_ptr<Scene>(new SceneObj(settings));
	}
	if (scene_name == "Sphere") {
		settings.model_path = MEDIA_PATH + std::string("sphere/sphere.obj");
		settings.camera_position = glm::vec3(2.104f, 0.f, 0.f);
		settings.camera_yaw = 180.f;
		settings.camera_pitch = 0.f;
		settings.scale = 1.f;
		settings.point_light_position = glm::vec4(3.f, 0.f, 0.f, 1.f);
		settings.point_light_intensity = 10.f;
		settings.directional_light_direction = glm::vec2(1.16f, 1.45f);
		settings.directional_light_intensity = 0.f;
		settings.scale_intensity_envmap = 0.4f;
		scene = std::unique_ptr<Scene>(new SceneObj(settings));
	}
	if (scene_name == "Ogre") {
		settings.model_path = MEDIA_PATH + std::string("ogre/bs_angry.obj");
		settings.camera_position = glm::vec3(0.f, 0.f, 5.f);
		settings.camera_yaw = -90.f;
		settings.camera_pitch = 0.f;
		settings.scale = 3.f;
		settings.point_light_position = glm::vec4(200.f, 100.f, 80.f, 1.f);
		settings.point_light_intensity = 0.f;
		settings.directional_light_direction = glm::vec2(1.16f, 1.45f);
		settings.directional_light_intensity = 1.f;
		settings.scale_intensity_envmap = 0.1f;
		scene = std::unique_ptr<Scene>(new SceneObj(settings));
	}


	return runner.run(std::move(scene));
}
