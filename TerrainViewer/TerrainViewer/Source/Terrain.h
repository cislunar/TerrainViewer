#pragma once
#include "RenderObjBase.h"
#include <stdint.h>
#include "TextureData.h"

class Terrain : public RenderObjBase
{
public:
	Terrain() : RenderObjBase()
	{
		m_renderNormals		= false;
		m_position			= glm::vec4(0,0,0,1);
		m_modelScale		= glm::vec3(20, 2, 20);
		m_scaler			=  glm::vec2( 15000, 15000.f);
		m_faceResolution	= glm::uvec2(256,256);
		m_vertResolution	= m_faceResolution + glm::uvec2(1,1);

		m_vertices = (glm::vec4*)malloc(sizeof(glm::vec4) * VerticesCnt() );
		if(m_vertices == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_vertices, 0, sizeof(glm::vec4) * VerticesCnt() );

		m_indices = (GLuint*)malloc(sizeof(GLuint) * IndicesCount());
		if(m_indices == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_indices, 0, sizeof(GLuint) * IndicesCount());

		m_texCoords = (glm::vec2*)malloc(sizeof(glm::vec2) * VerticesCnt() );
		if(m_texCoords == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_texCoords, 0, sizeof(glm::vec2) * VerticesCnt() );

		m_vertNormals = (glm::vec3*)malloc(sizeof(glm::vec3) * VerticesCnt() );
		if(m_vertNormals == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_vertNormals, 0, sizeof(glm::vec3) * VerticesCnt() );

		m_heightData = (float*)malloc(sizeof(float) * VerticesCnt() );
		if(m_heightData == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_heightData, 0, sizeof(float) * VerticesCnt() );

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

		if(m_vertNormals != NULL)
		{
			free(m_vertNormals);
			m_vertNormals = NULL;
		}

		if(m_heightData != NULL)
		{
			free(m_heightData);
			m_heightData = NULL;
		}
	}

	virtual void		Render(void);
	virtual glm::mat4	GetModelMat();
	void				Setup();
	void				Cleanup();
	glm::vec2*			GetScale() {return &m_scaler;}
	void				Update( float _dt );

protected:
	virtual void		CreateVBO(void);
	virtual void		CreateShaders(void);
	virtual void		DestroyVBO(void);
	virtual void		DestroyShaders(void);
	void				BindTerrainForRender();
	void				BindNormalsForRender();
	void				UnbindForRender();
	int					TriCount();
	int					VerticesCnt();
	int					IndicesCount();
	glm::vec4			GetTexel( TextureData* _texData,  int _x, int _y);
	void				GetTextureData( GLuint _tex, GLenum _texType, TextureData* _texData );
	glm::vec4			SampleTexture_Linear( glm::vec2 _coords, TextureData* _texData );
	void				GetHeightData();
	void				SmoothVertices();
	void				CreateNormalsVBO();
	void				CreateNormalShader();
	void				CreateTerrainShader();
	void				CreateTerrainVBO();
	void				InitTriVertices();
	void				InitTriIndices();
	void				RenderTerrain();
	void				RenderNormals();
	void				InitNormals();
private:
	

	glm::vec4*	m_vertices;
	GLuint*		m_indices; 
	float*		m_heightData;
	glm::vec2*	m_texCoords;
	glm::vec3*	m_vertNormals;
	glm::uvec2	m_faceResolution;
	glm::uvec2	m_vertResolution;
	glm::vec4	m_position;
	GLuint		m_testTex;
	GLuint		m_heightmap1;
	GLuint		m_heightmapTxbo;
	glm::vec3	m_modelScale;
	glm::vec2	m_scaler;
	ShaderInfo	m_terrainShader;
	ShaderInfo	m_normalsShader;
	bool		m_renderNormals;

	GLuint
		m_terrainProjMatLoc,
		m_terrainViewMatLoc,
		m_terrainModelMatLoc,
		m_terrainHeightmapLoc,
		m_normalsProjMatLoc,
		m_normalsViewMatLoc,
		m_normalsModelMatLoc;
};