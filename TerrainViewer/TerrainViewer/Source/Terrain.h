#pragma once
#include "RenderObjBase.h"
#include <stdint.h>

class Terrain : public RenderObjBase
{
public:
	Terrain() : RenderObjBase()
	{
		m_position = glm::vec4(0,0,0,1);
		m_scale = 100.0f;
		m_faceResolution = glm::uvec2(1024,1024);
		m_vertResolution = m_faceResolution + glm::uvec2(1,1);

		m_vertices = (glm::vec4*)malloc(sizeof(glm::vec4) * m_vertResolution.y * m_vertResolution.x);
		if(m_vertices == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_vertices, 0, sizeof(glm::vec4) * m_vertResolution.y * m_vertResolution.x);

		m_indices = (GLuint*)malloc(sizeof(GLuint) * IndicesCount());
		if(m_indices == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_indices, 0, sizeof(GLuint) * IndicesCount());

		m_texCoords = (glm::vec2*)malloc(sizeof(glm::vec2) * m_vertResolution.y * m_vertResolution.x);
		if(m_texCoords == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_texCoords, 0, sizeof(glm::vec2) * m_vertResolution.y * m_vertResolution.x);
		Init();
	}

	~Terrain()
	{
		if(m_indices != NULL)
		{
			free(m_indices);
			m_indices = NULL;
		}

		if(m_vertices != NULL)
		{
			free(m_vertices);
			m_vertices = NULL;
		}

		if(m_texCoords != NULL)
		{
			free(m_texCoords);
			m_texCoords = NULL;
		}
	}
	virtual void Render(void);
	virtual glm::mat4 GetModelMat();
	void Setup();
	void Cleanup();

protected:
	virtual void CreateVBO(void);
	virtual void CreateShaders(void);
	virtual void DestroyVBO(void);
	virtual void DestroyShaders(void);
	void BindForRender();
	void UnbindForRender();
	int TriCount();
	int IndicesCount();
private:
	void Init();

	glm::vec4* m_vertices;
	GLuint* m_indices; 
	glm::vec2* m_texCoords;
	glm::uvec2 m_faceResolution;
	glm::uvec2 m_vertResolution;
	glm::vec4 m_position;
	GLuint m_testTex;
	GLuint m_txbo;
	float m_scale;
	GLuint
		ProjectionMatrixUniformLocation,
		ViewMatrixUniformLocation,
		ModelMatrixUniformLocation,
		m_texLocation;
};