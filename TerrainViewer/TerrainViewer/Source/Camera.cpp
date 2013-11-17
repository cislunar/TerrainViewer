#include "Camera.h"
#include "Render.h"
#include "Simulation.h"


Camera::Camera()
{
	m_pos = glm::vec3(0,50000, 0);
	m_rot = glm::vec3();
	m_nearClipPlane = 1.0f;
	m_farClipPlane = 1000000.f;
	m_FOV = 15.f;
	m_moveSpeed = 10000.f;
	m_rotSpeed = 0.5f;
	m_forward = glm::vec3(0,0,-1);
	m_right = glm::vec3(1,0,0);
}

void Camera::Update( float _dt, glm::vec2 _mouseDelta )
{
	// Always update rotation before translation
	UpdateRot( _dt, _mouseDelta );
	UpdatePos(_dt);
}

void Camera::UpdatePos( float _dt )
{
	static Simulation* _sim = Simulation::GetSimulation();
	glm::vec3 moveDir = glm::vec3();
	glm::vec3 camPlaneForward = glm::cross(glm::vec3(0,1,0), m_right);
	// Global Z Axis
	if( _sim->GetKey(SDLK_w) )
	{
		moveDir += m_moveSpeed * camPlaneForward;
	}
	if( _sim->GetKey(SDLK_s))
	{
		moveDir += m_moveSpeed * (-camPlaneForward);
	}
	// Global X Axis
	if( _sim->GetKey(SDLK_a))
	{
		moveDir += m_moveSpeed * (-m_right);
	}
	if( _sim->GetKey(SDLK_d))
	{
		moveDir += m_moveSpeed * (m_right);
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

	glm::vec3 finalRot = m_rot + rotOffset;
	finalRot.x = glm::clamp(finalRot.x, -50.f, 50.f);
	finalRot.y = finalRot.y < 0 ? 
					finalRot.y + 360 : finalRot.y >= 360 ? 
						finalRot.y - 360 : finalRot.y;

	SetRot( finalRot );

	// Rotate orientation vectors
	// Rotate forward
	glm::vec3 newForward	= glm::rotateX( glm::vec3(0,0,-1), -m_rot.x );
	newForward				= glm::normalize(newForward);
	newForward				= glm::rotateY( newForward, m_rot.y );
	newForward				= glm::normalize(newForward);
	m_forward				= newForward;

	// Rotate right
	glm::vec3 newRight		= glm::rotateY( glm::vec3(1,0,0), -m_rot.y );
	newRight				= glm::normalize(newRight);
	m_right					= newRight;
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