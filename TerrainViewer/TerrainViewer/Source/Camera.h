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
protected:
private:
	glm::vec3 m_pos;
	glm::vec3 m_rot;
	float m_nearClipPlane;
	float m_farClipPlane;
	float m_FOV;
};