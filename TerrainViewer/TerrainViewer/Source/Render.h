#ifndef __RENDER_H__
#define __RENDER_H__

#include <Windows.h>
#include "GL\glew.h"
#include "SDL.h"
#include "SDL_image.h"
#include "Math.h"
#include "GL\glut.h"

#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char *file, int line);

#if 1
#define SCREEN_W 1920
#define SCREEN_H 1080
#else
#define SCREEN_W 1024
#define SCREEN_H 768
#endif


struct Color
{
	Color( float _r, float _g, float _b ) : r(_r), g(_g), b(_b) {}
	float r, g, b;
};

struct ShaderInfo
{
	GLuint
		VertexShaderId,
		FragmentShaderId,
		GeometryShaderId,
		ProgramId,
		VaoId,
		VboId,
		IndexBufferId;
};

struct ShaderFiles
{
	ShaderFiles()
	{
		vertFile = NULL;
		geomFile = NULL;
		fragFile = NULL;
	}
	char* vertFile;
	char* fragFile;
	char* geomFile;
};

void	DrawImage( GLuint sourceTexture, Color& color, float xPos, float yPos, float width, float height );
void	DrawImgRot( GLuint sourceTexture, Color& color, float xPos, float yPos, float width, float height, float rot );
void	DrawLine( glm::vec2& start, glm::vec2& end, Color& color );
GLuint	LoadImage( char* filename );
void	GLQueryCompileStatus(GLuint shader, GLenum type);
void	GLQueryLinkStatus(GLuint prog);
void	GLQueryValidation(GLuint prog);
void	LoadShader(ShaderInfo* _shader, ShaderFiles sFiles);


class Render
{
public:
	~Render();
	static Render*		GetInstance() {return &singleton;}
	void				init_GL();
	//void				DrawLine_Shader( Vec2& start, Vec2& end, Color& color );
	//void				LoadShader(Shader* _shader, ShaderFiles sFiles);


private:
	// Singleton enforcement
	Render();

	//void			InitDrawLine();
	//void			CleanUpDrawLine();

	// static variables
	static	Render	singleton;
	//Shader			m_drawLineShader;
};

#endif // __RENDER_H__