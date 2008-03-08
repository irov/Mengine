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
	//, m_charPos(0,0,0)
	{}
	//////////////////////////////////////////////////////////////////////////
	Actor::~Actor()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setController( CapsuleController * _controller )
	{
		m_controller = _controller;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setRigidBody( RigidBody3D * _rb )
	{
		m_rigidBody = _rb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::_update( float _timing )
	{
		SceneNode3D::_update( _timing );

		if( m_controller != NULL )
		{
			float step = 0.005f * _timing;

			mt::vec3f workv = mt::vec3f::zero_v3;

			if(m_contMovement != mt::vec3f::zero_v3)
			{
				workv += this->getLocalOrient() * m_contMovement * step;
			}

			if(workv != mt::vec3f::zero_v3)
			{
				m_controller->move(workv);
				this->setLocalPosition(
					m_controller->getFilteredPosition()
					);
			}
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

	/*	if( getName() == "Zombie1")
		{
			m_controller = Holder<PhysicEngine>::hostage()->createCapsuleController( this->getWorldPosition(),0.1f,1.0f );
		}
*/
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

		this->callMethod("onCompile", "()" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::_release()
	{
		SceneNode3D::_release();
		
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
//		m_entityInterface->play("idle2");
		m_contMovement = mt::vec3f::zero_v3;
	}
	//////////////////////////////////////////////////////////////////////////
	/*	void Actor::setParentRoot( const mt::vec3f & _pos, const mt::quatf & _q )
	{
	const std::string & name = this->getName();

	SceneNodeInterface * interface
	= Holder<RenderEngine>::hostage()->createSceneNode( name );

	m_interface = interface;

	this->setLocalPosition( _pos );
	this->setLocalOrient( _q );

	}*/
	//////////////////////////////////////////////////////////////////////////
	/*	void Actor::setParentActor( Actor * _parent, const mt::vec3f & _pos )
	{
	if( _parent == NULL )
	{
	const std::string & name = this->getName();

	SceneNodeInterface * interface
	= Holder<RenderEngine>::hostage()->createSceneNode( name );

	m_interface = interface;
	}
	else
	{
	m_interface = _parent->m_interface;
	}

	this->setLocalPosition( _pos );
	}*/
}