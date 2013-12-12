#include "Camera.h"
#include "Render.h"
#include "Simulation.h"

static Simulation* _sim = Simulation::GetSimulation();

Camera::Camera()
{
	m_springPhys_timeOffset		= 1.f;
	m_gravity					= -100000.f;
	m_spring_minDist			= 200000.f;
	m_springPhys_vertOffset		= 10000.f;
	m_mass						= 75.f;
	m_spring_Str				= 500.f;
	m_spring_DampStr			= 100.f;
	m_spring_EquilibriumDist	= 700000.f;
	m_follow_horizDist			= 4000000;
	m_follow_vertDist			= 4000000;
	m_orbitSpeed				= .2f;
	m_moveState					= USER_INPUT;
	m_pos						= glm::vec3(0,4000000, 0);
	m_rot						= glm::vec3();
	m_nearClipPlane				= 200.0f;
	m_farClipPlane				= 80000000.f;
	m_FOV						= 60.f;
	m_moveSpeed					= 1000000.f;
	m_rotSpeed					= 0.5f;
	m_forward					= glm::vec3(0,0,-1);
	m_right						= glm::vec3(1,0,0);
	m_moveSpeedMulti			= 2.f;
	m_orbitOffset				= glm::vec3( m_follow_horizDist , 0, 0 );


	SetupCamPosFile();
}

void Camera::Update( float _dt, glm::vec2 _mouseDelta )
{
	UpdateMoveState();
	// Always update rotation before translation
	UpdateRot( _dt, _mouseDelta );
	UpdatePos(_dt);
	if(_sim->GetOnKeyDown(SDLK_b))
	{
		SavePosToFile();
	}
}

void Camera::UpdateMoveState( )
{
	if( _sim->GetOnKeyDown( SDLK_r ) )
	{
		int state = (int)m_moveState + 1;
		if(state == (int)CAM_MOVE_STATE_COUNT)
		{
			state = 0;
		}
		
		// State initialize
		if( state == ORBIT)
		{
			glm::vec3 orbitStartPos = m_pos;
			orbitStartPos.y = m_follow_vertDist;
			// WE do it twice to reset the previous pos variable
			SetPos( orbitStartPos );
			SetPos( orbitStartPos );
			m_box.SetPos( glm::vec3(0,0,0) ); // TEST
			m_box.SetPos(  glm::vec3(0,0,0) );// TEST
		}
		m_moveState = (CAM_MOVE_STATE)state;
	}
}

float Camera::UpdateHeight_SpringForce( glm::vec3 _newPos, float _dt, float* _yVel )
{
	//F = -k(|x|-d)(x/|x|) - bv
	// F = final adjustment force
	// k is spring force (the higher the value the tighter the spring)
	// x is the vector difference from a to b(b-a) when applying force to a
	// d is the equilibrium distance between two points
	// b is the the strength of the damper (which forces the spring to equilibrium)
	// v is the relative velocity between the two connected points
	
	glm::vec3 anchorPoint	= _newPos;	
	anchorPoint.y			= _sim->GetHeightOnTerrain( anchorPoint );
	float yDiff				=  _newPos.y - anchorPoint.y;

	//F = -k(|x|-d)(x/|x|) - bv
	float springForce	= (-m_spring_Str) 
							* ( abs(yDiff) - m_spring_EquilibriumDist)
							* (1) 
							- (m_spring_DampStr * m_yVel);
	// This makes the spring only push up the camera
	springForce			= max(springForce, 0);
	float retval 		= _newPos.y;
	m_yAccel			= springForce / m_mass;
	m_yAccel			+= m_gravity;// Gravity
	*_yVel				+= m_yAccel * _dt;
	retval				+= m_yVel * _dt;

	// CLAMP y so we don't go into the ground
	if(yDiff < m_spring_minDist)
	{
		retval = anchorPoint.y + m_spring_minDist;
		*_yVel =  max(*_yVel,0);
	}

	return retval;
}

void Camera::UpdatePos_Orbit_RoseCurve( float _dt )
{
	float tickVal = (SDL_GetTicks() / 1000.f) * m_orbitSpeed;
	float n = 3; // n == number of "rose petals" in this pattern
	float nTickVal = n * tickVal;
	float r = m_follow_horizDist * cos( nTickVal); // r is radius, distance from center
	float zRot = sin( tickVal );
	float xRot = cos( tickVal );
	glm::vec3 dirToCurPoint = glm::normalize( glm::vec3( xRot, 0.f, zRot) ) * r;
	dirToCurPoint.y = m_pos.y;
	dirToCurPoint.y = UpdateHeight_SpringForce( dirToCurPoint, _dt, &m_yVel );

	SetPos( dirToCurPoint );
}

void Camera::Update_Pos_Orbit_Circle( float _dt )
{
	// Here we get the current tick time and then add a time offset to it for physics calculations
	float tickVal = (SDL_GetTicks() / 1000.f + (m_springPhys_timeOffset * 1000.f) ) * m_orbitSpeed;
	float zRot = sin( tickVal );
	float xRot = cos( tickVal );
	
	// Here we calculate two different dirToCurPoints
	// One uses a future position to calculatea future height
	// the other uses the current position to calculate current height
	// whichever is higher is the one we use because we 
	// DO NOT WANT TO CLIP THE GROUND
	float yVel1 = m_yVel,
		yVel2 = m_yVel;
	glm::vec3 dirToCurPoint1 = glm::normalize( glm::vec3( xRot, 0.f, zRot) ) * m_follow_horizDist;
	dirToCurPoint1.y = m_pos.y - m_springPhys_vertOffset;
	dirToCurPoint1.y = UpdateHeight_SpringForce( dirToCurPoint1, _dt, &yVel1 );

	glm::vec3 dirToCurPoint2 = glm::normalize( glm::vec3( xRot, 0.f, zRot) ) * m_follow_horizDist;
	dirToCurPoint2.y = m_pos.y - m_springPhys_vertOffset;
	dirToCurPoint2.y = UpdateHeight_SpringForce( dirToCurPoint2, _dt, &yVel2 );

	glm::vec3 finalPoint = dirToCurPoint1.y > dirToCurPoint2.y ? dirToCurPoint1 : dirToCurPoint2;
	// WE have to update the yvel because UpdateHeight_SpringForce doesn't know
	// what the outcome of this conditional is, it only provides final yVels
	// for both positions
	m_yVel = finalPoint == dirToCurPoint2 ? yVel2 : yVel1; 

	m_box.SetPos( finalPoint ); // ADDED
	SetPos( finalPoint + m_orbitOffset ); // COMMENTED
}

void Camera::UpdatePos_Orbit( float _dt )
{
	
	Update_Pos_Orbit_Circle( _dt );
}

void Camera::UpdatePos_UserInput( float _dt )
{
	glm::vec3 moveDir = glm::vec3();
	glm::vec3 camPlaneForward = glm::cross(glm::vec3(0,1,0), m_right);

	float speed = m_moveSpeed;
	// Speed modifier
	if( _sim->GetKey(SDLK_LSHIFT) )
	{
		speed *= m_moveSpeedMulti;
	}
	// Global Z Axis
	if( _sim->GetKey(SDLK_w) )
	{
		moveDir += speed * camPlaneForward;
	}
	if( _sim->GetKey(SDLK_s))
	{
		moveDir += speed * (-camPlaneForward);
	}
	// Global X Axis
	if( _sim->GetKey(SDLK_a))
	{
		moveDir += speed * (-m_right);
	}
	if( _sim->GetKey(SDLK_d))
	{
		moveDir += speed * (m_right);
	}
	// Global Y Axis
	if( _sim->GetKey(SDLK_q))
	{
		moveDir += glm::vec3(0,-1,0) * (speed/2.f);
	}
	if( _sim->GetKey(SDLK_e))
	{
		moveDir += glm::vec3(0,1,0) * (speed/2.f);
	}

	SetPos( m_pos + (moveDir * _dt) );
}

void Camera::UpdatePos( float _dt )
{
	if(m_moveState == USER_INPUT)
	{
		UpdatePos_UserInput( _dt );
	}
	else
	{
		UpdatePos_Orbit( _dt );
	}
}
void Camera::UpdateRot( float _dt, glm::vec2 _mouseDelta )
{
	if(m_moveState == USER_INPUT )
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
	else
	{
		glm::vec3 forwardDir = m_pos - m_prevPos;
		if( forwardDir != glm::vec3(0,0,0) )
		{
			forwardDir = glm::normalize( forwardDir );
			// Transform the right dir as if in 2D space
			// Right dir can never point up or down.
			m_right = glm::vec3( forwardDir.z, 0, -forwardDir.x );
			m_forward = forwardDir;
			glm::vec3 xzPlaneProjection = m_forward - ( glm::dot( m_forward, glm::vec3(0,1,0) ) * glm::vec3(0,1,0) );
			// +90 to get to proper default orientation
			float yRot = glm::degrees( atan2(xzPlaneProjection.z, xzPlaneProjection.x) ) + 90; 
			float pctMaxHeight = _sim->GetPctMaxHeightTerrain( m_pos );
			float xRot = 0.f;
			// Here we calculate if the camera should look down
			// and if so at what degree of rotation
			float threshold =  .7f;
			if(pctMaxHeight > threshold )
			{
				float pctMaxAngle = glm::clamp( (pctMaxHeight - threshold) / .5f, 0.f, 1.0f);
				xRot += 30 * pctMaxAngle;
			}
			glm::vec3 finalRot = glm::vec3( xRot, yRot, 0);

			SetRot( finalRot );
		}
	}
}

void Camera::DebugRender()
{
	m_box.Render();
}


void Camera::SetPos( glm::vec3 _pos)
{
	m_prevPos = m_pos;
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

glm::mat4 Camera::GetViewMat_NoTrans()
{
	// Returns an identity matrix that has been pushed backwards
	// This mat keeps track of the camera rotation
	glm::mat4 retval = glm::mat4(1.0f);
	retval = glm::rotate(retval, m_rot.x, glm::vec3(1,0,0));
	retval = glm::rotate(retval, m_rot.y, glm::vec3(0,1,0));
	return retval;
}

glm::mat4 Camera::GetProjMat()
{
	// This matrix keeps track of the camera's lens
	glm::mat4 retval = glm::perspective(m_FOV, (float)SCREEN_W / SCREEN_H, m_nearClipPlane, m_farClipPlane );
	return 	retval;
}

void Camera::SetupCamPosFile()
{
	if(CAMERA_POS_CAPTURE)
	{
		// We want to write to the end of the file, no matter what
		m_camPosFile = fopen(CAM_POS_FILENAME, "a");
		if(m_camPosFile == NULL)
		{
			perror("Error opening file");
		}
	}
}

void Camera::SavePosToFile()
{
	if(CAMERA_POS_CAPTURE)
	{
		fprintf(m_camPosFile, "%.2f %.2f %.2f\n", m_pos.x, m_pos.y, m_pos.z);
	}
}

void Camera::CleanupCamPosFile()
{
	if(CAMERA_POS_CAPTURE)
	{
		// close file
		if(fclose(m_camPosFile) != 0)
		{
			perror("Error closing file: ");
		}
	}
}

void Camera::DebugSetup()
{	 
	m_box.Setup();
}	 

void Camera::DebugCleanup()
{
	m_box.Cleanup();

}