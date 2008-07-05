#	include "SceneNode3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "DiscreteEntity.h"

#	include "Light.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ObjectImplement.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(SceneNode3D);
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::SceneNode3D()
		: m_interface(0)
		, m_yt( false )
		, m_yawSpeed( 0.0f )
		, m_yawTime( 400.0f )
		, m_yawAddSpeed( 0.0f )
		, m_yawAccelDown( 150.0f )
		, m_yawSpeedLimit( 0.01f )
		, m_yaw( 0.0f )
		, m_pitch( 0.0f )
		, m_yawLimits( -180.0f, 180.0f )
		, m_listener( NULL )
	{}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::~SceneNode3D()
	{
		Holder<RenderEngine>::hostage()->releaseSceneNode( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & SceneNode3D::getWorldOrient()
	{
		return *(mt::quatf*)m_interface->getWorldOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & SceneNode3D::getWorldPosition()
	{
		return *(mt::vec3f*)m_interface->getWorldPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & SceneNode3D::getLocalOrient()
	{
		return *(mt::quatf*)m_interface->getLocalOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & SceneNode3D::getLocalPosition()
	{
		return *(mt::vec3f*)m_interface->getLocalPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setLocalPosition( const mt::vec3f & _position )
	{
		m_interface->setLocalPosition( _position.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setLocalOrient( const mt::quatf & _quat )
	{
		m_interface->setLocalOrient( _quat.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::translate( const mt::vec3f & _delta )
	{
		m_interface->translate( _delta.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::yaw( float _angle )
	{
		m_yaw += _angle;
		if( m_yaw <= m_yawLimits.x )
		{
			m_yaw -= _angle;
			_angle = m_yawLimits.x - m_yaw;
			m_yaw = m_yawLimits.x;
			m_yt = false;
			this->callEvent( "YAW_STOP_LIMIT", "(O)", this->getEmbedding() );
			m_interface->yaw(_angle);
			return;
		}
		else if( m_yaw >= m_yawLimits.y )
		{
			m_yaw -= _angle;
			_angle = m_yawLimits.y - m_yaw;
			m_yaw = m_yawLimits.y;
			m_yt = false;
			this->callEvent( "YAW_STOP_LIMIT", "(O)", this->getEmbedding() );
			m_interface->yaw(_angle);
			return;
		}
		else
		{
			m_interface->yaw( _angle );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::pitch( float _angle )
	{
		m_pitch += _angle;
		m_interface->pitch( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::roll( float _angle )
	{
		m_interface->roll( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::addChild( SceneNode3D * _node )
	{
		m_interface->addChild( _node->m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setScale( const mt::vec3f & _scale )
	{
		m_interface->setScale( (float*)_scale.m);
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::attachToRootNode()
	{
		const std::string & name = getName();
		m_interface = Holder<RenderEngine>::hostage()->createSceneNode( name );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_addChildren( SceneNode3D * _node )
	{
		const std::string & name = _node->getName();
		_node->m_interface = m_interface->createChildSceneNode( name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loaderTransformation_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Position")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setLocalPosition);
				}
			}

			XML_CASE_NODE("Scale")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setScale);
				}
			}

			XML_CASE_NODE("Rotation")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setLocalOrient);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( XmlElement * _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
			{
				XML_PARSE_ELEMENT( this, &SceneNode3D::loaderTransformation_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode3D::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_update( float _timing )
	{
		if( m_yt )
		{
			yaw( m_yawSpeed );

			if( ::fabsf( m_yawSpeed ) > m_yawSpeedLimit )
			{
				m_yawSpeed /= ::powf( 2, _timing / m_yawAccelDown );
			}
			else
			{
				m_yawSpeed = 0.0f;
				m_yt = false;
				this->callEvent( "YAW_STOP", "(O)", this->getEmbedding() );
			}

			m_yawSpeed += m_yawAddSpeed * _timing / m_yawTime;
			m_yawTiming -= _timing;
			if ( m_yawSpeed > m_yawAddSpeed || m_yawSpeed < m_yawAddSpeed || m_yawTiming < 0 )
			{
				m_yawAddSpeed = 0.0f;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::render( bool _enableDebug )
	{
		//NodeRenderable::render();
	};
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode3D::isRenderable()
	{
		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setYawSpeed( float _yawAddSpeed, float _accelDown, float _yawTime )
	{
		m_yt = true;
		m_yawAddSpeed = _yawAddSpeed;
		m_yawTiming = m_yawTime;
		m_yawAccelDown = _accelDown;
		m_yawTime = _yawTime;
		//m_yawSpeed = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setYawLimits( const mt::vec2f& _limits )
	{
		m_yawLimits = _limits;
	}
	//////////////////////////////////////////////////////////////////////////
	float SceneNode3D::getYaw() const
	{
		return m_yaw;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setListener( PyObject* _listener )
	{
		m_listener = _listener;
		this->_onSetListener();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_onSetListener()
	{
		registerEvent("YAW_STOP", "onYawStop", m_listener );
		registerEvent("YAW_STOP_LIMIT", "onYawStopLimit", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setFixedYawAxis( bool _fixed )
	{
		m_interface->setFixedYawAxis( _fixed );
	}
	//////////////////////////////////////////////////////////////////////////
	float SceneNode3D::getPitch() const
	{
		return m_pitch;
	}
	//////////////////////////////////////////////////////////////////////////
}