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
	void		SetPos( glm::dvec3 _pos);
	void		SetRot( glm::dvec3 _rot);
	glm::dvec3*	GetPos();
	glm::dvec3*	GetRot();
	glm::dmat4	GetViewMat();
	glm::dmat4	GetProjMat();
	void		Update( float _dt, glm::vec2 _mouseDelta );
	glm::dvec3	GetRight();
	glm::dmat4	GetRotMat();
	double*		GetOrbitSpeed(){return &m_orbitSpeed;}
	double*		GetSpringStr(){return &m_spring_Str;}
	double*		GetSpringDampStr(){return &m_spring_DampStr;}
	double*		GetSpringEquilibriumDist(){return &m_spring_EquilibriumDist;}
	double*		GetMass(){return & m_mass;}
	double*		GetGravity(){return &m_gravity;}

protected:
private:
	void UpdatePos( float _dt );
	void UpdatePos_Orbit( float _dt );
	void UpdatePos_Orbit_RoseCurve( float _dt );
	void Update_Pos_Orbit_Circle( float _dt );
	void UpdatePos_UserInput( float _dt );
	void UpdateRot( float _dt, glm::vec2 _mouseDelta  );
	void UpdateMoveState( );
	float UpdateHeight_SpringForce( glm::dvec3 _newPos, float _dt );
	void SetupCamPosFile();
	void CleanupCamPosFile();
	void SavePosToFile();

	double m_follow_horizDist;
	double m_follow_vertDist;
	glm::dvec3 m_orbitOffset;
	double m_orbitSpeed;
	glm::dvec3 m_forward;
	glm::dvec3 m_right;
	glm::dvec3 m_pos;
	glm::dvec3 m_prevPos;
	glm::dvec3 m_rot;
	double 
		m_nearClipPlane,
		m_farClipPlane,
		m_FOV;
	double m_moveSpeed,
			m_moveSpeedMulti;
	double m_rotSpeed;
	double m_mass;
	double 
		m_yAccel,
		m_yVel,
		m_gravity;
	double 
		m_spring_Str,
		m_spring_DampStr,
		m_spring_EquilibriumDist,
		m_spring_minDist;
	CAM_MOVE_STATE m_moveState;
	FILE * m_camPosFile;
};