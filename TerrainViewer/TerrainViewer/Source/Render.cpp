
#include "Main.h"
#include "Math.h"
#include "Render.h"

void init_GL()
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
	glAlphaFunc ( GL_GREATER, 0.99f );
	glEnable ( GL_ALPHA_TEST ) ;
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
	glAlphaFunc ( GL_GREATER, 0.99f );
	glEnable ( GL_ALPHA_TEST ) ;
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
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
		// Set the texture’s stretching properties
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