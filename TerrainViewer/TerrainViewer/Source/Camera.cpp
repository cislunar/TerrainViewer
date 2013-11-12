#include "Camera.h"
#include "Render.h"

Camera::Camera()
{
	m_pos = glm::vec3(0,1, 4);
	m_rot = glm::vec3();
	m_nearClipPlane = 1.0f;
	m_farClipPlane = 6000.f;
	m_FOV = 60.f;
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

glm::mat4 Camera::GetViewMat()
{
	// Returns an identity matrix that has been pushed backwards
	// This mat keeps track of the camera position, rotation, and scale
	glm::mat4 retval = glm::mat4(1.0f);
	retval = glm::rotate(retval, m_rot.x, glm::vec3(1,0,0));
	retval = glm::translate( retval, -m_pos );
	return retval;
}
glm::mat4 Camera::GetProjMat()
{
	// This matrix keeps track of the camera's lens
	glm::mat4 retval = glm::perspective(m_FOV, (float)SCREEN_W / SCREEN_H, m_nearClipPlane, m_farClipPlane );
	return 	retval;
}