#include "Terrain.h"
#include "Simulation.h"
#include <stdint.h>


Simulation* m_sim = NULL;
void Terrain::Update( float _dt )
{
#if DEMO
	UpdateRenderState_Demo( _dt );
#elif
	UpdateRenderState_UserInput();
#endif
}

void Terrain::UpdateRenderState_Demo( float _dt )
{
	m_curRStateSecs += _dt;
	if( m_curRStateSecs >= m_demoStatesLength)
	{
		m_renderState = RenderState((int)m_renderState + 1);
		m_curRStateSecs = 0;

		// Loop 
		if(m_renderState >= RENDER_STATE_COUNT)
		{
			m_renderState = RenderState((int)RENDER_STATE_COUNT - (int)RENDER_STATE_COUNT);
		}
	}

	// Determine if we need to turn on normals
	if( (m_renderState == NORMALS_RENDER_STATE
		&& m_renderNormals == false)
		|| (m_renderState != NORMALS_RENDER_STATE
		&& m_renderNormals == true) )
	{
		m_renderNormals = !m_renderNormals;
	}
}

void Terrain::UpdateRenderState_UserInput()
{
	if(m_sim->GetOnKeyDown( SDLK_LEFTBRACKET ) )
	{
		int renderState = (int) m_renderState;
		renderState -= 1;

		if(renderState < 0)
		{
			renderState = (int)RENDER_STATE_COUNT - 1;
		}
		m_renderState = (RenderState)renderState;
	}

	if( m_sim->GetOnKeyDown( SDLK_RIGHTBRACKET ) )
	{
		int renderState = (int) m_renderState;
		renderState += 1;

		if(renderState >= RENDER_STATE_COUNT)
		{
			renderState = RENDER_STATE_COUNT - RENDER_STATE_COUNT;
		}
		m_renderState = (RenderState)renderState;
	}

	if( m_sim->GetOnKeyDown(SDLK_n)
		|| (m_renderState == NORMALS_RENDER_STATE
		&& m_renderNormals == false)
		|| (m_renderState != NORMALS_RENDER_STATE
		&& m_renderNormals == true) )
	{
		m_renderNormals = !m_renderNormals;
	}
}

void Terrain::Setup()
{
	m_sim = Simulation::GetSimulation();
	m_heightmap1 = LoadImage("Assets\\HeightMap4.jpg");
	GetTextureData(m_heightmap1, GL_TEXTURE_2D, &m_rawTexData);
	SetupTerrainTextures();
	GetHeightData();
	InitTriVertices();
	InitTriIndices();
	InitNormals();

	RenderObjBase::Setup();
	printOpenGLError();

}

void Terrain::Cleanup()
{
	RenderObjBase::Cleanup();
	glDeleteTextures(1, &m_heightmap1);
	
}

void Terrain::CreateNormalsVBO()
{
	glUseProgram(m_normalsShader.ProgramId);

	// Vertex array object (creates a place to store vertices)
	glGenVertexArrays(1, &m_normalsShader.VaoId);
	glBindVertexArray(m_normalsShader.VaoId);
	printOpenGLError();

	// Enables vertex shader attributes for use
	glEnableVertexAttribArray(0);

	// buffer vertex data to shader
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainShader.VboId);
	glBufferData(	GL_ARRAY_BUFFER, 
		sizeof(glm::vec4) * m_vertResolution.x * m_vertResolution.y, 
		m_vertices, 
		GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Enables vertex shader attribute for use
	glEnableVertexAttribArray(1);

	// creates a buffer object to transfer normals data to normals array
	// then buffers the data
	glBindBuffer(GL_ARRAY_BUFFER, m_normalsShader.VboId);
	glBufferData(	GL_ARRAY_BUFFER, 
		sizeof(glm::vec3) * m_vertResolution.x * m_vertResolution.y, 
		m_vertNormals, 
		GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Determines the draw order of the vertices we transferred to gpu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainShader.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndicesCount(), m_indices, GL_STATIC_DRAW);
	printOpenGLError();

	// Detaches vertex array
	glBindVertexArray(0);
	glUseProgram(0);
}

void Terrain::CreateTerrainVBO()
{
	glUseProgram(m_terrainShader.ProgramId);

	// Vertex array object (creates a place to store vertices)
	glGenVertexArrays(1, &m_terrainShader.VaoId);
	glBindVertexArray(m_terrainShader.VaoId);
	printOpenGLError();

	// Enables vertex shader attributes for use
	glEnableVertexAttribArray(0);

	// creates a buffer object to transfer vertices data to vertex array
	// then buffers the data
	glGenBuffers(1, &m_terrainShader.VboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainShader.VboId);
	glBufferData(	GL_ARRAY_BUFFER, 
		sizeof(glm::vec4) * m_vertResolution.x * m_vertResolution.y, 
		m_vertices, 
		GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Enables vertex shader attribute for use
	// specifically, normals
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_normalsShader.VboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalsShader.VboId);
	glBufferData(	GL_ARRAY_BUFFER, 
					sizeof(glm::vec3) * m_vertResolution.x * m_vertResolution.y, 
					m_vertNormals, 
					GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Enable texCoord at loc 2
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_snowBo);
	glBufferData(	GL_ARRAY_BUFFER, 
					sizeof(glm::vec2) * m_vertResolution.x * m_vertResolution.y, 
					m_snowTexCoords, 
					GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Enable texCoord at loc 3
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, m_rockFaceBo);
	glBufferData(	GL_ARRAY_BUFFER, 
					sizeof(glm::vec2) * m_vertResolution.x * m_vertResolution.y, 
					m_rockFaceTexCoords, 
					GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Enable texCoord at loc 4
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, m_grassBo);
	glBufferData(	GL_ARRAY_BUFFER, 
					sizeof(glm::vec2) * m_vertResolution.x * m_vertResolution.y, 
					m_grassTexCoords, 
					GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();

	// Determines the draw order of the vertices we transferred to gpu
	glGenBuffers(1, &m_terrainShader.IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainShader.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndicesCount(), m_indices, GL_STATIC_DRAW);
	printOpenGLError();

	// Get texture location
	/*m_terrainHeightmapLoc = glGetUniformLocation(m_terrainShader.ProgramId, "heightMap");
	// The 0, used below, sets the location of this texture, which ties into GL_TEXTURE0
	glProgramUniform1i(m_terrainShader.ProgramId, m_terrainHeightmapLoc , 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_heightmap1);	*/

	// Get texture locations
	m_snowTexLoc		= glGetUniformLocation(m_terrainShader.ProgramId, "snow");
	m_rockFaceTexLoc	= glGetUniformLocation(m_terrainShader.ProgramId, "rock");
	m_grassTexLoc		= glGetUniformLocation(m_terrainShader.ProgramId, "grass");

	// Bind the textures
	BindTexture(m_snowTex, GL_TEXTURE_2D, GL_TEXTURE0, 0, m_snowTexLoc, m_terrainShader);
	BindTexture(m_rockFaceTex, GL_TEXTURE_2D, GL_TEXTURE1, 1, m_rockFaceTexLoc, m_terrainShader);
	BindTexture(m_grassTex, GL_TEXTURE_2D, GL_TEXTURE2, 2, m_grassTexLoc, m_terrainShader);

	// Detaches vertex array
	glBindVertexArray(0);
	glUseProgram(0);
}

void Terrain::BindTexture( GLuint _tex, GLenum _texType, GLenum _texEnum, 
							int _texNum, GLuint _texLoc, ShaderInfo _shader )
{
	// The _texLoc, used below, sets the location of this texture, which ties into _texEnum
	glProgramUniform1i(_shader.ProgramId, _texLoc , _texNum);
	glActiveTexture(_texEnum);
	glBindTexture(_texType, _tex);	
}

void Terrain::CreateVBO()
{
	CreateTerrainVBO();
	CreateNormalsVBO();
}

void Terrain::CreateTerrainShader()
{
	// Load the shader
	ShaderFiles sFiles;
	sFiles.fragFile = (char *)malloc(sizeof(char) * (32));
	sFiles.vertFile = (char *)malloc(sizeof(char) * (32));
	memcpy(sFiles.vertFile, "Assets\\TerrainVert.txt", sizeof(char) * (32));
	memcpy(sFiles.fragFile, "Assets\\TerrainFrag.txt", sizeof(char) * (32));
	LoadShader(&m_terrainShader, sFiles);

	m_terrainModelMatLoc = glGetUniformLocation(m_terrainShader.ProgramId, "ModelMatrix");
	m_terrainViewMatLoc = glGetUniformLocation(m_terrainShader.ProgramId, "ViewMatrix");
	m_terrainProjMatLoc = glGetUniformLocation(m_terrainShader.ProgramId, "ProjectionMatrix");
	m_terrainNormalsMatLoc = glGetUniformLocation(m_terrainShader.ProgramId, "NormalsMatrix");
	m_terrainRenderStateLoc = glGetUniformLocation(m_terrainShader.ProgramId, "renderState");
	printOpenGLError();

	free(sFiles.fragFile);
	free(sFiles.vertFile);
}

void Terrain::SetupTerrainTextures()
{
	m_snowTex		= LoadImage("Assets\\snowTex.jpg");
	m_rockFaceTex	= LoadImage("Assets\\rockTex.jpg");
	m_grassTex		= LoadImage("Assets\\grassTex1.jpg");

	glBindTexture( GL_TEXTURE_2D, m_snowTex );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture( GL_TEXTURE_2D, m_rockFaceTex );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture( GL_TEXTURE_2D, m_grassTex );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenBuffers(1, &m_snowBo);
	glGenBuffers(1, &m_rockFaceBo);
	glGenBuffers(1, &m_grassBo);


	glm::vec2 snowCoordOffsets;
	glm::vec2 rockCoordOffsets;
	glm::vec2 grassCoordOffsets;

	snowCoordOffsets = glm::vec2( 20.f / m_vertResolution.x,
									20.f / m_vertResolution.y );

	rockCoordOffsets = glm::vec2( 50.f / m_vertResolution.x,
									50.f / m_vertResolution.y );

	grassCoordOffsets = glm::vec2( 80.f / m_vertResolution.x,
									80.f / m_vertResolution.y );

	// Here, we set the texture coordinates.
	// The plan is for these repeatable textures to repeat at different
	// rates so we get more variance when we render them.
	uint32_t idx = 0;
	for(uint32_t i=0; i < m_vertResolution.y; ++i)
	{
		for(uint32_t j=0; j < m_vertResolution.x; ++j)
		{
			idx = i * m_vertResolution.x + j;
			m_snowTexCoords[idx] = glm::vec2( snowCoordOffsets.x * j,
											 snowCoordOffsets.y * i );
			m_rockFaceTexCoords[idx] = glm::vec2( rockCoordOffsets.x * j,
											 rockCoordOffsets.y * i );
			m_grassTexCoords[idx] = glm::vec2( grassCoordOffsets.x * j,
											 grassCoordOffsets.y * i );
		}
	}
}

void Terrain::CleanupTerrainTextures()
{
	glDeleteBuffers(1, &m_grassBo);
	glDeleteBuffers(1, &m_rockFaceBo);
	glDeleteBuffers(1, &m_snowBo);

	glDeleteTextures(1, &m_rockFaceTex);
	glDeleteTextures(1, &m_grassTex);
	glDeleteTextures(1, &m_snowTex);
}

void Terrain::CreateNormalShader()
{
	// Load the shader
	ShaderFiles sFiles;
	sFiles.fragFile = (char *)malloc(sizeof(char) * (64));
	sFiles.geomFile = (char *)malloc(sizeof(char) * (64));
	sFiles.vertFile = (char *)malloc(sizeof(char) * (64));
	memcpy(sFiles.vertFile, "Assets\\TerrainNormalsVert.txt", sizeof(char) * (64));
	memcpy(sFiles.geomFile, "Assets\\TerrainNormalsGeo.txt", sizeof(char) * (64));
	memcpy(sFiles.fragFile, "Assets\\TerrainNormalsFrag.txt", sizeof(char) * (64));
	LoadShader(&m_normalsShader, sFiles);

	m_normalsModelMatLoc = glGetUniformLocation(m_normalsShader.ProgramId, "ModelMatrix");
	m_normalsViewMatLoc = glGetUniformLocation(m_normalsShader.ProgramId, "ViewMatrix");
	m_normalsProjMatLoc = glGetUniformLocation(m_normalsShader.ProgramId, "ProjectionMatrix");
	printOpenGLError();

	free(sFiles.vertFile);
	free(sFiles.geomFile);
	free(sFiles.fragFile);
}

void Terrain::CreateShaders()
{
	CreateTerrainShader();
	CreateNormalShader();
}

void Terrain::DestroyShaders()
{
	glDetachShader(m_normalsShader.ProgramId, m_normalsShader.FragmentShaderId);
	glDetachShader(m_normalsShader.ProgramId, m_normalsShader.GeometryShaderId);
	glDetachShader(m_normalsShader.ProgramId, m_normalsShader.VertexShaderId);
	printOpenGLError();

	glDetachShader(m_terrainShader.ProgramId, m_terrainShader.FragmentShaderId);
	glDetachShader(m_terrainShader.ProgramId, m_terrainShader.VertexShaderId);
	printOpenGLError();

	glDeleteShader(m_normalsShader.FragmentShaderId);
	glDeleteShader(m_normalsShader.GeometryShaderId);
	glDeleteShader(m_normalsShader.VertexShaderId);
	printOpenGLError();

	glDeleteShader(m_terrainShader.FragmentShaderId);
	glDeleteShader(m_terrainShader.VertexShaderId);
	printOpenGLError();

	glDeleteProgram(m_normalsShader.ProgramId);
	glDeleteProgram(m_terrainShader.ProgramId);
	printOpenGLError();
}

void Terrain::DestroyVBO()
{
	UnbindForRender();
	glDeleteBuffers(1, &m_normalsShader.IndexBufferId);
	glDeleteBuffers(1, &m_normalsShader.VboId);
	glDeleteVertexArrays(1, &m_normalsShader.VaoId);

	glDeleteBuffers(1, &m_heightmapTxbo);

	glDeleteBuffers(1, &m_terrainShader.IndexBufferId);
	glDeleteBuffers(1, &m_terrainShader.VboId);
	glDeleteVertexArrays(1, &m_terrainShader.VaoId);

	printOpenGLError();
}

int Terrain::VerticesCnt()
{
	return m_vertResolution.x * m_vertResolution.y;
}

int Terrain::TriCount()
{
	return m_faceResolution.x * m_faceResolution.y * 2;
}

int Terrain::IndicesCount()
{
	return TriCount() * 3;
}

void Terrain::RenderTerrain()
{
	// Wireframe rendering
	if( m_renderState == WIREFRAME_RENDER_STATE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	BindTerrainForRender();
	glDrawElements(GL_TRIANGLES, IndicesCount(), GL_UNSIGNED_INT, (GLvoid*)0);
	UnbindForRender();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	printOpenGLError();
}

void Terrain::RenderNormals()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	BindNormalsForRender();
	glDrawElements(GL_TRIANGLES, IndicesCount(), GL_UNSIGNED_INT, (GLvoid*)0);
	UnbindForRender();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	printOpenGLError();
}

void Terrain::Render()
{
	RenderTerrain();
	if(m_renderNormals)
	{
		RenderNormals();
	}
}

glm::mat4 Terrain::GetInverseWorldMat()
{
	return glm::inverse( GetModelMat() );
}

bool Terrain::AboveTerrain( glm::vec3 _pos )
{
	bool retval = false;
	glm::vec3 posInLocalSpace = (GetInverseWorldMat() * glm::vec4( _pos, 1.0f )).xyz;
	if( posInLocalSpace.x >= -.5f
		&& posInLocalSpace.x <= .5f
		&& posInLocalSpace.z >= -.5f
		&& posInLocalSpace.z <= .5f )
	{
		retval = true;
	}

	return retval;
}

glm::mat4 Terrain::GetModelMat()
{
	// Identity matrix times scale of this object
	glm::mat4 retval = glm::mat4(1.0f);
	glm::vec3 scaler = (m_scaler.x * glm::vec3(m_modelScale.x, 0, m_modelScale.z)) + (m_scaler.y * glm::vec3(0, m_modelScale.y, 0));
	return 	glm::scale(retval, scaler);
}

void Terrain::BindNormalsForRender()
{
	// use the shader
	glUseProgram(m_normalsShader.ProgramId);
	printOpenGLError();


	Simulation *sim = Simulation::GetSimulation();

	// Buffer the matrices
	glUniformMatrix4fv(m_normalsModelMatLoc, 1, GL_FALSE, &(GetModelMat())[0][0] );
	// THIS NEEDS TO BE VIEW MATRIX
	glUniformMatrix4fv(m_normalsViewMatLoc, 1, GL_FALSE, &(sim->GetViewMat())[0][0]);
	// Then get the perspective matrix
	glUniformMatrix4fv(m_normalsProjMatLoc, 1, GL_FALSE, &(sim->GetProjMat())[0][0]);

	// bind the vertex array
	glBindVertexArray(m_normalsShader.VaoId);
}

glm::mat3 Terrain::GetNormalsMatrix()
{
	glm::mat3 retval = glm::mat3();
	glm::mat4 viewModel = m_sim->GetViewMat() * GetModelMat();
	retval = glm::inverseTranspose(glm::mat3(retval));
	return retval;
}

void Terrain::BindTerrainForRender()
{
	// use the shader
	glUseProgram(m_terrainShader.ProgramId);
	printOpenGLError();

	// Buffer the matrices
	glUniformMatrix4fv(m_terrainModelMatLoc, 1, GL_FALSE, &(GetModelMat())[0][0] );
	// THIS NEEDS TO BE VIEW MATRIX
	glUniformMatrix4fv(m_terrainViewMatLoc, 1, GL_FALSE, &(m_sim->GetViewMat())[0][0]);
	// Then get the perspective matrix
	glUniformMatrix4fv(m_terrainProjMatLoc, 1, GL_FALSE, &(m_sim->GetProjMat())[0][0]);
	// Then buffer the normals matrix m_terrainNormalsMatLoc
	glUniformMatrix3fv(m_terrainNormalsMatLoc, 1, GL_FALSE, &(GetNormalsMatrix())[0][0]);
	// Buffer render state
	glUniform1i(m_terrainRenderStateLoc, (int)m_renderState);

	// bind the vertex array
	glBindVertexArray(m_terrainShader.VaoId);
}

void Terrain::UnbindForRender()
{
	glBindVertexArray(0);
	glUseProgram(0);
}

glm::vec4 Terrain::GetTexel( TextureData* _texData,  int _x, int _y)
{
	assert( _x < _texData->m_texWidth);
	assert( _y < _texData->m_texHeight);
	assert( _texData->m_byteCnt > 0);
	assert( _texData->m_texData != NULL);
	int idx = _y * _texData->TexByteWidth();
	glm::vec4 retval;

	switch( _texData->m_format)
	{
	case GL_RGB:
		idx += _x * 3;
		retval = glm::vec4(_texData->m_texData[idx],
							_texData->m_texData[idx+1],
							_texData->m_texData[idx+2],
							0.f);
		break;
	case GL_RGBA:
		idx += _x * 4;
		retval = glm::vec4(_texData->m_texData[idx],
			_texData->m_texData[idx+1],
			_texData->m_texData[idx+2],
			_texData->m_texData[idx+3]);
		break;
	}
	return retval;
}

void Terrain::GetTextureData( GLuint _tex, GLenum _texType, TextureData* _texData )
{
	_texData->m_texType = _texType;
	glBindTexture(_texData->m_texType, _tex);
	GLint format;
	glGetTexLevelParameteriv( _texData->m_texType, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
	glGetTexLevelParameteriv( _texData->m_texType, 0, GL_TEXTURE_WIDTH, &_texData->m_texWidth); 
	glGetTexLevelParameteriv( _texData->m_texType, 0, GL_TEXTURE_HEIGHT, &_texData->m_texHeight);
	_texData->m_byteCnt = _texData->m_texWidth*_texData->m_texHeight;
	
	switch(format)
	{
	case 3:
		_texData->m_channels = 3;
		_texData->m_byteCnt *= 3;
		_texData->m_format = GL_RGB;
		break;
	case 4:
		_texData->m_channels = 4;
		_texData->m_byteCnt *= 4;
		_texData->m_format = GL_RGBA;
		break;
	}
	_texData->SetupData(_texData->m_byteCnt);
	glBindTexture(_texData->m_texType, _tex);	
	glGetTexImage(_texData->m_texType, 0, _texData->m_format, GL_UNSIGNED_BYTE, _texData->m_texData);
	glBindTexture(_texData->m_texType, 0);	
}

glm::vec4 VecLerp( glm::vec4 _from, glm::vec4 _to, float _pct)
{
	return _from + (_to - _from) * _pct;
}

glm::vec4 Terrain::SampleTexture_Linear( glm::vec2 _coords, TextureData* _texData )
{
	float x = (_texData->m_texWidth-1) * _coords.x; // Get X coord in texture
	int ix = (int) x; // Get X coord as whole number
	int ix2 = glm::clamp(ix+1, 0, _texData->m_texWidth-1);
	float y = (_texData->m_texHeight-1) * _coords.y;
	int iy = (int) y;
	int iy2 = glm::clamp(iy+1, 0, _texData->m_texHeight-1);

	glm::vec4 x1 = GetTexel(_texData, ix, iy ); // Get top-left pixel
	glm::vec4 x2 = GetTexel(_texData, ix2, iy); // Get top-right pixel
	glm::vec4 y1 = GetTexel(_texData, ix, iy2); // Get bottom-left pixel
	glm::vec4 y2 = GetTexel(_texData, ix2, iy2); // Get bottom-right pixel

	glm::vec4 top = VecLerp(x1, x2, glm::fract(x) );
	glm::vec4 bot = VecLerp(y1, y2, glm::fract(x) );

	return VecLerp(top, bot, glm::fract(y) );
}

float Terrain::GetPctMaxHeightTerrain( glm::vec3 _otherWorldPos )
{
	glm::vec3 posInLocalSpace = (GetInverseWorldMat() * glm::vec4( _otherWorldPos, 1.0f )).xyz;
	return posInLocalSpace.y / m_heightDataScaler;
}

void Terrain::GetHeightData()
{
	glm::vec4 texSample;
	glm::vec2 coords;
	for(uint32_t y=0; y<m_vertResolution.y; ++y)
	{
		// Define coords in range of [0-1]
		coords.y = y/(m_vertResolution.y-1.f);
		for(uint32_t x=0; x<m_vertResolution.x; ++x)
		{
			coords.x = x/(m_vertResolution.x-1.f);
			texSample = SampleTexture_Linear(coords, &m_rawTexData);
			// Values are going to be in range of [0-255], need to make them in range of [0-1]
			m_heightData[y * m_vertResolution.x + x] = (texSample.x / 255.f) * m_heightDataScaler;
		}
	}
}

float Terrain::GetHeightAtPos( glm::vec3 _pos )
{
	float retval = 0.f;
	glm::vec3 posInLocalSpace = (GetInverseWorldMat() * glm::vec4( _pos, 1.0f )).xyz;
	// Convert range from [-.5,.5] to [0, 1]
	glm::vec2 uvCoords = posInLocalSpace.xz + glm::vec2(.5f, .5f);
	glm::vec4 texSample = SampleTexture_Linear(uvCoords, &m_rawTexData);
	// Values are going to be in range of [0-255], need to make them in range of [0-1]
	glm::vec4 localHeight = glm::vec4(0,(texSample.x / 255.f) * m_heightDataScaler,0,1);
	localHeight = GetModelMat() * localHeight;
	retval = localHeight.y;

	return retval;
}

void Terrain::InitTriVertices()
{
	//###########################################################
	// Here, we evenly distribute the vertices of the terrain
	// based on the face counts.
	// We're also going to set the texCoords.
	float xPos = -.5f;
	float zPos = -.5f;
	float texXPos = 0.f;
	float texYPos = 0.f;
	float xStart = -.5f;
	float wInc = 1.f/m_faceResolution.x;
	float dInc = 1.f/m_faceResolution.y;
	int idx = 0;

	for(uint16_t i=0; i < m_vertResolution.y; ++i)
	{
		for(uint16_t j=0; j < m_vertResolution.x; ++j)
		{
			idx = i * m_vertResolution.x + j;
			m_vertices[idx] = glm::vec4(xPos, m_heightData[idx], zPos, 1.f);
			xPos+=wInc;

			m_texCoords[idx] = glm::vec2(texXPos, texYPos);
			texXPos += wInc;
		}
		xPos = xStart;
		texXPos = 0.f;

		zPos+=dInc;
		texYPos+=dInc;
	}
	SmoothVertices();
}

void Terrain::SmoothVertices()
{
	//###########################################################
	// Now we average the positions so that the terrain is smooth
	uint32_t ii = 0;
	uint32_t jj = 0;
	float avgHeight = 0;
	int vertCnt;
	int oIdx = 0;
	int idx = 0;
	for(uint16_t i=0; i < m_vertResolution.y; ++i)
	{
		for(uint16_t j=0; j < m_vertResolution.x; ++j)
		{
			// Get this vert index
			idx = i * m_vertResolution.x + j;

			// Reset variables
			avgHeight = m_heightData[idx];
			vertCnt = 1; // We use 1 because we always have at least 1 vertex in the avg, THIS VERTEX
			oIdx = 0;

			// Check West vertex
			if(j > 0)
			{
				jj = j-1;
				ii = i;
				oIdx = ii * m_vertResolution.x + jj;
				avgHeight += m_heightData[oIdx];
				vertCnt++;
			}

			// Check North vertex
			if(i > 0)
			{
				jj = j;
				ii = i-1;
				oIdx = ii * m_vertResolution.x + jj;
				avgHeight += m_heightData[oIdx];
				vertCnt++;
			}

			// Check East vertex
			if(j < m_vertResolution.x - 1)
			{
				jj = j+1;
				ii = i;
				oIdx = ii * m_vertResolution.x + jj;
				avgHeight += m_heightData[oIdx];
				vertCnt++;
			}

			// Check South vertex
			if(i < m_vertResolution.y - 1)
			{
				jj = j;
				ii = i+1;
				oIdx = ii * m_vertResolution.x + jj;
				avgHeight += m_heightData[oIdx];
				vertCnt++;
			}
			avgHeight /= vertCnt;
			m_vertices[idx].y = avgHeight;
		}
	}
}

void Terrain::InitNormals()
{
	//m_vertNormals
	int idx = -1,
		v1Idx = -1,
		v2Idx = -1;
	glm::vec3	normal,
				eastWest,
				southNorth,
				v1,
				v2;
	for(uint32_t i=0; i<m_vertResolution.y; ++i)
	{
		for(uint32_t j=0; j<m_vertResolution.y; ++j)
		{
			idx = i * m_vertResolution.x + j;
			// Reset variables
			normal = glm::vec3();
			v1Idx = v2Idx = idx;

			// Get west vertex
			if(j > 0)
			{
				v1Idx = i * m_vertResolution.x + (j-1);
			}
			// Scale the vertices before getting the normal
			// so that the normal is correctly calculated
			v1 = (GetModelMat() * m_vertices[v1Idx]).xyz;

			// Get west vertex
			if(j < m_vertResolution.x - 1)
			{
				v2Idx = i * m_vertResolution.x + (j+1);
			}
			v2 = (GetModelMat() * m_vertices[v2Idx]).xyz;

 			eastWest = v1 - v2;
			eastWest = glm::normalize(eastWest);

			// Get North vertex
			if(i > 0)
			{
				v1Idx = (i-1)* m_vertResolution.x + j;
			}
			v1 = (GetModelMat() * m_vertices[v1Idx]).xyz;

			// Get South vertex
			if(i < m_vertResolution.y - 1)
			{
				v2Idx = (i+1)* m_vertResolution.x + j;
			}
			v2 = (GetModelMat() * m_vertices[v2Idx]).xyz;
			southNorth = v1 - v2;
			southNorth = glm::normalize(southNorth);

			normal = glm::cross( southNorth, eastWest);
			normal = glm::normalize(normal);
			if(normal.x == -0)
			{
				normal.x = 0;
			}
			if(normal.y == -0)
			{
				normal.y = 0;
			}
			if(normal.z == -0)
			{
				normal.z = 0;
			}
			
			m_vertNormals[idx] = normal;
		}
	}
}

void Terrain::InitTriIndices()
{
	//###########################################################
	// Here we provided the indices of the vertices we're going to use to draw the tris
	// and in what order.
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	int v4 = 0;
	int idxCnt = 0;

	for(uint16_t i=0; i<m_faceResolution.y; ++i)
	{
		for(uint16_t j=0; j<m_faceResolution.x; ++j)
		{
			// We use the face Idx to determine the vertex Idx we want
			// with the following calculation. It works like an ordinary
			// 2D array indexer, but with the added bonus of ignoring
			// vertices that reside on the side of the plane
			// and thus cannot be the starting corner of a face
			v1 = i * m_faceResolution.x + j + i;	// upper left vertex
			v2 = v1 + m_vertResolution.x;			// bot left vertex
			v3 = v2 + 1;							// bot right vertex
			v4 = v1 + 1;							// upper right vertex

			// We now add the indices that represent a triangle in CCW fashion,
			// 2 triangles per face
			m_indices[idxCnt++] = v1;
			m_indices[idxCnt++] = v4;
			m_indices[idxCnt++] = v2;
			m_indices[idxCnt++] = v4;
			m_indices[idxCnt++] = v3;
			m_indices[idxCnt++] = v2;
		}
	}
}
