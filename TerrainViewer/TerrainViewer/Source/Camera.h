#pragma once
#include "Math.h"

class Camera
{
public:
	Camera();
	void SetPos( glm::vec3 _pos);
	void SetRot( glm::vec3 _rot);
	glm::vec3* GetPos();
	glm::vec3* GetRot();
	glm::mat4 GetViewMat();
	glm::mat4 GetProjMat();
	void Camera::Update( float _dt, glm::vec2 _mouseDelta );
	glm::vec3 GetRight();
	glm::mat4 GetRotMat();
protected:
private:
	void UpdatePos( float _dt );
	void UpdateRot( float _dt, glm::vec2 _mouseDelta  );

	glm::vec3 m_pos;
	glm::vec3 m_rot;
	float m_nearClipPlane;
	float m_farClipPlane;
	float m_FOV;
	float m_moveSpeed;
	float m_rotSpeed;
};