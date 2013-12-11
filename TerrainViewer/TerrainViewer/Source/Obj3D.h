#pragma once
#include "RenderObjBase.h"

class Obj3D : public RenderObjBase
{
public:
	Obj3D() : RenderObjBase()
	{ 
		m_position = glm::vec4(0,0,0,1);
		m_scale = 100000.0f;
	}
	virtual void		Render(void);
	virtual glm::mat4	GetModelMat();
	void				SetPos( glm::vec3 _pos );
	float*				GetScale(){return &m_scale;}
	glm::vec4*			GetPos(){return &m_position;}

protected:
	virtual void CreateVBO(void);
	virtual void CreateShaders(void);
	virtual void DestroyVBO(void);
	virtual void DestroyShaders(void);
	void BindForRender();
	void UnbindForRender();
private:
	glm::vec4 m_position;
	glm::vec4 m_prevPos;
	float m_scale;
	GLuint
		ProjectionMatrixUniformLocation,
		ViewMatrixUniformLocation,
		ModelMatrixUniformLocation;
};