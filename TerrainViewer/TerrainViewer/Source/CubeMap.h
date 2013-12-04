#pragma once

#include "Render.h"
#include "Math.h"

#define CUBE_MAP_LEFT	"Source\\Sunny2_left.jpg"
#define CUBE_MAP_RIGHT	"Source\\Sunny2_right.jpg"
#define CUBE_MAP_UP		"Source\\Sunny2_up.jpg"
#define CUBE_MAP_DOWN	"Source\\Sunny2_down.jpg"
#define CUBE_MAP_FRONT	"Source\\Sunny2_front.jpg"
#define CUBE_MAP_BACK	"Source\\Sunny2_back.jpg"

class CubeMap
{
public:
	CubeMap(){}
	void Setup();
	void Cleanup();
	void Render();
private:
	void CreateSkyBoxShader();
	void DestroySkyBoxShader();
	void CreateSkyboxVBO();
	void DestroySkyboxVBO();
	void LoadCubeMap();
	void Bind();
	void UnBind();
	GLuint
		m_cubeMap,
		m_cubeMapViewMatLoc,
		m_cubeMapProjMatLoc;
	ShaderInfo m_skyboxShader;
};