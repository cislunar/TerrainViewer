#include "Obj3D.h"
#include "Simulation.h"

void Obj3D::CreateVBO()
{
	Vertex cubeVerts[] =
	{
		// Front face, bot left CW
		{ { -.5f, -.5f,  .5f, 1 }, { 1, 1, 1, 1 } },
		{ { -.5f,  .5f,  .5f, 1 }, { 1, 1, 1, 1 } },
		{ {  .5f,  .5f,  .5f, 1 }, { 1, 1, 1, 1 } },
		{ {  .5f, -.5f,  .5f, 1 }, { 1, 1, 1, 1 } },
		// Back, bot left clockwise
		{ { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
		{ { -.5f,  .5f, -.5f, 1 }, { 1, 1, 1, 1 } },
		{ {  .5f,  .5f, -.5f, 1 }, { 1, 1, 1, 1 } },
		{ {  .5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } }
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

	glUseProgram(m_shaderInfo.ProgramId);

	// Vertex array object (creates a place to store vertices)
	glGenVertexArrays(1, &m_shaderInfo.VaoId);
	glBindVertexArray(m_shaderInfo.VaoId);
	printOpenGLError();

	// Enables vertex shader attributes for use
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// creates a buffer object to transfer vertices data to vertex array
	// then buffers the data
	glGenBuffers(1, &m_shaderInfo.VboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_shaderInfo.VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	printOpenGLError();

	// defines the data we just transferred to the gpu
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(cubeVerts[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(cubeVerts[0]), 
		(GLvoid*)sizeof(cubeVerts[0].Position));
	printOpenGLError();

	// Determines the draw order of the vertices we transferred to gpu
	glGenBuffers(1, &m_shaderInfo.IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shaderInfo.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	printOpenGLError();

	// Detaches vertex array
	glBindVertexArray(0);
	glUseProgram(0);
}

void Obj3D::CreateShaders()
{
	// Load the shader
	ShaderFiles sFiles;
	sFiles.fragFile = (char *)malloc(sizeof(char) * (32));
	sFiles.vertFile = (char *)malloc(sizeof(char) * (32));
	memcpy(sFiles.vertFile, "Source\\CubeVert.txt", sizeof(char) * (32));
	memcpy(sFiles.fragFile, "Source\\CubeFrag.txt", sizeof(char) * (32));
	LoadShader(&m_shaderInfo, sFiles);

	ModelMatrixUniformLocation = glGetUniformLocation(m_shaderInfo.ProgramId, "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(m_shaderInfo.ProgramId, "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(m_shaderInfo.ProgramId, "ProjectionMatrix");
	printOpenGLError();

	free(sFiles.fragFile);
	free(sFiles.vertFile);
}

void Obj3D::DestroyShaders()
{
	glDetachShader(m_shaderInfo.ProgramId, m_shaderInfo.FragmentShaderId);
	glDetachShader(m_shaderInfo.ProgramId, m_shaderInfo.VertexShaderId);
	printOpenGLError();

	glDeleteShader(m_shaderInfo.FragmentShaderId);
	glDeleteShader(m_shaderInfo.VertexShaderId);
	printOpenGLError();

	glDeleteProgram(m_shaderInfo.ProgramId);
	printOpenGLError();
}

void Obj3D::DestroyVBO()
{
	UnbindForRender();
	glDeleteBuffers(1, &m_shaderInfo.IndexBufferId);
	glDeleteBuffers(1, &m_shaderInfo.VboId);
	glDeleteVertexArrays(1, &m_shaderInfo.VaoId);
	printOpenGLError();
}

void Obj3D::Render()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	BindForRender();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
	UnbindForRender();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	printOpenGLError();
}

glm::mat4 Obj3D::GetModelMat()
{
	glm::mat4 retval = glm::mat4(1.0f);
	glm::vec3 translate = m_position.xyz - m_prevPos.xyz;
	retval = glm::translate(retval, translate );

	// Identity matrix times scale of this object
	glm::vec3 scaler = glm::vec3( m_scale, m_scale, m_scale );
	retval = glm::scale(retval,  scaler);

	
	return retval;
}


void Obj3D::BindForRender()
{
	// use the shader
	glUseProgram(m_shaderInfo.ProgramId);
	printOpenGLError();
	

	Simulation *sim = Simulation::GetSimulation();

	// Buffer the matrices
	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, &(GetModelMat())[0][0] );
	// THIS NEEDS TO BE VIEW MATRIX
	glUniformMatrix4dv(ViewMatrixUniformLocation, 1, GL_FALSE, &(sim->GetViewMat())[0][0]);
	// Then get the perspective matrix
	glUniformMatrix4dv(ProjectionMatrixUniformLocation, 1, GL_FALSE, &(sim->GetProjMat())[0][0]);

	// bind the vertex array
	glBindVertexArray(m_shaderInfo.VaoId);
}

void Obj3D::UnbindForRender()
{
	glBindVertexArray(0);
	glUseProgram(0);
}

void Obj3D::SetPos( glm::vec3 _pos )
{
	m_prevPos.xyz = m_position.xyz;
	m_position.xyz = _pos;
}
