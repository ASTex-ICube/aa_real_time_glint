// The MIT License
// Copyright © 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Real-Time Geometric Glint Anti-Aliasing with Normal Map Filtering
// 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [i3D 2021](http://i3dsymposium.github.io/2021/).

#include "scene_obj.h"

#include <time.h>
#include <string>
#include <sstream>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


SceneObj::SceneObj(const SceneSettings& settings) :

	m_model(settings.model_path), // Scene model
	camera(settings.camera_position,
			glm::vec3(0., 1., 0.),
			settings.camera_yaw,
			settings.camera_pitch), // Camera position, up, yaw, pitch
	scale(settings.scale), // Model scale
	
	// Lighting
	light_pos(settings.point_light_position),					// Point light position
	point_light_intensity(settings.point_light_intensity),		// Point light intensity
	dir_light_dir(settings.directional_light_direction),		// Directional light (the direction in spherical coordinates)
	dir_light_intensity(settings.directional_light_intensity),	// Directional light intensity
	envmap_intensity_scale(settings.scale_intensity_envmap),	// Environment map intensity multiplier
	
	override_materials_params(false),		// Override materials parameters
	sigmas_rho(glm::vec3(0.16f,0.16f,0.f)), // 
	log_microfacet_density(15.f),			//
	microfacet_relative_area(0.025f),		//
	max_anisotropy(4.f),					//

	filter(true),					// GGAA filter activation
	kernel_size(0.5f),				// Size of the filtering kernel
	use_hemis_derivatives(true),
	
	super_sampling(false),			// Super sampling activation. Use to produce references.
	tonemapping(true),				// Apply tone mapping to the final image. It is applied during the post-process phase.
	max_intensity(10.),				// Exposure of the final image. It is applied during the post-process phase.
	gamma_correction(true),			// Apply gamma correction to the final image. It is applied during the post-process phase.
	bloom(true),					// Activate bloom.
	only_specular(false),			// True: render only specular radiance.
	lean_mode(0),					// 0 Full lean mapping, 1 lean mapping without rho, 
	use_env_map(true),				// true: use envmap
	use_bump(true),					// true: activate bump mapping

	// Record parameter
	format(true),					// true PNG, false EXR
	show_imgui(true),				// DON'T MODIFY, used to hide imgui when a frame capture is made.

	// CONSTANT
	super_sampling_count(32),		// DON'T MODIFY, Square root number of samples. Use to produce references.
	tPrev(0.0f),					// DON'T MODIFY
	skybox(1000.)					// DON'T MODIFY
{}

void SceneObj::initScene() {

	compileAndLinkShader();
	
	setupSuperSampling();
	setupPostProcessing();
	setupQuad();

	// Load envmap
	envmap_tex = Texture::loadHdrCubeMap(MEDIA_PATH + "../media/textures/cube_map/glacier/glacier");
	
	// Projection is constant for each frame.
	// View and model and defined for each frame.
	projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.01f, 2000.0f);
	

	// Constant uniform values that don't need to be modified during the update phase.
	prog_glints.use();
	prog_glints.setUniform("Resolution", glm::ivec2(width, height));


	// Load multiscale dictionary of marginal distributions
	int numberOfLevels = 16;
	int numberOfDistributionsPerChannel = 64;
	dicoTex = Texture::loadMultiscaleMarginalDistributions(MEDIA_PATH + std::string("../media/dictionary/dict_16_192_64_0p5_0p02"), numberOfLevels, numberOfDistributionsPerChannel);
	
	prog_glints.setUniform("Dictionary.Alpha", 0.5f);
	prog_glints.setUniform("Dictionary.N", numberOfDistributionsPerChannel * 3);
	prog_glints.setUniform("Dictionary.NLevels", numberOfLevels);
	prog_glints.setUniform("Dictionary.Pyramid0Size", 1 << (numberOfLevels - 1));

}

bool SceneObj::update(float t, GLFWwindow* window) {
	
	if (show_imgui){
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Parameters");

			if (ImGui::BeginTabBar("Menu")) {
	
				if (ImGui::BeginTabItem("Parameters and output")) {
					
					ImGui::Checkbox("GGAA", &filter);
					ImGui::Checkbox("Use hemispherical derivatives", &use_hemis_derivatives);
					ImGui::SliderFloat("Kernel size", &kernel_size, 0.f, 2.0f);

					ImGui::Separator();

					ImGui::DragFloat("Max intensity", &max_intensity, 0.05f, 0.f, 350.f, "%.3f");

					ImGui::Checkbox("Gamma correction", &gamma_correction);
					ImGui::SameLine();
					ImGui::Checkbox("Tonemapping", &tonemapping);
					
					ImGui::Checkbox("Bloom", &bloom);
					ImGui::SameLine();
					ImGui::Checkbox("Only specular", &only_specular);

					ImGui::Separator();

					ImGui::Checkbox("1,024 spp (only for saved frames)", &super_sampling);

					ImGui::Separator();

					ImGui::Text("Frames are saved in the current directory.");

					if (ImGui::Button("Save frame")) {
						show_imgui = false;
					}
					ImGui::SameLine();
					if (format) {
						if (ImGui::Button("PNG"))
							format = !format;
					}
					else
						if (ImGui::Button("EXR"))
							format = !format;

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Normal map filtering")) {

					ImGui::Checkbox("Use normal mapping", &use_bump);
					ImGui::Separator();

					ImGui::RadioButton("Full LEAN mapping", &lean_mode, 0);
					ImGui::RadioButton("Without rho", &lean_mode, 1);
					ImGui::RadioButton("Without LEAN mapping", &lean_mode, 2);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Material parameters")) {

					ImGui::Checkbox("Override materials values", &override_materials_params);
					if (override_materials_params) {
						ImGui::Separator();
						ImGui::Columns(2);
						ImGui::SliderFloat("Sigma X", &sigmas_rho.x, 0.01f, 1.0f);
						ImGui::NextColumn();
						ImGui::SliderFloat("Sigma Y", &sigmas_rho.y, 0.01f, 1.0f);
						ImGui::Columns(1);
						ImGui::SliderFloat("Slope correlation factor", &sigmas_rho.z, -0.99f, 0.99f);
						ImGui::SliderFloat("Log microfacet density", &log_microfacet_density, 0.1f, 50.0f);
						ImGui::SliderFloat("Microfacet relative area", &microfacet_relative_area, 0.01f, 1.0f);
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Misc")) {

					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					ImGui::Text("Camera position, x: %.3f, y: %.3f, z: %.3f", camera.Position.x, camera.Position.y, camera.Position.z);
					ImGui::Text("Camera pitch: %.3f, yaw: %.3f", camera.Pitch, camera.Yaw);
					
					ImGui::Separator();
					
					ImGui::Text("Camera movement : W,A,S,D");
					ImGui::Text("Camera rotation : Arrow Up,Left,Down,Right");
					
					ImGui::Separator();

					ImGui::Text("Lighting");
					if(ImGui::TreeNode("Point light")){
						ImGui::Columns(3);
						ImGui::DragFloat("Pos x", &light_pos.x, 1.f, -100.f, 100.f, "%.1f");
						ImGui::NextColumn();
						ImGui::DragFloat("Pos y", &light_pos.y, 1.f, -100.f, 100.f, "%.1f");
						ImGui::NextColumn();
						ImGui::DragFloat("Pos z", &light_pos.z, 1.f, -100.f, 100.f, "%.1f");

						ImGui::Columns(1);
						ImGui::DragFloat("Intensity", &point_light_intensity, 1.f, 0.f, 1000.f, "%.1f");
						ImGui::TreePop();
					}
					
					if (ImGui::TreeNode("Directional light")) {
						ImGui::Columns(2);
						ImGui::DragFloat("Theta", &dir_light_dir.x, 0.05f, 0.f, 1.56f, "%.2f");
						ImGui::NextColumn();
						ImGui::DragFloat("Phi", &dir_light_dir.y, 0.05f, 0.f, 6.28f, "%.2f");

						ImGui::Columns(1);
						ImGui::DragFloat("Intensity", &dir_light_intensity, 0.01f, 0.f, 10.f, "%.1f");
						ImGui::TreePop();

					}

					if (ImGui::TreeNode("Env map")) {
						ImGui::DragFloat("Scale intensity envmap", &envmap_intensity_scale, 0.01f, 0.f, 1.f, "%.2f");
						ImGui::Checkbox("Use envmap", &use_env_map);
						ImGui::TreePop();
					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}

	float deltaT = t - tPrev;
	if (tPrev == 0.0f) deltaT = 0.0f;
	tPrev = t;

	
	// Camera update
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaT*0.5);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaT * 0.5);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaT * 0.5);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaT * 0.5);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessMouseMovement(-5.f, 0.f, deltaT);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessMouseMovement(5.f, 0.f, deltaT);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessMouseMovement(0.f, 5.f, deltaT);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessMouseMovement(0.f, -5.f, deltaT);
	

	view = camera.GetViewMatrix();

	prog_glints.use();
	prog_glints.setUniform("CameraPosition", camera.Position);

	prog_glints.setUniform("DictionaryTex", 0);  //layout binding not supported on 4.1 mac
	prog_glints.setUniform("DiffuseTex", 1);
	prog_glints.setUniform("SlopeTex", 2);
	prog_glints.setUniform("SecondMomentTex", 3);

	prog_glints.setUniform("LeanMode", lean_mode);

	prog_glints.setUniform("UserSigmasRho", sigmas_rho);
	prog_glints.setUniform("UserLogMicrofacetDensity", log_microfacet_density);
	prog_glints.setUniform("UserMicrofacetRelativeArea", microfacet_relative_area);

	prog_glints.setUniform("Filter", filter && !super_sampling);
	prog_glints.setUniform("UseHemisDerivatives", use_hemis_derivatives);
	prog_glints.setUniform("KernelSize", kernel_size);

	prog_glints.setUniform("PointLight.L", glm::vec3(point_light_intensity));
	prog_glints.setUniform("PointLight.Position", light_pos);
	prog_glints.setUniform("DirLight.L", glm::vec3(dir_light_intensity));
	prog_glints.setUniform("DirLight.ThetaPhi", dir_light_dir);
	prog_glints.setUniform("ScaleIntensityEnvMap", envmap_intensity_scale);

	prog_glints.setUniform("ComputeReference", super_sampling);

	prog_glints.setUniform("UseEnvMap", use_env_map);
	prog_glints.setUniform("UseBump", use_bump);
	prog_glints.setUniform("OnlySpecular", only_specular);
	prog_glints.setUniform("OverrideMaterials", override_materials_params);

	prog_glints.setUniform("DictionaryTex",0);
	prog_glints.setUniform("DiffuseTex",1);
	prog_glints.setUniform("SlopeTex",2);
	prog_glints.setUniform("SecondMomentTex",3);
	prog_glints.setUniform("SpecularTex",4);
	prog_glints.setUniform("MaskTex",5);
	prog_glints.setUniform("EnvMap",6);

	prog_post_processing.use();
	prog_post_processing.setUniform("MaxIntensity", max_intensity);
	prog_post_processing.setUniform("Tonemapping", tonemapping);
	prog_post_processing.setUniform("GammaCorrection", gamma_correction);
	prog_post_processing.setUniform("Bloom", bloom);

	return false;
}

void SceneObj::render()
{
	// Rendering

	// Clear the framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw calls
	drawScene();

	// draw imgui if the frame is not saved.
	if (show_imgui){
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else { // save the frame, generate PNG or EXR file from the fboPostProssing framebuffer (float values) 
		show_imgui = true;

		auto getFileName = [&]() {
			time_t t = time(NULL);
			tm* tt = localtime(&t);
			std::stringstream ss_file_name;
			ss_file_name << "./glints_";
			if (filter)
				ss_file_name << "filter-ks-" << kernel_size << "_";
			if (super_sampling)
				ss_file_name << "ss-" << super_sampling_count << "_";
			else
				ss_file_name << tt->tm_year << "-" << tt->tm_mon << "-" << tt->tm_mday << "_" << tt->tm_hour << "-" << tt->tm_min << "-" << tt->tm_sec << "_";
			return ss_file_name.str();
		};

		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_post_processing);
		if(format)
			saveScreenToPNG(getFileName());
		else
			saveScreenToEXR(getFileName());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void SceneObj::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	prog_glints.use();
	prog_glints.setUniform("Resolution", glm::ivec2(width, height));
	prog_quad_fullscreen.use();
	prog_quad_fullscreen.setUniform("Resolution", glm::ivec2(width, height));
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.01f, 2000.0f);
}

void SceneObj::compileAndLinkShader() {
	try {
		prog_skybox.compileShader((SHADER_PATH + std::string("skybox.vert.glsl")).c_str());
		prog_skybox.compileShader((SHADER_PATH + std::string("skybox.frag.glsl")).c_str());
		prog_skybox.link();

		prog_glints.compileShader((SHADER_PATH + std::string("improved_glint_envmap.vert.glsl")).c_str());
		prog_glints.compileShader((SHADER_PATH + std::string("improved_glint_envmap.frag.glsl")).c_str());
		prog_glints.link();
	
		prog_quad_fullscreen.compileShader((SHADER_PATH + std::string("render_texture.vert.glsl")).c_str());
		prog_quad_fullscreen.compileShader((SHADER_PATH + std::string("render_texture.frag.glsl")).c_str());
		prog_quad_fullscreen.link();

		prog_post_processing.compileShader((SHADER_PATH + std::string("postprocessing.vert.glsl")).c_str());
		prog_post_processing.compileShader((SHADER_PATH + std::string("postprocessing.frag.glsl")).c_str());
		prog_post_processing.link();

	}
	catch (GLSLProgramException& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void SceneObj::drawScene() {


	// Clear the fbo_super_sampling frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_super_sampling);
	glClearColor(0., 0., 0., 1.);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Sponza position and scale
	glm::vec3 pos(0., 0., 0.);
	
	// Activate super samping only if the current frame will be saved
	int AA = (super_sampling && !show_imgui)  ? super_sampling_count : 1;
	int AAAA = AA * AA;



	for (int i = 0; i < AAAA; i++) {
	
		///////////////////////////
		// Render the scene in a frame buffer
		//  - generate tex_sample
		////////////////////////


		glEnable(GL_DEPTH_TEST);

		// Render in fbo_sample framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_sample);
		
		// Clear the fbo_sample framebuffer between each sample.
		glClearColor(0., 0., 0., 1.);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		///////////////////////////
		// Move the camera for each sample

		float offset_x = (float(i % AA) / float(AA)) * 2.f - 1.f;
		float offset_y = (float(i / AA) / float(AA)) * 2.f - 1.f;
		offset_x /= float(width);
		offset_y /= float(height);
		offset_x += (1.f / float(AA)) / (float(width));
		offset_y += (1.f / float(AA)) / (float(height));

		// Set matrices
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
		model = glm::translate(model, pos);
	
		// Define the view model matrix with an offset defined previously
		glm::vec3 shift(offset_x, offset_y, 0.);
		glm::mat4 aa = glm::translate(glm::mat4(1.f), shift);
		glm::mat4 mv = view * model;


		///////////////////////////
		// Draw sky

		if(use_env_map){
			glDisable(GL_DEPTH_TEST);

			prog_skybox.use();
			prog_skybox.setUniform("VP", projection * glm::mat4(glm::mat3(view)));
			prog_skybox.setUniform("SkyBoxTex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envmap_tex);
			skybox.draw();

			glEnable(GL_DEPTH_TEST);
		}

		///////////////////////////
		// Draw scene
		
		prog_glints.use();
		prog_glints.setUniform("ModelMatrix", model);
		prog_glints.setUniform("MVP", aa * projection * mv);
			
		prog_glints.setUniform("MaxAnisotropy", max_anisotropy);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D_ARRAY, dicoTex);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envmap_tex);

		m_model.Draw(prog_glints);

		///////////////////////////
		// Render tex_sample on the next framebuffer with an alpha of 1 / AAAA
		//  - generate tex_super_sampling
		/////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, fbo_super_sampling);
		glDisable(GL_DEPTH_TEST);

		prog_quad_fullscreen.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_sample);
		prog_quad_fullscreen.setUniform("Tex", 0);

		// Set the blending function
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
		// Set the alpha to 1 / AAAA
		glBlendColor(0., 0., 0., 1.f / float(AAAA));


		glEnable(GL_BLEND);
		// Draw tex_sample on the quad
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisable(GL_BLEND);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	}

	///////////////////////////
	// Post processing
	// - bloom
	// - tonemapping
	/////////////////////// 

	// Render tex_sample in the framebuffer fbo_post_processing
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_post_processing);
	glDisable(GL_DEPTH_TEST);

	
	// Draw tex_super_sampling on the quad
	prog_post_processing.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_super_sampling);
	prog_post_processing.setUniform("Tex", 0);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	///////////////////////////
	// Display fbo_post_processing on screen
	/////////////////////////
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_post_processing);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

}



void SceneObj::setupSuperSampling() {

	// Supersampling
	glGenTextures(1, &tex_super_sampling);
	
	glBindTexture(GL_TEXTURE_2D, tex_super_sampling);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo_super_sampling);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_super_sampling);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_super_sampling, 0);

	glGenRenderbuffers(1, &rb_super_sampling_depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_super_sampling_depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb_super_sampling_depth_buffer); // now actually attach it

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/// Single sample (with a normal texture color attachment)

	glGenTextures(1, &tex_sample);
	glBindTexture(GL_TEXTURE_2D, tex_sample);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo_sample);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_sample);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_sample, 0);

	glGenRenderbuffers(1, &rb_sample_depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_sample_depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb_sample_depth_buffer); // now actually attach it


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Shader setup
	prog_quad_fullscreen.use();
	prog_quad_fullscreen.setUniform("Resolution", glm::ivec2(width, height));
}

void SceneObj::setupPostProcessing(){
	glGenRenderbuffers(1, &rb_post_processing);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_post_processing);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, width, height);
	glGenFramebuffers(1, &fbo_post_processing);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_post_processing);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fbo_post_processing);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	// Post processing shader setup
	prog_post_processing.use();
	prog_post_processing.setUniform("Resolution", glm::ivec2(width, height));
}



void SceneObj::setupQuad() {

	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(0);

}

SceneObj::~SceneObj() {
	glDeleteTextures(1, &envmap_tex);
}

