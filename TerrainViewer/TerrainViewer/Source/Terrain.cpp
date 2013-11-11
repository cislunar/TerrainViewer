#include "Terrain.h"
#include "Simulation.h"

void Terrain::Setup()
{
	RenderObjBase::Setup();
	m_testTex = LoadImage("Source\\TexTest.png");
	printOpenGLError();
}

void Terrain::Cleanup()
{
	RenderObjBase::Cleanup();
	glDeleteTextures(1, &m_testTex);
}

void Terrain::CreateVBO()
{

	glUseProgram(m_renderInfo.ProgramId);

	// Vertex array object (creates a place to store vertices)
	glGenVertexArrays(1, &m_renderInfo.VaoId);
	glBindVertexArray(m_renderInfo.VaoId);
	printOpenGLError();

	// Enables vertex shader attributes for use
	glEnableVertexAttribArray(0);

	// creates a buffer object to transfer vertices data to vertex array
	// then buffers the data
	glGenBuffers(1, &m_renderInfo.VboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_renderInfo.VboId);
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

	// Create and bind texture buffer object
	glGenBuffers(1, &m_txbo);

	// Get texture uv location and upload
	glBindBuffer(GL_ARRAY_BUFFER, m_txbo);
	glBufferData(	GL_ARRAY_BUFFER, 
					sizeof(glm::vec2) * m_vertResolution.y * m_vertResolution.x, 
					m_texCoords, 
					GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	printOpenGLError();


	// Determines the draw order of the vertices we transferred to gpu
	glGenBuffers(1, &m_renderInfo.IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderInfo.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndicesCount(), m_indices, GL_STATIC_DRAW);
	printOpenGLError();

	// Get texture location
	m_texLocation = glGetUniformLocation(m_renderInfo.ProgramId, "heightMap");
	// The 0, used below, sets the location of this texture, which ties into GL_TEXTURE0
	glProgramUniform1i(m_renderInfo.ProgramId, m_texLocation , 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_testTex);	

	// Detaches vertex array
	glBindVertexArray(0);
	glUseProgram(0);
}

void Terrain::CreateShaders()
{
	// Load the shader
	ShaderFiles sFiles;
	sFiles.fragFile = (char *)malloc(sizeof(char) * (32));
	sFiles.vertFile = (char *)malloc(sizeof(char) * (32));
	memcpy(sFiles.vertFile, "Source\\TerrainVert.txt", sizeof(char) * (32));
	memcpy(sFiles.fragFile, "Source\\TerrainFrag.txt", sizeof(char) * (32));
	LoadShader(&m_renderInfo, sFiles);

	ModelMatrixUniformLocation = glGetUniformLocation(m_renderInfo.ProgramId, "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(m_renderInfo.ProgramId, "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(m_renderInfo.ProgramId, "ProjectionMatrix");
	printOpenGLError();

	free(sFiles.fragFile);
	free(sFiles.vertFile);
}

void Terrain::DestroyShaders()
{
	glDetachShader(m_renderInfo.ProgramId, m_renderInfo.FragmentShaderId);
	glDetachShader(m_renderInfo.ProgramId, m_renderInfo.VertexShaderId);
	printOpenGLError();

	glDeleteShader(m_renderInfo.FragmentShaderId);
	glDeleteShader(m_renderInfo.VertexShaderId);
	printOpenGLError();

	glDeleteProgram(m_renderInfo.ProgramId);
	printOpenGLError();
}

void Terrain::DestroyVBO()
{
	UnbindForRender();
	glDeleteBuffers(1, &m_txbo);
	glDeleteBuffers(1, &m_renderInfo.IndexBufferId);
	glDeleteBuffers(1, &m_renderInfo.VboId);
	glDeleteVertexArrays(1, &m_renderInfo.VaoId);
	printOpenGLError();
}

int Terrain::TriCount()
{
	return m_faceResolution.x * m_faceResolution.y * 2;
}

int Terrain::IndicesCount()
{
	return TriCount() * 3;
}


void Terrain::Render()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	BindForRender();
	glDrawElements(GL_TRIANGLES, IndicesCount(), GL_UNSIGNED_INT, (GLvoid*)0);
	UnbindForRender();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	printOpenGLError();
}

glm::mat4 Terrain::GetModelMat()
{
	// Identity matrix times scale of this object
	glm::mat4 retval = glm::mat4(1.0f);
	return 	glm::scale(retval, glm::vec3(m_scale,m_scale,m_scale) );
}


void Terrain::BindForRender()
{
	// use the shader
	glUseProgram(m_renderInfo.ProgramId);
	printOpenGLError();


	Simulation *sim = Simulation::GetSimulation();

	// Buffer the matrices
	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, &(GetModelMat())[0][0] );
	// THIS NEEDS TO BE VIEW MATRIX
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, &(sim->GetViewMat())[0][0]);
	// Then get the perspective matrix
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, &(sim->GetProjMat())[0][0]);

	// bind the vertex array
	glBindVertexArray(m_renderInfo.VaoId);
}

void Terrain::UnbindForRender()
{
	glBindVertexArray(0);
	glUseProgram(0);
}

void Terrain::Init()
{
	// Here, we evenly distribute the vertices of the terrain
	// based on the face counts.
	// We're also going to set the texCoords.
	float xPos = -.5f;
	float zPos = .5f;
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
			m_vertices[idx] = glm::vec4(xPos, 0.f, zPos, 1.f);
			xPos+=wInc;

			m_texCoords[idx] = glm::vec2(texXPos, texYPos);
			texXPos += wInc;
		}
		xPos = xStart;
		texXPos = 0.f;

		zPos-=dInc;
		texYPos+=dInc;
	}


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
			//TODO Change the order to be CW because
			m_indices[idxCnt++] = v1;
			m_indices[idxCnt++] = v4;
			m_indices[idxCnt++] = v2;
			m_indices[idxCnt++] = v4;
			m_indices[idxCnt++] = v3;
			m_indices[idxCnt++] = v2;
		}
	}
}
