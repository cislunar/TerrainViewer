#include "Camera.h"
#include "Render.h"
#include "Simulation.h"


Camera::Camera()
{
	m_pos = glm::vec3(0,1, 4);
	m_rot = glm::vec3();
	m_nearClipPlane = 1.0f;
	m_farClipPlane = 6000.f;
	m_FOV = 60.f;
	m_moveSpeed = 100.f;
	m_rotSpeed = 0.5f;
}

void Camera::Update( float _dt, glm::vec2 _mouseDelta )
{
	UpdateRot( _dt, _mouseDelta );
	UpdatePos(_dt);
}

void Camera::UpdatePos( float _dt )
{
	static Simulation* _sim = Simulation::GetSimulation();
	glm::vec3 moveDir = glm::vec3();
	// Global Z Axis
	if( _sim->GetKey(SDLK_w) )
	{
		moveDir += glm::vec3(0,0,-1) * m_moveSpeed;
	}
	if( _sim->GetKey(SDLK_s))
	{
		moveDir += glm::vec3(0,0,1) * m_moveSpeed;
	}
	// Global X Axis
	if( _sim->GetKey(SDLK_a))
	{
		moveDir += glm::vec3(-1,0,0) * m_moveSpeed;
	}
	if( _sim->GetKey(SDLK_d))
	{
		moveDir += glm::vec3(1,0,0) * m_moveSpeed;
	}
	// Global Y Axis
	if( _sim->GetKey(SDLK_q))
	{
		moveDir += glm::vec3(0,-1,0) * m_moveSpeed;
	}
	if( _sim->GetKey(SDLK_e))
	{
		moveDir += glm::vec3(0,1,0) * m_moveSpeed;
	}

	SetPos( m_pos + (moveDir * _dt) );
}
void Camera::UpdateRot( float _dt, glm::vec2 _mouseDelta )
{
	glm::vec3 rotOffset = glm::vec3();
	rotOffset.x = _mouseDelta.y * m_rotSpeed;
	rotOffset.y = _mouseDelta.x * m_rotSpeed;

	SetRot( m_rot + rotOffset );
}

void Camera::SetPos( glm::vec3 _pos)
{
	m_pos = _pos;
}
void Camera::SetRot( glm::vec3 _rot)
{
	m_rot = _rot;
}

glm::vec3* Camera::GetPos()
{
	return &m_pos;
}

glm::vec3* Camera::GetRot()
{
	return &m_rot;
}

glm::mat4 Camera::GetRotMat()
{
	return glm::rotate(glm::mat4(1.0f), m_rot.x, glm::vec3(1,0,0));
}

glm::mat4 Camera::GetViewMat()
{
	// Returns an identity matrix that has been pushed backwards
	// This mat keeps track of the camera position, rotation, and scale
	glm::mat4 retval = glm::mat4(1.0f);
	retval = glm::rotate(retval, m_rot.x, glm::vec3(1,0,0));
	retval = glm::rotate(retval, m_rot.y, glm::vec3(0,1,0));
	retval = glm::translate( retval, -m_pos );
	return retval;
}
glm::mat4 Camera::GetProjMat()
{
	// This matrix keeps track of the camera's lens
	glm::mat4 retval = glm::perspective(m_FOV, (float)SCREEN_W / SCREEN_H, m_nearClipPlane, m_farClipPlane );
	return 	retval;
}