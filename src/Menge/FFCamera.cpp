#	include "FFCamera.h"
#	include "Interface/RenderSystemInterface.h"
#	include "RenderEngine.h"
#	include "Holder.h"
#	include "Actor.h"
#	include "DiscreteEntity.h"

namespace Menge
{
	FFCamera3D::FFCamera3D()
	: m_actor(0)
	, m_yawAngle(0)
	, m_pitchAngle(0)
	, m_pitchMinAngle(-20)
	, m_pitchMaxAngle(20)
	, m_translate(mt::vec3f::zero_v3)
	, m_transOrient(1.f,0.f,0.f,0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FFCamera3D::~FFCamera3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::setActor(Actor * _actor)
	{
		m_actor = _actor;
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::update(float _timing)
	{

		const mt::vec3f & pos = m_actor->getWorldPosition();
		float R = 2 * m_actor->getBoundingRadius();
		m_interface->isSphereIntersect(pos.x,pos.y,pos.z,R,m_transOrient.m);

		m_yawAngle = 0;
		m_pitchAngle = 0;
		m_translate = mt::vec3f::zero_v3;
	}
	//////////////////////////////////////////////////////////////////////////	
	void FFCamera3D::_updateCamera( bool _isOrient )
	{
		m_interface->yaw(m_yawAngle);
		m_interface->pitch(m_pitchAngle);
		// двигаемся учитываю ориентацию камеры

		mt::vec3f tt = m_translate;

		if(_isOrient == true)
		{
			tt = m_transOrient * m_translate;
		}

		m_interface->translate(tt.m);
	}
	//
	void FFCamera3D::yaw( float _yaw )
	{
		printf("%f;%f;%f;%f\n",m_transOrient.x,m_transOrient.y,m_transOrient.z,m_transOrient.w);
		mt::rotate_q(m_transOrient, mt::vec3f(0,1,0), _yaw);
		m_yawAngle = _yaw;
		_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::pitch( float _pitch )
	{
		m_pitchAngle = _pitch;
		_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::forward( float s )
	{
		m_translate.z += s;
		_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::zoom( float _dist )
	{
		mt::vec3f pos = m_actor->getWorldPosition();
		pos-=*(mt::vec3f*)m_interface->getPosition();
		printf("%f \n",pos.length());
		if(pos.length()<15 && _dist < 0)
		{
			return;
		}
		
		m_interface->translate(mt::vec3f(0,_dist,0).m);
		_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::left( float s )
	{
		m_translate.x+=s;
		_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FFCamera3D::activate()
	{
		std::string m_name = "FreeCamera";

		m_interface = Holder<RenderEngine>::hostage()->createCamera(
			m_name );

		m_interface->setAspectRatio( 1.3f );
		m_interface->setNearClipDistance( 10.0f );
		m_interface->setFarClipDistance( 1000.0f );

		m_interface->setPosition(0, 16, 30 );

		mt::vec3f pos = *(mt::vec3f*)m_interface->getPosition();
		m_interface->lookAt(pos.x,pos.y - 15,pos.z-0.001);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}