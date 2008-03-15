#	include "Actor.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine.h"
#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"

#	include "XmlEngine.h"

#	include "DiscreteEntity.h"

#	include "RigidBody3d.h"
#	include "CapsuleController.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Actor )
	//////////////////////////////////////////////////////////////////////////
	Actor::Actor()
	: m_dir(0,0,0)
	, m_initOrient(1,0,0,0)
	, m_contMovement(0,0,0)
	, m_controller(0)
	, m_rigidBody(0)
	, m_resourcenameMesh("")
	, m_moveSpeed( 0.0f, 0.0f, 0.0f )
	, m_yawSpeed( 0.0f )
	, m_pitchSpeed( 0.0f )
	, m_rollSpeed( 0.0f )
	{}
	//////////////////////////////////////////////////////////////////////////
	Actor::~Actor()
	{}
	//////////////////////////////////////////////////////////////////////////
	/*void Actor::setController( CapsuleController * _controller )
	{
		m_controller = _controller;
	}*/
	//////////////////////////////////////////////////////////////////////////
	void Actor::setRigidBody( RigidBody3D * _rb )
	{
		m_rigidBody = _rb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::_update( float _timing )
	{
		SceneNode3D::_update( _timing );

		if( m_controller != NULL )
		{
			/*float step = 0.005f * _timing;

			mt::vec3f workv = mt::vec3f::zero_v3;

			if(m_contMovement != mt::vec3f::zero_v3)
			{
				workv += this->getLocalOrient() * m_contMovement * step;
			}*/
			mt::vec3f workv = getLocalOrient() * m_moveSpeed * _timing * 0.001f;

			if(workv != mt::vec3f::zero_v3)
			{
				m_controller->move( workv.m );

				//mt::vec3f v = m_controller->getFilteredPosition();
				//printf("filt pos = %f; %f; %f\n",v.x,v.y,v.z);

				float* fpos = m_controller->getFilteredPosition();
				this->setLocalPosition( mt::vec3f( fpos[0], fpos[1] - 0.8f, fpos[2] ) );
			}
		}

		yaw( m_yawSpeed * _timing );
		pitch( m_pitchSpeed * _timing );
		roll( m_rollSpeed * _timing );

		for( TStringVector::iterator it = m_playingAnimations.begin(),
				it_end = m_playingAnimations.end(); it != it_end; it++ )
		{
			m_entity->animationAddTime( (*it).c_str(), _timing );
		}

		this->callEvent("UPDATE", "(f)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor::_activate() 
	{
		if( SceneNode3D::_activate() == false )
		{
			return false;
		}
		
	/*	this->registerEvent("MOVE_END", "onMoveEnd" );
		this->registerEvent("MOVE_STOP", "onMoveStop" );
		
		this->registerEvent("ROTATE_END", "onRotateEnd" );
		this->registerEvent("ROTATE_STOP", "onRotateStop" );

		this->registerEvent("UPDATE", "onUpdate" );
*/
		this->callMethod("onActivate", "()" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::_deactivate()
	{
		SceneNode3D::_deactivate();

		this->callMethod("onDeactivate", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor::_compile()
	{
		if( SceneNode3D::_compile() == false )
		{
			return false;
		}

		ResourceMesh* resourceMesh = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceMesh>( m_resourcenameMesh );

		if( resourceMesh == NULL )
		{
			MENGE_LOG( "Error: Actor can't open resource file '%s'\n", m_resourcenameMesh.c_str() );
			return false;
		}

		const std::string & mesh = resourceMesh->getMeshName();
		const std::string & name = this->getName();

		m_entity = Holder<RenderEngine>::hostage()->createEntity(name, mesh);

		m_skeleton = m_entity->getSkeleton();

		m_interface->attachEntity( m_entity );

		// Creating physics controller
		m_controller = Holder<PhysicEngine>::hostage()->createCapsuleController( getWorldPosition() + mt::vec3f( 0.0f, 0.8f, 0.0f ), 0.3f, 1.0f );

		this->callMethod("onCompile", "()" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::_release()
	{
		SceneNode3D::_release();

		//m_interface->attachEntity( NULL );
		Holder<RenderEngine>::hostage()->releaseEntity( m_entity );
		m_entity = NULL;
		m_skeleton = NULL;
		ResourceMesh* resourceMesh = Holder<ResourceManager>::hostage()->getResourceT<ResourceMesh>( m_resourcenameMesh );
		resourceMesh->release();

		if( m_controller )
		{
			Holder<PhysicEngine>::hostage()->releaseCapsuleController( m_controller );
		}
		
		this->callMethod("onRelease", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::step( const mt::vec3f & _v )
	{
//		m_entityInterface->play("idle2");
//		m_entityInterface->setLooped(true);
		m_contMovement = m_initOrient * _v;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::left()
	{
//		m_entityInterface->play("move forward");
///		m_entityInterface->setLooped(true);
//		m_charPos += mt::vec3f(0,0,-1)*1.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::stop()
	{
		m_contMovement = mt::vec3f::zero_v3;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setAnimationEnabled( const std::string& _animName, bool _enable )
	{
		m_entity->setAnimationEnabled( _animName.c_str(), _enable );
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::playAnimation( const std::string& _animName )
	{
		TStringVector::iterator it = std::find( m_playingAnimations.begin(), m_playingAnimations.end(), _animName );
		if( it == m_playingAnimations.end() )
		{
			m_entity->setAnimationEnabled( _animName.c_str(), true );
			m_playingAnimations.push_back( _animName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::pauseAnimation( const std::string& _animName )
	{
		m_playingAnimations.erase( std::find( m_playingAnimations.begin(), m_playingAnimations.end(), _animName ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::stopAnimation( const std::string& _animName )
	{
		m_entity->setAnimationEnabled( _animName.c_str(), false );
		m_entity->setAnimationTimePosition( _animName.c_str(), 0.0f );
		m_playingAnimations.erase( 
			std::remove( m_playingAnimations.begin(), m_playingAnimations.end(), _animName ),
			m_playingAnimations.end() 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::stopAllAnimations()
	{
		for( TStringVector::iterator it = m_playingAnimations.begin(),
				it_end = m_playingAnimations.end(); it != it_end; it++ )
		{
			m_entity->setAnimationEnabled( (*it).c_str(), false );
		}
		m_playingAnimations.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setMoveSpeed( const mt::vec3f& _speed )
	{
		m_moveSpeed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setYawSpeed( float _speed )
	{
		m_yawSpeed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setPitchSpeed( float _speed )
	{
		m_pitchSpeed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setRollSpeed( float _speed )
	{
		m_rollSpeed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
}