#pragma once
#include "Render.h"

typedef struct
{
	float XYZW[4];
	float RGBA[4];
} Vertex;

class RenderObjBase
{
public:
	RenderObjBase(void){}
	virtual void Setup(void){CreateVBO(); CreateShaders(); }
	virtual void Cleanup(void){ DestroyShaders(); DestroyVBO();}
	virtual void Render(void) = 0;

protected:
	virtual void CreateVBO(void)		= 0;
	virtual void CreateShaders(void)	= 0;
	virtual void DestroyVBO(void)		= 0;
	virtual void DestroyShaders(void)	= 0;

private:
};