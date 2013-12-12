#pragma once

#include "Render.h"
#include "Math.h"

#define CUBE_MAP_LEFT	"Assets\\Sunny2_left.jpg"
#define CUBE_MAP_RIGHT	"Assets\\Sunny2_right.jpg"
#define CUBE_MAP_UP		"Assets\\Sunny2_up.jpg"
#define CUBE_MAP_DOWN	"Assets\\Sunny2_down.jpg"
#define CUBE_MAP_FRONT	"Assets\\Sunny2_front.jpg"
#define CUBE_MAP_BACK	"Assets\\Sunny2_back.jpg"

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