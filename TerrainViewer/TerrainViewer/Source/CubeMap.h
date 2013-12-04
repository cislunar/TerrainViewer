#pragma once

#include "Render.h"
#include "Math.h"

#define CUBE_MAP_LEFT	"Source\\Sunny2_left.tif"
#define CUBE_MAP_RIGHT	"Source\\Sunny2_right.tif"
#define CUBE_MAP_UP		"Source\\Sunny2_up.tif"
#define CUBE_MAP_DOWN	"Source\\Sunny2_down.tif"
#define CUBE_MAP_FRONT	"Source\\Sunny2_front.tif"
#define CUBE_MAP_BACK	"Source\\Sunny2_back.tif"

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