#pragma once
#include "Math.h"

class Camera
{
public:
	enum CAM_MOVE_STATE
	{
		FOLLOW_PATH,
		USER_INPUT,
		CAM_MOVE_STATE_COUNT
	};
	Camera();
	void		SetPos( glm::vec3 _pos);
	void		SetRot( glm::vec3 _rot);
	glm::vec3*	GetPos();
	glm::vec3*	GetRot();
	glm::mat4	GetViewMat();
	glm::mat4	GetProjMat();
	void		Update( float _dt, glm::vec2 _mouseDelta );
	glm::vec3	GetRight();
	glm::mat4	GetRotMat();
	float*		GetOrbitSpeed(){return &m_orbitSpeed;}
protected:
private:
	void UpdatePos( float _dt );
	void UpdatePos_Orbit( float _dt );
	void UpdatePos_UserInput( float _dt );
	void UpdateRot( float _dt, glm::vec2 _mouseDelta  );
	void UpdateMoveState( );

	float m_follow_horizDist;
	float m_follow_vertDist;
	float m_orbitSpeed;
	glm::vec3 m_forward;
	glm::vec3 m_right;
	glm::vec3 m_pos;
	glm::vec3 m_prevPos;
	glm::vec3 m_rot;
	float m_nearClipPlane;
	float m_farClipPlane;
	float m_FOV;
	float m_moveSpeed;
	float m_rotSpeed;
	CAM_MOVE_STATE m_moveState;
};