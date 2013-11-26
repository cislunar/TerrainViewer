
#include <stdlib.h>	// For random tint
#include "Math.h"
#include "Render.h"

void Log(const char *fmt, ...);


class Simulation
{
public:
	static Simulation* GetSimulation() {return &singleton;}

	void Init();
	void Shutdown();

	void SimulateOneFrame(float deltaTime);	// A single logic step
	void DrawFrame();						// Draw all the sprites
	void RotationWrap();					// Wrap rotation of sprites
	glm::mat4 GetViewMat();
	glm::mat4 GetProjMat();
	void UpdatePrevKeys( SDLKey _key, bool _State );
	void UpdateKeys(SDLKey _key, bool _State );
	bool GetKey( SDLKey _key );
	bool GetOnKeyDown( SDLKey _key );
	float GetHeightOnTerrain( glm::vec3 _other );

private:
	Simulation();							// Force use of singleton

	static Simulation	singleton;
	static const int	MAX_SPRITES = 2;
	GLuint				sprites[MAX_SPRITES];
	bool				m_KEYS[322]; // Number of SDLK_DOWN events
	bool				m_prevKEYS[322]; // Number of SDLK_DOWN events

};
