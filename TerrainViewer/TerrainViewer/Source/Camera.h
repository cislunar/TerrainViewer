#pragma once
#include "Math.h"
#include <stdlib.h>
#include <stdio.h>

#define CAMERA_POS_CAPTURE true
#define CAM_POS_FILENAME "Cam-Trail-Positions.txt"

class Camera
{
public:
	enum CAM_MOVE_STATE
	{
		ORBIT,
		USER_INPUT,
		CAM_MOVE_STATE_COUNT
	};
	Camera();
	~Camera()
	{
		CleanupCamPosFile();
	}
	void		SetPos( glm::vec3 _pos);
	void		SetRot( glm::vec3 _rot);
	glm::vec3*	GetPos();
	glm::vec3*	GetRot();
	glm::mat4	GetViewMat();
	glm::mat4	GetViewMat_NoTrans();
	glm::mat4	GetProjMat();
	void		Update( float _dt, glm::vec2 _mouseDelta );
	glm::vec3	GetRight();
	glm::mat4	GetRotMat();
	float*		GetOrbitSpeed(){return &m_orbitSpeed;}
	float*		GetSpringStr(){return &m_spring_Str;}
	float*		GetSpringDampStr(){return &m_spring_DampStr;}
	float*		GetSpringEquilibriumDist(){return &m_spring_EquilibriumDist;}
	float*		GetMass(){return & m_mass;}
	float*		GetGravity(){return &m_gravity;}

protected:
private:
	void UpdatePos( float _dt );
	void UpdatePos_Orbit( float _dt );
	void UpdatePos_Orbit_RoseCurve( float _dt );
	void Update_Pos_Orbit_Circle( float _dt );
	void UpdatePos_UserInput( float _dt );
	void UpdateRot( float _dt, glm::vec2 _mouseDelta  );
	void UpdateMoveState( );
	float UpdateHeight_SpringForce( glm::vec3 _newPos, float _dt );
	void SetupCamPosFile();
	void CleanupCamPosFile();
	void SavePosToFile();

	float m_follow_horizDist;
	float m_follow_vertDist;
	glm::vec3 m_orbitOffset;
	float m_orbitSpeed;
	glm::vec3 m_forward;
	glm::vec3 m_right;
	glm::vec3 m_pos;
	glm::vec3 m_prevPos;
	glm::vec3 m_rot;
	float 
		m_nearClipPlane,
		m_farClipPlane,
		m_FOV;
	float m_moveSpeed,
			m_moveSpeedMulti;
	float m_rotSpeed;
	float m_mass;
	float 
		m_yAccel,
		m_yVel,
		m_gravity;
	float 
		m_spring_Str,
		m_spring_DampStr,
		m_spring_EquilibriumDist,
		m_spring_minDist;
	CAM_MOVE_STATE m_moveState;
	FILE * m_camPosFile;
};