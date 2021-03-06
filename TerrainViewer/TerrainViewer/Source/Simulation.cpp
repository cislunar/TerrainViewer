#include <sdl.h>
#include <math.h>
#include "Main.h"
#include "Simulation.h"
#include "RenderObj.h"
#include "RenderObjTest.h"
#include "Obj3D.h" 
#include "Terrain.h"
#include "Camera.h"
#include "CubeMap.h"

static bool			enableGravity = true;
static glm::vec2	grav(0, 175);
static bool			enableRotation = true;
static float		imgRot = 0;

static bool			drawLines = false;
static bool			followMouse = true;

Simulation Simulation::singleton;

static RenderObjTest renderObjTest;
static Obj3D			cube;
float cubeDist			= 100000;
float cubeYPos			= 20000.f;
static Terrain			terrain;

static bool				wireframe = false;

Camera m_cam;

CubeMap m_skyBox;

Simulation::Simulation()
{
	for(int i = 0; i < 322; i++) { // init them all to false
		m_KEYS[i] = false;
		m_prevKEYS[i] = false;
	}
	// you can configure this how you want, 
	//but it makes it nice for when you want to register a key 
	// continuously being held down
	SDL_EnableKeyRepeat(0,0); 
}

void Simulation::Shutdown()
{
	for(int i = 0; i < MAX_SPRITES; ++i)
	{
		glDeleteTextures( 1, &sprites[i] );
	}
	m_cam.DebugCleanup();
	terrain.Cleanup();
	m_skyBox.Cleanup();
}

// Draw all the sprites
void Simulation::DrawFrame()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	m_skyBox.Render();
	if(wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	}
	terrain.Render();
	//m_cam.DebugRender();
}

void Simulation::SimulateOneFrame(float deltaTime)
{
	int currX, currY;
	unsigned char buttons = SDL_GetMouseState( &currX, &currY );
	static int prevX = currX;
	static int prevY = currY;
	currX = m_mouseMotion.xrel + prevX;
	currY = m_mouseMotion.yrel + prevY;
	
	glm::vec2 dragDelta( (float)(currX - prevX), (float)(currY - prevY) );
	dragDelta *= .2f;
	glm::vec2 mousePos( (float)currX, (float)currY );
	prevX = currX;
	prevY = currY;

	if( GetOnKeyDown(SDLK_SPACE) )
	{
		wireframe = !wireframe;
	}
	
	if( GetOnKeyDown(SDLK_f))
	{
		followMouse = !followMouse;
	}

	dragDelta = followMouse == true ? dragDelta : glm::vec2();

	m_cam.Update( deltaTime, dragDelta );	
	RotationWrap();
	terrain.Update(deltaTime);

	memcpy(m_prevKEYS, m_KEYS, sizeof(bool) * 322);
	m_mouseMotion = SDL_MouseMotionEvent();
}

void Simulation::UpdateMouseMotion( SDL_MouseMotionEvent _mme )
{
	m_mouseMotion = _mme;
}


bool Simulation::GetOnKeyDown( SDLKey _key )
{
	bool retval = (m_KEYS[_key] == true && m_prevKEYS[_key] == false);
	return retval;
}


bool Simulation::GetKey( SDLKey _key )
{
	return m_KEYS[_key];
}

void Simulation::UpdatePrevKeys( SDLKey _key, bool _State )
{
	m_prevKEYS[_key] = _State;
}

void Simulation::UpdateKeys(SDLKey _key, bool _State )
{
	m_KEYS[_key] = _State;
}

float Simulation::GetHeightOnTerrain( glm::vec3 _other )
{
	float retval = 0;
	if(terrain.AboveTerrain( _other ))
	{
		retval = terrain.GetHeightAtPos( _other );
	}
	return retval;
}

float Simulation::GetPctMaxHeightTerrain( glm::vec3 _otherWorldPos )
{
	return terrain.GetPctMaxHeightTerrain( _otherWorldPos );
}

glm::mat4 Simulation::GetViewMat_NoTrans()
{
	return m_cam.GetViewMat_NoTrans();
}

glm::mat4 Simulation::GetViewMat()
{
	return m_cam.GetViewMat();
}
glm::mat4 Simulation::GetProjMat()
{
	return m_cam.GetProjMat();
}

void Simulation::RotationWrap()
{
	// Wrap any value greater than or equal to 180
	imgRot = fmod(imgRot,180.f);
	if(imgRot < 0)
	{
		imgRot += 360;
	}
}

void Log(const char *fmt, ...)
{
#if _DEBUG
	static char buf[1024];
	va_list args;
	va_start(args,fmt);
	vsprintf_s(buf,fmt,args);
	va_end(args);
	OutputDebugStringA(buf);
#endif
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void Simulation::Init()
{
	m_cam.DebugSetup();
	m_skyBox.Setup();
	terrain.Setup();

#if TWEAK_MENU
#define TWEAK_SEPERATOR()	TwAddSeparator(TwkBar(), "", "")

	//wireframe
	TwAddVarRW(TwkBar(), "Wireframe On",			TW_TYPE_BOOL8, &wireframe,							"");
	TWEAK_SEPERATOR();
	TwAddVarRW(TwkBar(), "Cam Orbit Speed",			TW_TYPE_FLOAT, m_cam.GetOrbitSpeed(),				"min=.0001 max=1");
	TwAddVarRW(TwkBar(), "Cam Spring Equilb Dist",	TW_TYPE_FLOAT, m_cam.GetSpringEquilibriumDist(),	"min=0 max=1000000");
	TwAddVarRW(TwkBar(), "Cam Spring Str",			TW_TYPE_FLOAT, m_cam.GetSpringStr(),				"min=0 max=5000000");
	TwAddVarRW(TwkBar(), "Cam Spring Damp Str",		TW_TYPE_FLOAT, m_cam.GetSpringDampStr(),			"min=0 max=5000000");
	TwAddVarRW(TwkBar(), "Cam Mass",				TW_TYPE_FLOAT, m_cam.GetMass(),						"min=0 max=1000");
	TwAddVarRW(TwkBar(), "Cam Gravity",				TW_TYPE_FLOAT, m_cam.GetGravity(),					"min=-50000 max=50000");
	

	//TwAddVarRW(TwkBar(), "Cam dist",			TW_TYPE_FLOAT, &m_cam.GetPos()->z,		"min=-500 max=500");
	//TwAddVarRW(TwkBar(), "Cam X Pos",			TW_TYPE_FLOAT, &m_cam.GetPos()->x,		"min=-500 max=500");
	//TwAddVarRW(TwkBar(), "X Rot",				TW_TYPE_FLOAT, &m_cam.GetRot()->x,		"min=0 max=360");
	//TwAddVarRW(TwkBar(), "Terr Horiz Scale",	TW_TYPE_FLOAT, &terrain.GetScale()->x,	"min=0 max=100000");
	//TwAddVarRW(TwkBar(), "Terr Vert Scale",		TW_TYPE_FLOAT, &terrain.GetScale()->y,	"min=0 max=100000");
	
	//TwAddVarRW(TwkBar(), "Gravity On",		TW_TYPE_BOOL8, &enableGravity,			"");
	//TwAddVarRW(TwkBar(), "Gravity",			TW_TYPE_FLOAT, &grav.y,					"min=0 max=200");
	//TwAddVarRW(TwkBar(), "Rotation On",		TW_TYPE_BOOL8, &enableRotation,			"");
	//TwAddVarRW(TwkBar(), "Rotation",		TW_TYPE_FLOAT, &imgRot,					"min=-360 max=360");

	TWEAK_SEPERATOR();
	//TwAddVarRW(TwkBar(), "Draw Lines",		TW_TYPE_BOOL8, &drawLines,				"");
#endif // TWEAK_MENU
}


