#include "CubeMap.h"
#include "Simulation.h"

Simulation* _sim = Simulation::GetSimulation();

static const GLenum cubeMapTexTypes[6] = 
{  
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 
};

float cubeVerts[] =
{
	// Front face, bot left CW
	-.5f, -.5f,  .5f, 1,
	-.5f,  .5f,  .5f, 1,
	 .5f,  .5f,  .5f, 1,
	 .5f, -.5f,  .5f, 1,
	// Back, bot left CW
	-.5f, -.5f, -.5f, 1,
	-.5f,  .5f, -.5f, 1,
	 .5f,  .5f, -.5f, 1,
	 .5f, -.5f, -.5f, 1
};

// defines the order to draw vertices in
GLuint cubeIndices[] = {
	0,2,1,  0,3,2,
	4,3,0,  4,7,3,
	4,1,5,  4,0,1,
	3,6,2,  3,7,6,
	1,6,5,  1,2,6,
	7,5,6,  7,4,5
};

void CubeMap::Setup()
{
	// Load shaders
	CreateSkyBoxShader();

	// Buffer the textures that go into the cube map
	LoadCubeMap();

	// Create VBO for skybox so it can render
	CreateSkyboxVBO();
}

void CubeMap::CreateSkyBoxShader()
{
	// Load the shader
	ShaderFiles sFiles;
	sFiles.fragFile = (char *)malloc(sizeof(char) * (32));
	sFiles.vertFile = (char *)malloc(sizeof(char) * (32));
	memcpy(sFiles.vertFile, "Assets\\CubeMapVert.txt", sizeof(char) * (32));
	memcpy(sFiles.fragFile, "Assets\\CubeMapFrag.txt", sizeof(char) * (32));
	LoadShader(&m_skyboxShader, sFiles);

	m_cubeMapViewMatLoc = glGetUniformLocation(m_skyboxShader.ProgramId, "V");
	m_cubeMapProjMatLoc = glGetUniformLocation(m_skyboxShader.ProgramId, "P");
	printOpenGLError();

	free(sFiles.fragFile);
	free(sFiles.vertFile);
}

void CubeMap::CreateSkyboxVBO()
{
	glUseProgram(m_skyboxShader.ProgramId);

	// Vertex array object (creates a place to store vertices)
	glGenVertexArrays (1, &m_skyboxShader.VaoId);
	glBindVertexArray (m_skyboxShader.VaoId);

	// Enable position attribute
	glEnableVertexAttribArray (0);

	// creates a buffer object to transfer vertices data to vertex array
	// then buffers the data
	glGenBuffers (1, &m_skyboxShader.VboId);
	glBindBuffer (GL_ARRAY_BUFFER, m_skyboxShader.VboId);
	glBufferData (GL_ARRAY_BUFFER, sizeof(float) * 32, &cubeVerts, GL_STATIC_DRAW);
	printOpenGLError();
	
	// defines the data we just transferred to the gpu
	glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	printOpenGLError();

	// Determines the draw order of the vertices we transferred to gpu
	glGenBuffers(1, &m_skyboxShader.IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_skyboxShader.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 36, cubeIndices, GL_STATIC_DRAW);
	printOpenGLError();

	// Detaches vertex array
	glBindVertexArray(0);
	glUseProgram(0);
}

void CubeMap::LoadCubeMap()
{
	char *files[6] = 
	{
		CUBE_MAP_LEFT,	
		CUBE_MAP_RIGHT,
		CUBE_MAP_UP,
		CUBE_MAP_DOWN,
		CUBE_MAP_FRONT,	
		CUBE_MAP_BACK	
	};

	// Generate our cubemap image and bind it
	glGenTextures(1, &m_cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);

	GLenum texture_format=NULL;
	SDL_Surface *surface; // this surface will tell us the details of the image
	GLint nofcolors;

	for( int i=0; i<6; ++i)
	{
		char* file = files[i];
		if ( (surface = IMG_Load(file)) ) 
		{
			// Check that the image’s width is a power of 2
			if ( (surface->w & (surface->w - 1)) != 0 ) 
			{
				printf("warning: image.bmp’s width is not a power of 2\n");
			}
			// Also check if the height is a power of 2
			if ( (surface->h & (surface->h - 1)) != 0 ) 
			{
				printf("warning: image.bmp’s height is not a power of 2\n");
			}
			//get number of channels in the SDL surface
			nofcolors=surface->format->BytesPerPixel;
			//contains an alpha channel
			if(nofcolors==4)
			{
				if(surface->format->Rmask==0x000000ff)
					texture_format=GL_RGBA;
				else
					texture_format=GL_BGRA;
			}
			else if(nofcolors==3) //no alpha channel
			{
				if(surface->format->Rmask==0x000000ff)
					texture_format=GL_RGB;
				else
					texture_format=GL_BGR;
			}
			else
			{
				printf("warning: the image is not truecolor…this will break ");
			}
			// buffer the data to the gpu to the correct part of the cubemap tex
			glTexImage2D( cubeMapTexTypes[i], 0, nofcolors, surface->w, surface->h, 0,
				texture_format, GL_UNSIGNED_BYTE, surface->pixels );
		}
		else 
		{
			char temp[256];
			sprintf_s( temp, "SDL could not load image: %s\n", SDL_GetError() );
			printf("IMG_Load: %s\n", IMG_GetError());
			OutputDebugStringA(temp);
			SDL_Quit();
		}
		// Free the SDL_Surface only if it was successfully created
		if ( surface ) 
		{
			SDL_FreeSurface( surface );
		}
	}

	// define the texture data
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint cubeMapLoc		= glGetUniformLocation(m_skyboxShader.ProgramId, "cube_texture");
	glProgramUniform1i(m_skyboxShader.ProgramId, cubeMapLoc , 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);	
}

void CubeMap::Cleanup()
{
	DestroySkyboxVBO();

	if (m_cubeMap != 0) {
		glDeleteTextures(1, &m_cubeMap);
	}

	DestroySkyBoxShader();
}

void CubeMap::Render()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
	UnBind();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	printOpenGLError();

}

void CubeMap::Bind()
{
	glUseProgram (m_skyboxShader.ProgramId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
	
	// Buffer the matrices
	// THIS NEEDS TO BE VIEW MATRIX
	glUniformMatrix4fv(m_cubeMapViewMatLoc, 1, GL_FALSE, &(_sim->GetViewMat_NoTrans())[0][0]);
	// Then get the perspective matrix
	glUniformMatrix4fv(m_cubeMapProjMatLoc, 1, GL_FALSE, &(_sim->GetProjMat())[0][0]);

	// Bind vertex array for rendering
	glBindVertexArray (m_skyboxShader.VaoId);
}

void CubeMap::DestroySkyBoxShader()
{
	glDetachShader(m_skyboxShader.ProgramId, m_skyboxShader.FragmentShaderId);
	glDetachShader(m_skyboxShader.ProgramId, m_skyboxShader.VertexShaderId);
	printOpenGLError();

	glDeleteShader(m_skyboxShader.FragmentShaderId);
	glDeleteShader(m_skyboxShader.VertexShaderId);
	printOpenGLError();

	glDeleteProgram(m_skyboxShader.ProgramId);
	printOpenGLError();
}

void CubeMap::DestroySkyboxVBO()
{
	UnBind();
	glDeleteBuffers(1, &m_skyboxShader.IndexBufferId);
	glDeleteBuffers(1, &m_skyboxShader.VboId);
	glDeleteVertexArrays(1, &m_skyboxShader.VaoId);

	printOpenGLError();
}

void CubeMap::UnBind()
{
	glBindVertexArray(0);
	glUseProgram(0);
}