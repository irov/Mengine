#	include "Actor.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine.h"
#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"

#	include "Bone.h"

#	include "XmlEngine.h"

#	include "Entity3d.h"

#	include "RigidBody3d.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Actor )
	//////////////////////////////////////////////////////////////////////////
	Actor::Actor()
	: m_dir(0,0,0)
	, m_initOrient(1,0,0,0)
	, m_contMovement(0,0,0)
	, m_body(0)
	, m_controller(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	Actor::~Actor()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setPhysicBody( RigidBody3D * _body )
	{
		m_body = _body;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::_update( float _timing )
	{
		SceneNode3D::_update( _timing );
		if( m_body != NULL )
		{
		//	this->setLocalPosition( m_body->getPosition() );
		//	this->setLocalOrient( m_body->getOrientation() );
		}
		else
		{
			float step = 0.005f * _timing;

			//mt::vec3f workv = mt::vec3f::zero_v3;

			mt::vec3f workv = mt::vec3f::zero_v3;

			if(m_contMovement != mt::vec3f::zero_v3)
			{
				workv += this->getLocalOrient() * m_contMovement * step;
			}

			if(workv != mt::vec3f::zero_v3)
			{
			//		float dist = Holder<PhysicEngine>::hostage()->rayCast("level",getWorldPosition(),mt::vec3f(0,-1,0));
			//		workv.y+= 10 - dist;
				mt::vec3f expp = getWorldPosition();
				//this->translate( workv );

				//printf("Actor = %f %f %f\n", expp.x, expp.y, expp.z);

				//workv +=mt::vec3f(0,-9.8f,0);

				//Holder<PhysicEngine>::hostage()->moveController(workv);
				m_controller->setPosition(workv.m);

				//Holder<PhysicEngine>::hostage()->getControllerPos();
				this->setLocalPosition(
					//Holder<PhysicEngine>::hostage()->getControllerPos()
					*(mt::vec3f*)m_controller->getFilteredPosition()
					);
			}
		}

		m_entityInterface->update(_timing);

		this->callEvent("UPDATE", "(f)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Actor::_activate()
	{
		if( SceneNode3D::_activate() == false )
		{
			return false;
		}
		
		this->registerEventMethod("MOVE_END", "onMoveEnd" );
		this->registerEventMethod("MOVE_STOP", "onMoveStop" );
		
		this->registerEventMethod("ROTATE_END", "onRotateEnd" );
		this->registerEventMethod("ROTATE_STOP", "onRotateStop" );

		this->registerEventMethod("UPDATE", "onUpdate" );

		this->callMethod("onActivate", "()" );

		if( getName() == "Zombie1")
		{
			m_controller = Holder<PhysicEngine>::hostage()->createCapsuleController( this->getWorldPosition(),0.1f,1.0f );
		}

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
	void Actor::attachEntity( Entity3d * _entity )
	{
		m_entityInterface = _entity;
		m_interface->attachEntity( _entity->get() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setParentActor( Actor * _parent, const mt::vec3f & _pos )
	{
		if( _parent == NULL )
		{
			const std::string & name = this->getName();

			SceneNodeInterface * interface
				= Holder<RenderEngine>::hostage()->attachSceneNodeToRoot( name );

			m_interface = interface;
		}
		else
		{
			m_interface = _parent->m_interface;
		}

		this->setLocalPosition( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::setParentRoot( const mt::vec3f & _pos, const mt::quatf & _q )
	{
		const std::string & name = this->getName();

		SceneNodeInterface * interface
			= Holder<RenderEngine>::hostage()->attachSceneNodeToRoot( name );

		m_interface = interface;

		this->setLocalPosition( _pos );
		this->setLocalOrient( _q );
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::step( const mt::vec3f & _v )
	{
		m_entityInterface->play("move forward");
		m_entityInterface->setLooped(true);
		m_contMovement = m_initOrient * _v;
	}
	//////////////////////////////////////////////////////////////////////////
	void Actor::stop()
	{
		m_entityInterface->play("idle1");
		m_contMovement = mt::vec3f::zero_v3;
	}
}