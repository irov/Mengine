#	include "FFCamera.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Actor.h"
#	include "DiscreteEntity.h"
#	include "ObjectImplement.h"
#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(FFCamera3D);
	//////////////////////////////////////////////////////////////////////////
	FFCamera3D::FFCamera3D()
	: m_actor(0)
	, m_yawAngle(0.f)
	, m_pitchAngle(0.f)
	, m_scrollSpeed(0.f)
	, m_scaleBoundingR(1.f)
	, m_translate(mt::vec3f::zero_v3)
	, m_transOrient(1.f,0.f,0.f,0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FFCamera3D::~FFCamera3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::loader( XmlElement * _xml )
	{
		Camera3D::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "ScrollSpeed", "Value", m_scrollSpeed );
			XML_CASE_ATTRIBUTE_NODE( "ScaleBoundingR", "Value", m_scaleBoundingR );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::_update(float _timing)
	{
		Camera3D::_update(_timing);

		if(m_actor)
		{
			const mt::vec3f & pos = m_actor->getWorldPosition3D();
			float R = m_scaleBoundingR * m_actor->getBoundingRadius();
			float pd = 0;
			mt::vec3f contact(0,0,0);

			for(int i = 2; i < 6; i++)
			{
				if(m_camera->getSphereFrustumContact(
					i,pos.x,pos.y,pos.z,R,pd,contact.x,contact.y,contact.z) == true)
				{
					mt::vec3f camPos = *(mt::vec3f*)m_camera->getPosition();

					mt::vec3f normal = contact - camPos;

					normal.y = 0;
					normal = mt::norm_v3(normal);

					camPos += normal * _timing * m_scrollSpeed;

					m_camera->setPosition(camPos.x,camPos.y,camPos.z);
				}
			}
		}

		m_yawAngle = 0;
		m_pitchAngle = 0;
		m_translate = mt::vec3f::zero_v3;
	}
	//////////////////////////////////////////////////////////////////////////	
	void FFCamera3D::_updateCamera( bool _isOrient )
	{
		m_camera->yaw(m_yawAngle);
		m_camera->pitch(m_pitchAngle);
		
		mt::vec3f transformed = m_translate;

		if(_isOrient == true)
		{
			transformed = m_transOrient * m_translate;
		}

		m_camera->translate(transformed.m);
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::setActor(Actor * _actor)
	{
		m_actor = _actor;
	}
	//////////////////////////////////////////////////////////////////////////
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
		const mt::vec3f & pos = m_actor->getWorldPosition3D();
		float R = m_scaleBoundingR * m_actor->getBoundingRadius();
		float pd = 0;
		mt::vec3f contact(0,0,0);

	//	m_camera->translate(mt::vec3f(0,_dist,0).m);
		if(m_camera->getSphereFrustumContact(
			0,pos.x,pos.y,pos.z,R,pd,contact.x,contact.y,contact.z))
		{
			mt::vec3f camPos = *(mt::vec3f*)m_camera->getPosition();

			mt::vec3f normal = camPos - contact;

			//normal = mt::norm_v3(normal);

			camPos += normal/* * pd*/;

			m_camera->setPosition(camPos.x,camPos.y,camPos.z);
		}
		else

		m_camera->translate(mt::vec3f(0,_dist,0).m);

		//_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::left( float s )
	{
		m_translate.x += s;
		_updateCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FFCamera3D::_activate()
	{
		if(Camera3D::_activate() == false)
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::_deactivate()
	{
		Camera3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void FFCamera3D::_release()
	{
		Camera3D::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FFCamera3D::_compile()
	{
		if(Camera3D::_compile() == false)
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}