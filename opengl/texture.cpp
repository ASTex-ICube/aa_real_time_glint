#define TINYEXR_IMPLEMENTATION

#include "texture.h"
#include "stb/stb_image.h"
#include "glutils.h"
#include "tinyexr.h"
#include <cmath>
#include <array>

const char* err = nullptr;

GLuint Texture::loadMultiscaleMarginalDistributions(const std::string& baseName, const unsigned int nlevels, const GLsizei ndists)
{
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_1D_ARRAY, texID);

	GLint width;
	GLint height;
	GLsizei layerCount = ndists * nlevels;

	// Load the first one to get width/height
	std::string texName = baseName + "_0000" + "_" + "0000" + ".exr";
	float* data;
	bool ret = exrio::LoadEXRRGBA(&data, &width, &height, texName.c_str(), err);
	if (!ret) {
		exit(-1);
	}



	GLsizei mipLevelCount = 1 + (GLsizei)log2f(width);

	// Allocate the storage
	glTexStorage2D(GL_TEXTURE_1D_ARRAY, mipLevelCount, GL_RGB16F, width, layerCount);

	// Upload pixel data
	glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, width, 1, GL_RGBA, GL_FLOAT, data);

	free(data);

	// Load the other 1D distributions
	for (GLsizei l = 0; l < nlevels; ++l) {
		std::string index_level;
		if (l < 10)
			index_level = "000" + std::to_string(l);
		else if (l < 100)
			index_level = "00" + std::to_string(l);
		else if (l < 1000)
			index_level = "0" + std::to_string(l);
		else
			index_level = std::to_string(l);

		for (int i = 0; i < ndists; i++) {

			if (l == 0 && i == 0) continue;

			std::string index_dist;
			if (i < 10)
				index_dist = "000" + std::to_string(i);
			else if (i < 100)
				index_dist = "00" + std::to_string(i);
			else if (i < 1000)
				index_dist = "0" + std::to_string(i);
			else
				index_dist = std::to_string(i);

			texName = baseName + "_" + index_dist + "_" + index_level + ".exr";
			bool ret = exrio::LoadEXRRGBA(&data, &width, &height, texName.c_str(), err);
			if (!ret) {
				exit(-1);
			}
			glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, l * ndists + i, width, 1, GL_RGBA, GL_FLOAT, data);
			free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

	glGenerateMipmap(GL_TEXTURE_1D_ARRAY);

	return texID;
}


GLuint Texture::loadTexture(const std::string& fName, bool generate_mipmap, bool flip, int& width_out, int& height_out) {
	int width, height, bytesPerPix, mipLevelCount;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(fName.c_str(), &width, &height, &bytesPerPix, 0);

	if (generate_mipmap)
		mipLevelCount = (int)std::log2(width) + 1;
	else
		mipLevelCount = 1;

	GLuint tex = 0;
	if (data != nullptr) {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		if (bytesPerPix == 1) {
			glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, GL_R8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
		}
		else if (bytesPerPix == 2) {
			glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, GL_RG8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RG, GL_UNSIGNED_BYTE, data);
		}
		else if (bytesPerPix == 3) {
			glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, GL_RGB8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (bytesPerPix == 4) {
			glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, GL_RGBA8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "Error: number of bytes per pixel different from 1, 3 or 4" << std::endl;
		}

		if (!generate_mipmap) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(data);

	}
	else
		std::cout << "Error: data is not loaded: " << fName << std::endl;


	width_out = width;
	height_out = height;
	return tex;
}


GLuint Texture::loadTexture1D(const std::string& fName, bool generate_mipmap, bool flip) {
	int width, height, bytesPerPix, mipLevelCount;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(fName.c_str(), &width, &height, &bytesPerPix, 0);

	if (generate_mipmap)
		mipLevelCount = (int)std::log2(width) + 1;
	else
		mipLevelCount = 1;

	GLuint tex = 0;
	if (data != nullptr) {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_1D, tex);
		if (bytesPerPix == 1) {
			glTexStorage1D(GL_TEXTURE_1D, mipLevelCount, GL_R8, width);
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, GL_RED, GL_UNSIGNED_BYTE, data);
		}
		else if (bytesPerPix == 3) {
			glTexStorage1D(GL_TEXTURE_1D, mipLevelCount, GL_RGB8, width);
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (bytesPerPix == 4) {
			glTexStorage1D(GL_TEXTURE_1D, mipLevelCount, GL_RGBA8, width);
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "Error: number of bytes per pixel different from 1, 3 or 4" << std::endl;
		}

		if (!generate_mipmap) {
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		}
		else {
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);


			glGenerateMipmap(GL_TEXTURE_1D);
		}

		stbi_image_free(data);
	}
	else
		std::cout << "Error: data is not loaded: " << fName << std::endl;

	return tex;
}

GLuint Texture::loadHdrCubeMap(const std::string& fName, bool generate_mipmap)
{
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	GLint w, h;

	// Load the first one to get width/height
	std::string texName = fName + "_" + suffixes[0] + ".hdr";
	float* data = stbi_loadf(texName.c_str(), &w, &h, NULL, 3);
	if (data == nullptr) {
		std::cerr << "Data is null. Wrong file" << std::endl;
		return 0;
	}

	GLuint mipLevelCount;
	if (generate_mipmap)
		mipLevelCount = (int)std::log2(w) + 1;
	else
		mipLevelCount = 1;

	// Allocate immutable storage for the whole cube map texture
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, mipLevelCount, GL_RGB32F, w, h);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
	stbi_image_free(data);

	// Load the other 5 cube-map faces
	for (int i = 1; i < 6; i++) {
		std::string texName = fName + "_" + suffixes[i] + ".hdr";
		data = stbi_loadf(texName.c_str(), &w, &h, NULL, 3);
		if (data == nullptr) {
			std::cerr << "Data is null. Wrong file" << std::endl;
			return 0;
		}
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (generate_mipmap) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	return texID;
}


bool Texture::generateLeanTextureFromBumpMapFS(const GLuint& input, const int& sizex, const int& sizey, GLuint& output1, GLuint& output2, float scale) {

	int mipLevelCount = (int)std::log2(sizex) + 1;

	// Create texures

	// Output 1
	glGenTextures(1, &output1);
	glBindTexture(GL_TEXTURE_2D, output1);
	glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, GL_RGBA32F, sizex, sizey);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Output2
	glGenTextures(1, &output2);
	glBindTexture(GL_TEXTURE_2D, output2);
	glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, GL_RGBA32F, sizex, sizey);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
	GLenum att[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output1, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, output2, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	GLuint vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(0);

	// Compile shaders
	GLSLProgram vf_shader;
	vf_shader.compileShader((SHADER_PATH + std::string("generateLeanTexture.vert.glsl")).c_str());
	vf_shader.compileShader((SHADER_PATH + std::string("generateLeanTexture.frag.glsl")).c_str());
	vf_shader.link();
	vf_shader.use();
	vf_shader.setUniform("Width", sizex);
	vf_shader.setUniform("Height", sizey);
	vf_shader.setUniform("Scale", scale);
	vf_shader.setUniform("img_input", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input);

	// Generate textures.
	std::array<GLint, 10> data;
	glGetIntegerv(GL_VIEWPORT, data.data());
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &(data[4]));

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
	glViewport(0, 0, sizex, sizey);
	GLenum out_buff[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2,out_buff);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glBindFramebuffer(GL_FRAMEBUFFER, data[4]);
	glViewport(data[0], data[1], data[2], data[3]);
	if (data[4] == 0)
	{
		GLenum db = GL_BACK;
		glDrawBuffers(1u, &db);
	}

	glBindVertexArray(0);
	glDeleteFramebuffers(1, &fb);
	glDeleteVertexArrays(1, &vao);

	glBindTexture(GL_TEXTURE_2D, output1);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, output2);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}