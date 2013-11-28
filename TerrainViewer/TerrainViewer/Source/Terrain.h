#pragma once
#include "RenderObjBase.h"
#include <stdint.h>
#include "TextureData.h"

class Terrain : public RenderObjBase
{
public:
	enum RenderState
	{
		AMBIENT_RENDER_STATE,
		DIFFUSE_RENDER_STATE,
		SPEC_RENDER_STATE,
		PHONG_RENDER_STATE,
		NORMALS_RENDER_STATE,
		RENDER_STATE_COUNT
	};

	Terrain() : RenderObjBase()
	{
		m_renderState		= PHONG_RENDER_STATE;
		m_heightScaler		= 0.1f;
		m_renderNormals		= false;
		m_position			= glm::vec4(0,0,0,1);
		m_modelScale		= glm::vec3(m_defaultScale, m_defaultScale, m_defaultScale);
		m_scaler			=  glm::vec2( (float)m_defaultScaler, (float)m_defaultScaler);
		m_faceResolution	= glm::uvec2( (float)m_defaultFaceDim, (float)m_defaultFaceDim);
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

		m_snowTexCoords = (glm::vec2*)malloc(sizeof(glm::vec2) * VerticesCnt() );
		if(m_snowTexCoords == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_snowTexCoords, 0, sizeof(glm::vec2) * VerticesCnt() );

		m_rockFaceTexCoords = (glm::vec2*)malloc(sizeof(glm::vec2) * VerticesCnt() );
		if(m_rockFaceTexCoords == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_rockFaceTexCoords, 0, sizeof(glm::vec2) * VerticesCnt() );

		m_grassTexCoords = (glm::vec2*)malloc(sizeof(glm::vec2) * VerticesCnt() );
		if(m_grassTexCoords == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
		memset(m_grassTexCoords, 0, sizeof(glm::vec2) * VerticesCnt() );
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

		if(m_snowTexCoords != NULL)
		{
			free(m_snowTexCoords);
			m_snowTexCoords = NULL;
		}

		if(m_rockFaceTexCoords != NULL)
		{
			free(m_rockFaceTexCoords);
			m_rockFaceTexCoords = NULL;
		}

		if(m_grassTexCoords != NULL)
		{
			free(m_grassTexCoords);
			m_grassTexCoords = NULL;
		}
	}

	virtual void		Render(void);
	virtual glm::mat4	GetModelMat();
	void				Setup();
	void				Cleanup();
	glm::vec2*			GetScale() {return &m_scaler;}
	void				Update( float _dt );
	glm::vec3			GetPos(){return m_position.xyz;}
	float				GetHeightAtPos( glm::vec3 _pos );
	bool				AboveTerrain( glm::vec3 _pos );
	glm::mat4			GetInverseWorldMat();
	float				GetPctMaxHeightTerrain( glm::vec3 _otherWorldPos );


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
	glm::mat3			GetNormalsMatrix();
	void				SetupTerrainTextures();
	void				CleanupTerrainTextures();
	void				BindTexture( GLuint _tex, GLenum _texType, GLenum _texEnum,
							int _texNum, GLuint _texLoc, ShaderInfo _shader );

private:
	
	TextureData m_rawTexData;
	float		m_heightScaler;
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
	RenderState m_renderState;
	const int	m_defaultScale = 20;
	const int	m_defaultScaler = 15000;
	const int	m_defaultFaceDim = 1024;

	glm::vec2	
		*m_snowTexCoords,
		*m_rockFaceTexCoords,
		*m_grassTexCoords;

	GLuint		
		m_snowTex,
		m_snowBo,
		m_snowTexLoc,
		m_rockFaceTex,
		m_rockFaceBo,
		m_rockFaceTexLoc,
		m_grassTex,
		m_grassBo,
		m_grassTexLoc;

	GLuint
		m_terrainProjMatLoc,
		m_terrainViewMatLoc,
		m_terrainModelMatLoc,
		m_terrainNormalsMatLoc,
		m_terrainHeightmapLoc,
		m_terrainRenderStateLoc,
		m_normalsProjMatLoc,
		m_normalsViewMatLoc,
		m_normalsModelMatLoc;
};