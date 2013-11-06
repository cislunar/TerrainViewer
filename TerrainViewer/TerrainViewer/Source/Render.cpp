#include "Main.h"
#include "Render.h"
#include <assert.h>

Render Render::singleton;

#pragma region Chapter 2 Shader tutorial
GLuint
	VertexShaderId,
	FragmentShaderId,
	ProgramId,
	VaoId,
	VboId,
	IndexBufferId;
const GLchar* VertexShader =
{
	"#version 400\n"\

	"layout(location=0) in vec4 in_Position;\n"\
	"layout(location=1) in vec4 in_Color;\n"\
	"out vec4 ex_Color;\n"\

	"void main(void)\n"\
	"{\n"\
	"   gl_Position = in_Position;\n"\
	"   ex_Color = in_Color;\n"\
	"}\n"
};

const GLchar* FragmentShader =
{
	"#version 400\n"\

	"in vec4 ex_Color;\n"\
	"out vec4 out_Color;\n"\

	"void main(void)\n"\
	"{\n"\
	"   out_Color = ex_Color;\n"\
	"}\n"
};

typedef struct
{
	float XYZW[4];
	float RGBA[4];
} Vertex;

void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}
void CreateVBO(void)
{
	Vertex Vertices[] =
	{
		{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		// Top
		{ { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Bottom
		{ { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Left
		{ { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Right
		{ { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
	};

	// defines the order to draw vertices in
	GLubyte Indices[] = {
		// Top
		0, 1, 3,
		0, 3, 2,
		3, 1, 4,
		3, 4, 2,
		// Bottom
		0, 5, 7,
		0, 7, 6,
		7, 5, 8,
		7, 8, 6,
		// Left
		0, 9, 11,
		0, 11, 10,
		11, 9, 12,
		11, 12, 10,
		// Right
		0, 13, 15,
		0, 15, 14,
		15, 13, 16,
		15, 16, 14
	};


	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].XYZW);

	// Generates array that describes how the vertex attributes are stored in VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	// Generates the actual buffer, and handle, to store the vertices
	glGenBuffers(1, &VboId);
	// Makes VboId the current buffer by binding it as a GL_ARRAY_BUFFER. 
	// Until another is bound, or this one unbound,
	// all buffer related operations execute on this buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// Uploads data to currently bound buffer
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);
	// glVertexAttribPointer defines the data that was just buffered by labeling 
	// it as the type associated with index=0 in the vertex shader.
	// We point to the attrib index, define the count, describe the type of data it is,
	// disable normalization, 
	// describe the stride between variables as VertexSize because that's
	// the size of the struct we're using as a variable,
	// and finally the pointer offset is 0 because we passed a single type data array
	// and there is no required offset to access the data we uploaded.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	// index has changed to 1 because it is the second vertex attribute.
	// changed pointer offset to RgbOffset to signify distance from start of 
	// individual vertex data to the color data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	// Enables the vertex attribute (variable) for use during drawing.
	// Should be disabled after this buffer is done being drawn.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Generates index buffer id
	glGenBuffers(1, &IndexBufferId);
	// specifies that the buffer we're uploading is an array that details
	// which vertices we are using and in what order
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void DestroyVBO(void)
{
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Binds nothing to GL_ARRAY_BUFFER: RESETS
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &VboId);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &IndexBufferId);

	// unbind vertex array
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void CreateShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);
	glUseProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void DestroyShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	glUseProgram(0);

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);

	glDeleteProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

#pragma endregion Chapter 2 Shader tutorial

Render::Render()
{
}

Render::~Render()
{
	//CleanUpDrawLine();
	Cleanup();
}



void Render::init_GL()
{
	// Set the OpenGL state after creating the context with SDL_SetVideoMode
	glClearColor( 0, 0, 0, 0 );
	glEnable( GL_TEXTURE_2D ); // Need this to display a texture
	glViewport( 0, 0, SCREEN_W, SCREEN_H );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, SCREEN_W, SCREEN_H, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	GLenum err = glewInit();
	fprintf(stderr, "Glew Error: %s\n", glewGetErrorString(err));
	if (err != GLEW_OK)
		exit(1); // or handle the error in a nicer way
	if(GLEW_VERSION_2_0)
	{
		printf("GLEW_VERSION_2_0 is supported\n");
	}
	else
	{
		printf("GLEW_VERSION_2_0 is not supported\n");
	}

#pragma region Chapter 2 Shader tutorial
	CreateShaders();
	CreateVBO();
#pragma endregion Chapter 2 Shader tutorial
}

void DrawImage( GLuint sourceTexture, Color& color, float xPos, float yPos, float width, float height )
{
	float halfW = width / 2.0f;
	float halfH = height / 2.0f;
	// Bind the texture to which subsequent calls refer to
//	glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, sourceTexture );
	glDisable ( GL_ALPHA_TEST ) ;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable ( GL_BLEND ) ;
	glBegin( GL_QUADS );
	// Top-left vertex (corner)
	glTexCoord2i( 0, 0 );
	glVertex3f( xPos-halfW, yPos-halfH, 0 );
	// Bottom-left vertex (corner)
	glTexCoord2i( 1, 0 );
	glVertex3f( xPos+halfW, yPos-halfH, 0 );
	// Bottom-right vertex (corner)
	glTexCoord2i( 1, 1 );
	glVertex3f( xPos+halfW, yPos+halfH, 0 );
	// Top-right vertex (corner)
	glTexCoord2i( 0, 1 );
	glVertex3f( xPos-halfW, yPos+halfH, 0 );
	glEnd();
}

void DrawImgRot( GLuint sourceTexture, Color& color, float xPos, float yPos, float width, float height, float rot )
{
	float halfW = width / 2.0f;
	float halfH = height / 2.0f;
	
	// NOTE: gl functions occur in reverse order from what is listed below
	//Return sprite to original position
	glTranslatef(xPos, yPos, 0);
	glRotatef( rot, 0.0, 0.0, 1.0 );
	// Move sprite to draw window origin (0,0) for rotation
	glTranslatef(-xPos, -yPos, 0);

	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);

	glEnable( GL_TEXTURE_2D );
	// Bind the texture to which subsequent calls refer to
	glBindTexture( GL_TEXTURE_2D, sourceTexture );
	glDisable ( GL_ALPHA_TEST ) ;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable ( GL_BLEND ) ;
	glBegin( GL_QUADS );
	// Top-left vertex (corner)
	glTexCoord2i( 0, 0 );
	glVertex3f( xPos-halfW, yPos-halfH, 0 );
	// Bottom-left vertex (corner)
	glTexCoord2i( 1, 0 );
	glVertex3f( xPos+halfW, yPos-halfH, 0 );
	// Bottom-right vertex (corner)
	glTexCoord2i( 1, 1 );
	glVertex3f( xPos+halfW, yPos+halfH, 0 );
	// Top-right vertex (corner)
	glTexCoord2i( 0, 1 );
	glVertex3f( xPos-halfW, yPos+halfH, 0 );
	glEnd();
}

void DrawLine( glm::vec2& start, glm::vec2& end, Color& color )
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINES);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

GLuint LoadImage( char* filename )
{
	GLuint texture=NULL;
	GLenum texture_format=NULL;
	SDL_Surface *surface; // this surface will tell us the details of the image
	GLint nofcolors;

	if ( (surface = IMG_Load(filename)) ) 
	{
		// Check that the image�s width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 ) 
		{
			printf("warning: image.bmp�s width is not a power of 2\n");
		}
		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 ) 
		{
			printf("warning: image.bmp�s height is not a power of 2\n");
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
			printf("warning: the image is not truecolor�this will break ");
		}
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
		// Set the texture�s stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, nofcolors, surface->w, surface->h, 0,
			texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	}
	else 
	{
		char temp[256];
		sprintf_s( temp, "SDL could not load image: %s\n", SDL_GetError() );
		OutputDebugStringA(temp);
		SDL_Quit();
		return 0;
	}
	// Free the SDL_Surface only if it was successfully created
	if ( surface ) 
	{
		SDL_FreeSurface( surface );
	}
	return texture;
}

void GLQueryCompileStatus(GLuint shader, GLenum type)
{
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", type, InfoLog);
		assert(false);
	}
}

void GLQueryLinkStatus(GLuint prog)
{
	GLint success;
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		assert(false);
	}
}


void GLQueryValidation(GLuint prog)
{
	GLint success;
	// Validate that all shaders play well together

	glGetProgramiv(prog, GL_VALIDATE_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error validating shader program: '%s'\n", ErrorLog);
		assert(false);
	}
}